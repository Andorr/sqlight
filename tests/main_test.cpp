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
#include <sstream>

#include "catch_amalgamated.hpp"
#include "storage.h"
#include "node.h"
#include "util.h"
#include "row.h"

TEST_CASE( "string_split must split string", "[split]" ) {
    std::vector<std::string> result = util::string_split("Hello world! :D", ' ');
    REQUIRE( result.size() == 3);
    
    std::vector<std::string> expected = {"Hello", "world!", ":D"};
    for(std::vector<std::string>::size_type i = 0; i < expected.size(); i++) {
        REQUIRE(expected[i] == result[i]);
    }
}


TEST_CASE("leaf_node utility methods", "[layout]") {

    void* page = std::malloc(PAGE_SIZE);
    uint8_t *node = (uint8_t*)page;

    set_node_type(page, NODE_LEAF);
    set_node_root(page, true);

    

    std::size_t num_cells = 13;
    *leaf_node_num_cells(page) = num_cells;


    for(size_t i = 0; i < num_cells; i++) {
        *(uint32_t*)(leaf_node_key(page, i)) = i + 1;
        void *value = leaf_node_value(page, i);

        std::stringstream username;
        username << "user" << i;

        std::stringstream email;
        email << "user" << i << "@example.com";

        Row r;
        r.id = i + 100;
        r.username = username.str();
        r.email = email.str();
        r.serialize(value);
    }


    REQUIRE(*(node + NODE_TYPE_OFFSET) == 1);
    REQUIRE(*(node + IS_ROOT_OFFSET) == 1);
    REQUIRE(*(node + LEAF_NODE_NUM_CELLS_OFFSET) == num_cells);

    for(std::size_t i = 0; i < num_cells; i++) {
        REQUIRE(*(leaf_node_key(node, i)) == i + 1);

        void *value = leaf_node_value(page, i);
        
        auto r = Row::deserialize(value);
        REQUIRE(r->id == i + 100);

        std::stringstream username;
        username << "user" << i;

        std::stringstream email;
        email << "user" << i << "@example.com";

        REQUIRE(r->username == username.str());
        REQUIRE(r->email == email.str());
    }

}