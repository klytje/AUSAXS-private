# Todo
 * [ ] Use nothrow
 * [ ] udregn præcis voxel størrelse for alle resolutions
 * [ ] gentag alt for de andre filer
 * [x] residual plots for em
 * [ ] flow diagram med hvad vi rent faktisk laver
 * [ ] chi2 vs opløsning for optimeret fit
 * [x] sammenligningsfigur separer kurver med faktor x^n
 * [ ] Fix fitter for EM
 * [ ] Test higher resolutions of native
 * [ ] Check if there are online databases of EM maps
 * [x] PlotOptions: Refactor to better handling similar to sim3a.
 * [ ] General: Consistency check of DrhoM
 * [ ] EM: Research and implement Electron Transfer Function (if not too difficult)
 * [ ] Atom: Add define statement controlling safety checks on set_element, get_mass and get_Z
 * [ ] Atom: Rename "effective charge" to "relative charge". Apply this change globally. 
 * [ ] EM: Do a better job of simulating experimental data (uncertainties, Gaussian noise, better spacing). Do a check on the voxel sizes and skip every Nth pixel if it is too small. 
 * [ ] IO: Support multiple terminate statements
 * [ ] Memory test all other executables.
 * [ ] plots: Change intensity plots to static methods
 * [ ] General: Determine where hydration_atoms should be stored. The Protein class seems like the best choice. 
 * [ ] Atom: Const uid 
 * [ ] Constants: Rethink how to determine charge densities for arbitrary ligands
 * [ ] Try to derive an analytical solution of the chi2 problem. Differentiate chi2 with respect to each variable, and set each expression equal to zero. As long as it's not an iterative equation, it should be good. 

# Stuff to consider
## Grid:
 * Consider simply calculating and using the bounding box at initialization as the entire grid instead of wasting memory
 * Consider how to improve culling method
 * Consider removing all bounds checks

## ScatteringHistogram:
 * Optional argument of q-values to calculate I(q) for - this would remove the necessity of splicing in the IntensityFitter
 * Take a closer look at the form factor
 * Convert `a` to a more sensible output (units)

## Body:
 * Consider removing hydration_atoms, they're not supposed to be used anyway

## Protein: 
 * When calculating the volume of the acids, the calculation is simply delegated to each individual body. However, if an amino acid happens to be cut in two halves in two different bodies, its volume is counted twice. 

# Dependencies
Maybe bundle them somehow to make it easier to install?
 * ROOT (compile options: `cmake -DCMAKE_INSTALL_PREFIX=<install> -Dminuit2=ON -DCMAKE_CXX_STANDARD=17 -Dbuiltin_gsl=ON <source>`)
 * Boost (Very minor dependency, consider removing it entirely.)
 * Elements
 * CLI11
 * catch2 for tests

# FITTING:
FOXS SAXS fitting program
ATSAS CRYSOL

# Other personal notes
## Articles
 * Joachim (Jochen) Hub - molecular dynamics
 * Aquaporin DDM
 * B. Jacrot rep prog phys ~ 1980
 * Peter Zipper ~1980
 * ATSAS Crysol
