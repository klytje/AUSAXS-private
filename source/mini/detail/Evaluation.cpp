/*
This software is distributed under the GNU General Public License v3.0. 
For more information, please refer to the LICENSE file in the project root.
*/

#include <mini/detail/Evaluation.h>

#include <string>

std::string mini::Evaluation::to_string() const {
    std::string s;
    for (auto val : vals) {
        s += std::to_string(val) + " ";
    }
    s += std::to_string(fval);
    return s;
}