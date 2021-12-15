#include <vector>
#include <utility>

using std::vector;

/**
 * @brief A state manager which keeps track of changes in each body. 
 *        This is meant to be used in conjunction with DistanceCalculator, such that it only recalculates what is absolutely necessary. 
 */
class StateManager {
    public:

        /**
         * @brief A small probe for signalling changes which can be dispatched to other classes. 
         */
        class Signaller {
            public: 
                Signaller(const int& id, StateManager* const owner) : owner(owner), id(id) {}

                /**
                 * @brief Signal that the state of this object has changed. 
                 */
                void state_change() const {owner->modified(id);}

            private: 
                StateManager* const owner;
                const int id;
        };

        StateManager(const int& size) : size(size), _modified(size, true), _modified_hydration(true) {
            for (int i = 0; i < size; i++) {probes.push_back(std::make_shared<Signaller>(i, this));}
        }

        /**
         * @brief Mark that the protein atoms of all bodies were modified. 
         */
        void modified_all() {
            _modified = vector<bool>(size, true);
        }

        /**
         * @brief Mark that the protein atoms of a body was modified.
         * @param i index of the body. 
         */
        void modified(const int& i) {
            _modified[i] = true;
        }

        /**
         * @brief Mark that the hydration atoms of a body was modified.
         * @param i index of the body. 
         */
        void modified_hydration_layer() {
            _modified_hydration = true;
        }

        /**
         * @brief Reset all marks to false.
         */
        void reset() {
            _modified = vector<bool>(size, false);
            _modified_hydration = false;
        }

        /**
         * @brief Get a pointer to the @a ith probe so it can be dispatched to other classes.
         */
        std::shared_ptr<Signaller> get_probe(const int& i) {
            return probes[i];
        }

    private:
        const int size;
        vector<bool> _modified;
        bool _modified_hydration;
        vector<std::shared_ptr<Signaller>> probes;
};