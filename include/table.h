#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <memory>
#include <fstream>

#include "storage.h"
#include "pager.h"

class Table {
    public:
        uint32_t num_rows;
        Pager *pager;  
        Table(std::string filename);
        ~Table();
        void* row_slot(uint32_t row_num);
        void close();
};


#endif