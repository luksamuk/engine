#include "io.hpp"
#include <fstream>

std::string
slurp_file(const char *path)
{
    std::ifstream ifs(path);
    return std::string((std::istreambuf_iterator<char>(ifs)),
                       (std::istreambuf_iterator<char>()));
}
