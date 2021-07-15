#ifndef CURSOR_H
#define CURSOR_H

#include "table.h"

class Cursor {

    public:
        Table &table;
        uint32_t row_num;
        bool end_of_table;

        Cursor(Table &table, uint32_t row_num, bool end_of_table): 
            table(table), 
            row_num(row_num), 
            end_of_table(end_of_table)
        {};

        void* value();
        void advance();

        static Cursor from_start(Table &table);
        static Cursor from_end(Table &table);
};

#endif