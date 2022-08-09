# generate lists of files for easy use as dependencies
pymol := pymol

cmake_threads := 6

source := $(addprefix source/, $(shell find source/ -printf "%P "))
include := $(addprefix include/, $(shell find include/ -printf "%P "))

#################################################################################
###				EXECUTABLES					 ###
#################################################################################
.SECONDARY:

docs: build/Makefile
	@ make -C build doc
	firefox build/docs/html/index.html 

.PHONY:
gui: build/source/gui/gui
	build/gui

gwidth := 1
bwidth := 1
ra := 2.4
rh := 1.5
ps := Radial
hydrate/%: build/executable/new_hydration
	$< data/$*.pdb output/$*.pdb --grid_width ${gwidth} --radius_a ${ra} --radius_h ${rh} --placement_strategy ${ps}
	$(pymol) output/$*.pdb -d "hide all; show spheres, hetatm; color orange, hetatm"

view/%: 
	@ structure=$(shell find data/ -name "$*.pdb"); \
	$(pymol) $${structure}

hist/%: build/executable/hist
	@structure=$(shell find data/ -name "$*.pdb"); \
	$< $${structure} figures/ --grid_width ${gwidth} --radius_a ${ra} --radius_h ${rh} --bin_width ${bwidth} --placement_strategy ${ps}

order := ""
rotate/%: build/executable/rotate_map
	$< data/$* ${order}

main/%: build/executable/main
	$< $*

# Fit an EM map to a SAXS measurement file.  
# The wildcard should be the name of a measurement file. All EM maps in the same folder will then be fitted to the measurement.
em_fitter/%: build/executable/em_fitter
	@ measurement=$(shell find data/ -name "$*.RSR" -or -name "$*.dat"); \
	folder=$$(dirname $${measurement}); \
	emmaps=$$(find $${folder}/ -name "*.map" -or -name "*.ccp4"); \
	for emmap in $${emmaps}; do\
		echo "Fitting " $${emmap} " ...";\
		sleep 1;\
		$< $${emmap} $${measurement};\
	done

# Fit both an EM map and a PDB file to a SAXS measurement. 
# The wildcard should be the name of a measurement file. All EM maps in the same folder will then be fitted to the measurement. 
em/%: build/executable/em
	@ structure=$(shell find data/ -name "$*.pdb"); \
	measurement=$(shell find data/ -name "$*.RSR" -or -name "$*.dat"); \
	emmap=$(shell find data/ -name "*$*.map" -or -name "*$*.ccp4"); \
	$< $${emmap} $${structure} $${measurement}

optimize_radius/%: build/source/scripts/optimize_radius
	$< data/$*.pdb figures/

# Perform a rigid-body optimization of the input structure. 
# The wildcard should be the name of both a measurement file and an associated PDB structure file. 
rigidbody/%: build/executable/rigidbody
	@ structure=$(shell find data/ -name "$*.pdb"); \
	measurement=$(shell find data/ -name "$*.RSR" -or -name "$*.dat"); \
	$< $${structure} $${measurement} figures/

qlow := 0
qhigh := 1000
center := center
options :=
intensity_fit/%: build/executable/intensity_fitter
	@ structure=$(shell find data/ -name "$*.pdb"); \
	measurement=$(shell find data/ -name "$*.RSR" -or -name "$*.dat"); \
	$< $${structure} $${measurement} -o figures/ --qlow ${qlow} --qhigh ${qhigh} --${center} --radius_a ${ra} --radius_h ${rh} --grid_width ${gwidth} --bin_width ${bwidth} --placement_strategy ${ps} ${options}

# Check the consistency of the program. 
# The wildcard should be the name of an EM map. A number of SAXS measurements will be simulated from the map, and then fitted to it. 
consistency/%: build/executable/consistency
	@ map=$(shell find data/ -name "*$*.map" -or -name "*$*.ccp4"); \
	$< $${map}

