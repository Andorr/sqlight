#include <iostream>

#include "storage.h"
#include "table.h"
#include "util.h"
#include "pager.h"

Table::Table(std::string filename) {
    Pager *pager = new Pager(filename);
    this->num_rows = pager->file_length / ROW_SIZE;
    this->pager = pager;
}

Table::~Table() {
    close();
}

void Table::close() {
    uint32_t num_full_pages = num_rows / ROWS_PER_PAGE;

    // Flush memory
    for(uint32_t i = 0; i < num_full_pages; i++) {
        if(pager->pages[i] == NULL) {
            continue;
        }
        pager->flush(i, PAGE_SIZE);
        pager->pages[i] = NULL;
    }
    
    // There may be a partial page to write to the end of the file
    // This should not be needed after we switch to a B-tree
    uint32_t num_additional_rows = num_rows % ROWS_PER_PAGE;
    if(num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if(pager->pages[page_num] != NULL) {
            pager->flush(page_num, num_additional_rows * ROW_SIZE);
            pager->pages[page_num] = NULL;
        }
    }

    delete pager;
}

