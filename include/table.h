#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <memory>
#include <fstream>

#include "storage.h"
#include "pager.h"
#include "cursor.h"

class Cursor;

class Table {
    public:
        uint32_t root_page_num;
        Pager *pager;  
        Table(std::string filename);
        ~Table();

        Cursor* find(uint32_t key);
        void close();
        void create_new_root(uint32_t right_child_page_num);
        void internal_node_insert(uint32_t parent_page_num, uint32_t child_page_num);

    private:
        Cursor* leaf_node_find(uint32_t page_num, uint32_t key);
        Cursor* internal_node_find(uint32_t page_num, uint32_t key);

};


#endif