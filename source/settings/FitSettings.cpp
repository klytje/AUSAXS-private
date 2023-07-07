#include <settings/FitSettings.h>
#include <settings/SettingsIORegistry.h>

namespace settings::fit {
    bool verbose = false;
    unsigned int N = 100;
    unsigned int max_iterations = 100;

    namespace io {
        settings::io::SettingSection general_settings("General", {
            settings::io::create(verbose, "fit-verbose"),
            settings::io::create(N, "N"),
            settings::io::create(max_iterations, "max_iterations")
        });
    }

}