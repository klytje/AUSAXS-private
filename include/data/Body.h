#pragma once

#include <utility/Concepts.h>
#include <data/detail/AtomCollection.h>
#include <data/DataFwd.h>
#include <io/IOFwd.h>
#include <math/MathFwd.h>

#include <vector>
#include <memory>

namespace grid {class Grid;}
namespace signaller {class Signaller;}
namespace data {
	class Body {
		public:
			/**
			 * @brief Default constructor.
			 * 
			 * This is not a very efficient way of constructing a Body.
			 */
			Body();

			/** 
			 * @brief Create a new collection of atoms (body) from the input .pdb or .xml file. 
			 * 
			 * @param path path to the input file. 
			 * @param signaller a signalling object to signal changes of state
			 */
			explicit Body(const io::File& path);

			/**
			 * @brief Create a new collection of atoms (body) based on two vectors
			 */
			explicit Body(const std::vector<record::Atom>& protein_atoms);

			/**
			 * @brief Create a new collection of atoms (body) based on two vectors
			 */
			Body(const std::vector<record::Atom>& protein_atoms, const std::vector<record::Water>& hydration_atoms);

			/**
			 * @brief Copy constructor. 
			 */
			Body(const Body& body);

			/**
			 * @brief Move constructor. 
			 */
			Body(Body&& body);

			~Body();

			/** 
			 * @brief Writes this body to disk.
			 * 
			 * @param path path to the destination. 
			 */
			void save(const io::File& path);

			/**
			 * @brief Get a reference to the constituent atoms.
			 */
			std::vector<record::Atom>& get_atoms();

			/**
			 * @brief Get a reference to the constituent atoms.
			 */
			const std::vector<record::Atom>& get_atoms() const;

			/**
			 * @brief Get a reference to the hydration atoms.
			 */
			std::vector<record::Water>& get_waters();

			/**
			 * @brief Get a reference to the hydration atoms.
			 */
			const std::vector<record::Water>& get_waters() const;
		
			record::Atom& get_atom(unsigned int index);

			const record::Atom& get_atom(unsigned int index) const;

			/** 
			 * @brief Calculate the center-mass coordinates for the body.
			 * @return The center-mass (x, y, z) coordinates. 
			 */
			Vector3<double> get_cm() const;

			/**
			 * @brief Calculate the volume of this body based on its constituent amino acids
			 */
			[[deprecated]] double get_volume_acids() const;

			/**
			 * @brief Calculate the volume of this body based on the number of C-alpha atoms
			 */
			[[deprecated]] double get_volume_calpha() const;

			// /**
			//  * @brief Generate a PDB file at @p path showing the filled grid volume.
			//  */
			// void generate_volume_file(std::string path);

			/**
			 * @brief Calculate the molar mass of this body in Daltons.
			 */
			double molar_mass() const;

			/**
			 * @brief Get the absolute mass of this body in kg.
			 */
			double absolute_mass() const;

			/**
			 * @brief Get the total atomic charge of this body.
			 */
			double total_atomic_charge() const;

			/**
			 * @brief Get the total effective charge of this body.
			 */
			double total_effective_charge() const;

			/**
			 * @brief Center this Body on origo. 
			 */
			void center();

			/** 
			 * @brief Move the entire body by a vector.
			 * @param v the translation vector
			 */
			void translate(const Vector3<double>& v);

			/**
			 * @brief Rotate all atoms by a given rotation matrix.
			 * 
			 * @param R The rotation matrix. 
			 */
			void rotate(const Matrix<double>& R);
			
			/**
			 * @brief Rotate all atoms @a rad radians about the axis @a axis. 
			 * 
			 * @param axis the rotation axis. 
			 * @param rad the amount to rotate in radians. 
			 */
			void rotate(const Vector3<double>& axis, double rad);

			/**
			 * @brief Euler angle rotation of all atoms. 
			 * 
			 * @param alpha radians to rotate about the z-axis.
			 * @param beta radians to rotate about the y-axis. 
			 * @param gamma radians to rotate about the x-axis. 
			 */
			void rotate(double alpha, double beta, double gamma);

			/**
			 * @brief Subtract the charge of the displaced water molecules from the effective charge of the protein atoms. 
			 * 
			 * @param charge the charge to be subtracted.
			 */
			void update_effective_charge(double charge);

			/**
			 * @brief Register a probe (listener) to this object, which will be notified of state changes. 
			 */
			void register_probe(std::shared_ptr<signaller::Signaller> signal);

			/**
			 * @brief Assign another body to this object. 
			 */
			Body& operator=(const Body& rhs);

			/**
			 * @brief Assign another body to this object. 
			 */
			Body& operator=(Body&& rhs);

			/**
			 * @brief Check if this object is equal to another. 
			 */
			bool operator==(const Body& rhs) const;

			/**
			 * @brief Check if the content of this object is equal to another, disregarding their unique ID. 
			 */
			bool equals_content(const Body& rhs) const;

			/**
			 * @brief Get the File backing this object. 
			 */
			data::detail::AtomCollection& get_file();

			/**
			 * @brief Signal that this object has changed its external state.
			 *        This triggers recalculating all external distances between this body and everything else the next time a histogram is requested. 
			 */
			void changed_external_state() const;

			/**
			 * @brief Signal that this object has changed its internal state.
			 *        This triggers recalculating all distances, both external and internal, between this body and everything else the next time a histogram is requested. 
			 */
			void changed_internal_state() const;

			std::shared_ptr<signaller::Signaller> get_signaller() const;

			[[nodiscard]] int get_id() const;

			/**
			 * @brief Get the total number of constituent atoms, excluding hydration. 
			 */
			[[nodiscard]] std::size_t atom_size() const;

		private:
			int uid;                     				// A unique identifier for this body
			bool updated_charge = false;          		// True if the effective charge of each atom has been updated to reflect the volume they occupy, false otherwise
			bool centered = false;                		// True if this object is centered, false otherwise
			inline static unsigned int uid_counter = 0; // The unique counter. 
			data::detail::AtomCollection file;          // The file backing this body

			// The signalling object to signal a change of state. The default doesn't do anything, and must be overriden by a proper Signaller object.  
			std::shared_ptr<signaller::Signaller> signal;

			void initialize();
	};
}