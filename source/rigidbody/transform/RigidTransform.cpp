#include <rigidbody/transform/RigidTransform.h>
#include <rigidbody/RigidBody.h>

#include <unordered_set>

using namespace rigidbody;

RigidTransform::RigidTransform(RigidBody* rigidbody) : TransformStrategy(rigidbody) {}

RigidTransform::~RigidTransform() = default;

void RigidTransform::apply(const Matrix<double>& M, const Vector3<double>& t, std::shared_ptr<Constraint> constraint) {
    auto group = get_connected(constraint);
    backup(group);

    // remove the bodies from the grid
    auto grid = rigidbody->get_grid();
    for (auto& body : group.bodies) {
        grid->remove(body);
    }

    rotate(M, group);
    translate(t, group);

    // add them back to the grid
    for (auto& body : group.bodies) {
        grid->add(body);
    }
}

TransformStrategy::TransformGroup RigidTransform::get_connected(std::shared_ptr<Constraint> pivot) {
    std::function<void(unsigned int, std::unordered_set<unsigned int>&)> explore_branch = [&] (unsigned int ibody, std::unordered_set<unsigned int>& indices) {
        if (indices.contains(ibody)) {
            return;
        }
        indices.insert(ibody);

        for (const auto& constraint : rigidbody->constraint_map[ibody]) {
            if (constraint->ibody1 == ibody) {
                explore_branch(constraint->ibody2, indices);
            } else {
                explore_branch(constraint->ibody1, indices);
            }
        }
        return;
    };

    // explore all branches
    std::unordered_set<unsigned int> _path1({pivot->ibody2});
    std::unordered_set<unsigned int> _path2({pivot->ibody1});
    explore_branch(pivot->ibody1, _path1);
    explore_branch(pivot->ibody2, _path2);
    _path1.erase(pivot->ibody2);
    _path2.erase(pivot->ibody1);
    std::vector<unsigned int> path1(_path1.begin(), _path1.end());
    std::vector<unsigned int> path2(_path2.begin(), _path2.end());

    // if the paths are the same length, we just return the pivot as the only body in the group
    if (path1.size() == path2.size() && path1 == path2) {
        return TransformGroup({&rigidbody->bodies[pivot->ibody1]}, {pivot->ibody1}, pivot, pivot->get_atom1().coords);
    }

    // create a vector of pointers to the bodies in the paths
    std::vector<Body*> bodies1, bodies2;
    for (const auto& ibody : path1) {
        bodies1.push_back(&rigidbody->bodies[ibody]);
    }
    for (const auto& ibody : path2) {
        bodies2.push_back(&rigidbody->bodies[ibody]);
    }

    // check if the system is overconstrained
    if (0.5*rigidbody->body_size() < path1.size() && 0.5*rigidbody->body_size() < path2.size()) {
        throw except::size_error("TransformStrategy::get_connected: The system is overconstrained. Use a different TransformStrategy.");
    }

    // if the paths are different lengths, we return the shorter path as the group
    if (path1.size() < path2.size()) {
        return TransformGroup(bodies1, path1, pivot, pivot->get_atom1().coords);
    } else {
        return TransformGroup(bodies2, path2, pivot, pivot->get_atom2().coords);
    }
}