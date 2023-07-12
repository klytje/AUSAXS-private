#include <em/ImageStack.h>

#include <iostream>

int main(int argc, char const *argv[]) {
    std::string mfile = argv[1];
    em::ImageStack map(mfile);

    std::cout << *map.get_header() << std::endl;
    std::cout << "1σ level: " << map.from_level(1) << std::endl;

    return 0;
}