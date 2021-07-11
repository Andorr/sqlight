#ifndef PAGER_H
#define PAGER_H

#include <fstream>
#include <string>

#include "storage.h"

class Pager {
    public:
        std::fstream file;
        uint32_t file_length;
        void *pages[TABLE_MAX_PAGES];

        Pager(std::string filename);
        ~Pager();
        
        void *get_page(uint32_t page_num);
        void flush(uint32_t page_num, uint32_t size);
        void close();
};

#endif