#pragma once

#include <rigidbody/RigidbodyFwd.h>
#include <rigidbody/detail/RigidbodyInternalFwd.h>
#include <utility/Concepts.h>
#include <math/MathFwd.h>

#include <vector>

namespace rigidbody::transform {
    /**
     * @brief TransformStrategy. 
     * 
     * This super-class defines the interface for the body transformation strategies for the rigid-body optimization. 
     * More specifically its implementations essentially specifies how other connected bodies are affected by a transformation. 
     */
    class TransformStrategy {
        public:
            /**
             * @brief Construtor. 
             */
            TransformStrategy(RigidBody* rigidbody);

            /**
             * @brief Destructor.
             */
            virtual ~TransformStrategy();

            /**
             * @brief Apply a transformation to a body. 
             * 
             * The most recent transformation can be undone by calling undo().
             * 
             * @param M The rotation matrix.
             * @param t The translation vector. 
             * @param constraint The constraint to transform along.
             */
            virtual void apply(const Matrix<double>& M, const Vector3<double>& t, constraints::DistanceConstraint& constraint) = 0;

            /**
             * @brief Undo the previous transformation. 
             */
            virtual void undo();

        protected: 
            RigidBody* rigidbody;

            std::vector<BackupBody> bodybackup;

            /**
             * @brief Create a backup of the bodies in the group.
             */
            void backup(TransformGroup& group);

            /**
             * @brief Rotate a body. 
             * 
             * @param M The rotation matrix.
             * @param group The group to apply the rotation to.
             */
            virtual void rotate(const Matrix<double>& M, TransformGroup& group);

            /**
             * @brief Translate a body. 
             * 
             * @param t The translation vector. 
             * @param constraint The group to apply the translation to. 
             */
            virtual void translate(const Vector3<double>& t, TransformGroup& group);
    };
}