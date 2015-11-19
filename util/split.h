//
// Created by wastl on 19.11.15.
//

#ifndef MARMOTTA_SPLIT_H
#define MARMOTTA_SPLIT_H

#include <string>
#include <sstream>
#include <vector>

namespace marmotta {
namespace util {

// Split a string at a certain delimiter and add the parts to the vector elems.
std::vector<std::string> &split(const std::string &s, char delim,
                                std::vector<std::string> &elems);

// Split a string, returning a new vector containing the parts.
std::vector<std::string> split(const std::string &s, char delim);

}
}

#endif //MARMOTTA_SPLIT_H
