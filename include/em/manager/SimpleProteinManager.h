#pragma once

#include <em/manager/SmartProteinManager.h>

namespace em::managers {
    class SimpleProteinManager : public SmartProteinManager {
        public: 
            using SmartProteinManager::SmartProteinManager;

            ~SimpleProteinManager() override = default;

        protected:
            void update_protein(double cutoff) override;
    };
}