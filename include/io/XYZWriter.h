#pragma once

#include <data/Protein.h>
#include <io/File.h>

#include <fstream>

namespace io {
    /**
     * @brief This class can write .xyz trajectory files.
     */
    class XYZWriter {
        public:
            XYZWriter(const io::File& path);

            ~XYZWriter();

            /**
             * @brief Write a frame to the file.
             */
            void write_frame(const Protein* protein);

        private:
            std::ofstream file;
            std::string path;
    };
}