res := 20
# usage: make fit_consistency/2epe map=10
# Check the consistency of the program. 
# The wildcard should be the name of both a measurement file and an associated PDB structure file. 
# A simulated EM map must be available. The resolution can be specified with the "res" argument. 
fit_consistency/%: build/executable/fit_consistency
	@ structure=$(shell find data/ -name "$*.pdb"); \
	measurement=$(shell find data/ -name "$*.RSR" -or -name "$*.dat"); \
	emmap=$(shell find sim/ -name "$*_${res}.ccp4" -or -name "$*_${res}.mrc"); \
	echo "./fit_consistency $${structure} $${measurement} $${emmap}\n"; \
	$< $${emmap} $${structure} $${measurement}

# Rebin a SAXS measurement file. This will dramatically reduce the number of data points. 
# The wildcard should be the name of a SAXS measurement file. 
rebin/%: build/executable/rebin
	@ measurement=$(shell find data/ -name "$*.RSR" -or -name "$*.dat"); \
	$< $${measurement}

# Calculate a unit cell and write it to the file as a CRYST1 record. 
# The wildcard should be the name of a PDB structure file. 
unit_cell/%: build/executable/unit_cell
	@ structure=$(shell find data/ -name "$*.pdb"); \
	$< $${structure}

#################################################################################
###			     SIMULATIONS					 ###
#################################################################################
simprog := ~/tools/EMAN/bin/pdb2mrc
simulate/%: 
	@ structure=$(shell find data/ -name "$*.pdb"); \
	$(simprog) $${structure} sim/$*_$(res).mrc res=$(res) het center

simfit/%: build/executable/fit_consistency
	@ structure=$(shell find data/ -name "$*.pdb"); \
	measurement=$(shell find data/ -name "$*.RSR" -or -name "$*.dat"); \
	$(simprog) $${structure} sim/$*_$(res).mrc res=$(res) het center; \
	echo "./fit_consistency $${structure} $${measurement} sim/$*_$(res).mrc\n"; \
	$< sim/$*_$(res).mrc $${structure} $${measurement}

old_simulate/%: 
	@ structure=$(shell find data/ -name "$*.pdb"); \
	phenix.fmodel $${structure} high_resolution=$(res);\
	phenix.mtz2map mtz_file=$(*F).pdb.mtz labels=FMODEL,PHIFMODEL output.prefix=$(*F) pdb_file=$${structure};\
	rm $(*F).pdb.mtz;\
	mv $(*F)_fmodel.ccp4 sim/$(*F)_$(res).ccp4;\

stuff/%: build/executable/stuff data/%.pdb
#	@$< data/$*.pdb sim/native_20.ccp4 sim/native_21.ccp4 sim/native_22.ccp4 sim/native_23.ccp4
	@$< data/$*.pdb $(shell find sim/ -name "$**" -printf "%p\n" | sort | awk '{printf("%s ", $$0)}')

#################################################################################
###				TESTS						 ###
#################################################################################
tags := ""
exclude_tags := "~[broken] ~[manual] ~[slow] ~[disable]"
memtest/%: $(shell find source/ -print) test/%.cpp	
	@ make -C build test -j${cmake_threads}
	valgrind --suppressions=suppressions.txt --track-origins=yes --log-file="valgrind.txt" build/test [$(*F)] ${tags}

test/%: $(shell find source/ -print) test/%.cpp
	@ make -C build test -j${cmake_threads}
	build/test [$(*F)] ~[slow] ~[broken] ${tags}

tests: $(shell find source/ -print) $(shell find test/ -print)
	@ make -C build test -j${cmake_threads}
	build/test $(exclude_tags) ~[memtest]

# special build target for our tests since they obviously depend on themselves, which is not included in $(source_files)
build/source/tests/%: $(shell find source/ -print) build/Makefile
	@ make -C build $* -j${cmake_threads}

#################################################################################
###				BUILD						 ###
#################################################################################
.PHONY: build
build: 
	@ mkdir -p build; 
	@ cd build; cmake ../

build/executable/%: $(source) $(include) executable/%.cpp
	@ cmake --build build/ --target $(*F) -j${cmake_threads} 

build/%: $(source) $(include)
	@ cmake --build build/ --target $(*F) -j${cmake_threads} 
	
build/Makefile: $(shell find -name "CMakeLists.txt" -printf "%P ")
	@ mkdir -p build
	@ cd build; cmake ../
	
clean/build: 
	@ rmdir -f build

