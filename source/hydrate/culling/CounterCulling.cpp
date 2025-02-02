/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <hydrate/culling/CounterCulling.h>
#include <hydrate/GridMember.h>
#include <hydrate/Grid.h>
#include <data/record/Water.h>

#include <cmath>

using namespace data::record;

std::vector<data::record::Water> grid::CounterCulling::cull(std::vector<grid::GridMember<Water>>& placed_water) const {
    if (target_count == 0) {
        std::vector<Water> final_water(placed_water.size());
        std::transform(placed_water.begin(), placed_water.end(), final_water.begin(), [] (GridMember<Water>& gm) {return gm.get_atom();});
        return final_water;
    }

    int factor = std::floor(placed_water.size()/target_count); // reduction factor
    if (factor < 2) {
        std::vector<Water> final_water(placed_water.size());
        std::transform(placed_water.begin(), placed_water.end(), final_water.begin(), [] (GridMember<Water>& gm) {return gm.get_atom();});
        return final_water;
    }

    std::vector<Water> final_water(placed_water.size()); // the final water molecules that will be used
    std::vector<Water> removed_water(placed_water.size()); // the water molecules which will be removed
    unsigned int rm_index = 0; // current index in removed_water
    unsigned int pw_index = 0; // current index in placed_water
    unsigned int counter = 0; // counter
    for (const auto& a : placed_water) {
        counter++;
        if (counter % factor != 0) {
            removed_water[rm_index++] = a.get_atom();
            continue;
        }
        final_water[pw_index++] = a.get_atom();
    }
    removed_water.resize(rm_index);
    final_water.resize(pw_index);
    grid->remove(removed_water);
    return final_water;
}