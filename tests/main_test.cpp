#define CONFIG_CATCH_MAIN
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <ranges>
#include <string_view>

#include "catch_amalgamated.hpp"
#include "util.h"

TEST_CASE( "string_split must split string", "[split]" ) {
    std::vector<std::string> result = util::string_split("Hello world! :D", ' ');
    REQUIRE( result.size() == 3);
    
    std::vector<std::string> expected = {"Hello", "world!", ":D"};
    for(std::vector<std::string>::size_type i = 0; i < expected.size(); i++) {
        REQUIRE(expected[i] == result[i]);
        i++;
    }
}