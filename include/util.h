#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>

namespace util {
    std::vector<std::string> string_split(std::string s, char delimiter = ' ');

    void serialize_string(std::string s, uint8_t *dest);
    std::string deserialize_string(std::string &s, uint8_t *source);
}



#endif