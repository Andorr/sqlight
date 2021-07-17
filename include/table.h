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

    private:
        Cursor* leaf_node_find(uint32_t page_num, uint32_t key);
};


#endif