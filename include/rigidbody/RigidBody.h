#pragma once

#include <memory>

#include "data/Protein.h"
#include "rigidbody/Constraint.h"
#include "rigidbody/BodySelectStrategy.h"
#include "rigidbody/TransformationStrategy.h"
#include "rigidbody/ParameterGenerationStrategy.h"
#include "fitter/IntensityFitter.h"

class RigidBody {
  public:
    /**
     * @brief Construtor. 
     * 
     * Prepare a new rigid body for optimization. 
     * 
     * @param protein The protein to be optimized. 
     */
    RigidBody(Protein& protein) : protein(protein) {}

    /**
     * @brief Perform a rigid-body optimization for this structure. 
     */
    void optimize();

    /**
     * @brief Add a constraint to this rigid body. 
     */
    void add_constraint(const Constraint& constraint);

    /**
     * @brief Create a constraint for this rigid body. 
     * 
     * This method is linear in the total number of atoms. For constant efficiency, also provide pointers to the bodies the two atoms are part of. 
     */
    void create_constraint(const Atom* const atom1, const Atom* const atom2);

    /**
     * @brief Create a constraint for this rigid body. 
     */
    void create_constraint(const Atom* const atom1, const Atom* const atom2, const Body* const body1, const Body* const body2);

    /**
     * @brief Create a constraint for this rigid body. 
     * 
     * This method is linear in the total number of atoms. For constant efficiency, also provide the bodies the two atoms are part of. 
     */
    void create_constraint(const Atom& atom1, const Atom& atom2);

    /**
     * @brief Create a constraint for this rigid body. 
     */
    void create_constraint(const Atom& atom1, const Atom& atom2, const Body& body1, const Body& body2);

    Protein& protein;
    std::vector<Constraint> constraints;

  private:
    std::unique_ptr<BodySelectStrategy> body_selector;
    std::unique_ptr<TransformationStrategy> transform;
    std::unique_ptr<ParameterGenerationStrategy> parameter_generator;

    void driver(const string& measurement_path);

    /**
     * @brief Perform a single step of the optimization, and calculate the resulting chi2 value. 
     */
    double chi2(IntensityFitter& fitter) const;

    /**
     * @brief Rotate a body with the currently chosen transformation strategy. 
     */
    void rotate();

    /**
     * @brief Translate a body with the currently chosen transformation strategy. 
     */
    void translate();

    /**
     * @brief Find the bodies containing the argument atoms. Helper method for create_constraint. 
     *        This is linear in the total number of atoms. 
     * 
     * @param atom1 The first return value will be a pointer to the host body of this atom. 
     * @param atom2 The second return value will be a pointer to the host body of this atom. 
     */
    std::pair<const Body*, const Body*> find_host_bodies(const Atom* const atom1, const Atom* const atom2) const noexcept(false);
};