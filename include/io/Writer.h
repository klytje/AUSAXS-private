#pragma once

#include <string>

namespace io {class File;}
/**
 * @brief Virtual super-class for all data file writers. 
 */
class Writer {
    public:
        virtual ~Writer() = default;

        /**
         * @brief Write the contents of the backing File to a given path. 
         */
        virtual void write(const io::File&) = 0;

        bool operator==(const Writer& rhs) const = default;
};