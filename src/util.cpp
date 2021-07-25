#include<iostream>
#include<vector>
#include <ranges>
#include <memory>
#include <cstring>

#include "util.h"


std::vector<std::string> util::string_split(std::string s, char delimiter) {
    auto r = s 
        | std::views::split(delimiter)
        | std::views::transform([](auto p){
            auto c = p | std::views::common;
            return std::string(c.begin(), c.end());    
        });
    std::vector v(std::ranges::begin(r), std::ranges::end(r));
    return v;
}

void util::serialize_string(std::string s, uint8_t *dest) {
    std::size_t size = s.size();
    std::memcpy(dest, &size, sizeof(std::size_t));
    std::memcpy(dest + sizeof(std::size_t), (char*)s.c_str(), size);
}

std::string util::deserialize_string(std::string &s, uint8_t *source) {
    std::size_t size;
    std::memcpy(&size, source, sizeof(std::size_t));
    return s.assign((char*)(source + sizeof(std::size_t)), size);
}