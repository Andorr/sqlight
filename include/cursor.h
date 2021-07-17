#ifndef CURSOR_H
#define CURSOR_H

#include "table.h"
#include "row.h"

class Table;

class Cursor {

    public:
        Table &table;
        uint32_t page_num;
        uint32_t cell_num;
        bool end_of_table;

        Cursor(Table &table, uint32_t page_num, uint32_t cell_num, bool end_of_table): 
            table(table), 
            page_num(page_num),
            cell_num(cell_num), 
            end_of_table(end_of_table)
        {};

        void* value();
        void advance();
        void insert(uint32_t key, Row* value);
        
        static Cursor from_start(Table &table);
};

#endif