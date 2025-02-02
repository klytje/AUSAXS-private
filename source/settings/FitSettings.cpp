/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <settings/FitSettings.h>
#include <settings/SettingsIORegistry.h>

bool settings::fit::verbose = false;
unsigned int settings::fit::N = 100;
unsigned int settings::fit::max_iterations = 100;

namespace settings::fit::io {
    settings::io::SettingSection general_settings("General", {
        settings::io::create(verbose, "fit-verbose"),
        settings::io::create(N, "N"),
        settings::io::create(max_iterations, "max_iterations")
    });
}