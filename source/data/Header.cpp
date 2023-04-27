#include <data/Header.h>

Record::RecordType Header::get_type() const {return RecordType::HEADER;}

void Header::parse_pdb(const std::string& s) {add(s);}

std::string Header::as_pdb() const {return get();}

void Header::add(const std::string& s) {contents.push_back(s);}

std::string Header::get() const {return utility::join(contents, "\n") + (size() == 0 ? "" : "\n");}

void Header::remove(const std::string& type) {
    auto t = type + std::string(6 - type.size(), ' ');
    std::vector<std::string> new_contents;
    new_contents.reserve(contents.size());
    for (unsigned int i = 0; i < contents.size(); i++) {
        if (contents[i].substr(0, 6) != t) {
            new_contents.push_back(contents[i]);
        }
    }
    contents = std::move(new_contents);
}

unsigned int Header::size() const {return contents.size();}