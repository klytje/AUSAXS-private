#include <io/ExistingFile.h>
#include <utility/Exceptions.h>

io::ExistingFile::ExistingFile(const File& file) : File(file) {
    validate();
}

io::ExistingFile::ExistingFile(File&& file) : File(std::move(file)) {
    validate();
}

io::ExistingFile::ExistingFile(const std::string& path) : File(path) {
    validate();
}

io::ExistingFile& io::ExistingFile::operator=(const std::string& path) {
    validate();
    File::operator=(path);
    return *this;
}

void io::ExistingFile::validate() const {
    if (!exists()) {
        throw except::io_error("io::ExistingFile: File \"" + path() + "\" does not exist.");
    }
}