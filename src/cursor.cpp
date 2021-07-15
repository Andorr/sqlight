#include <iostream>

#include "storage.h"
#include "cursor.h"

void* Cursor::value() {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = this->table.pager->get_page(page_num);
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return ((uint8_t*)page) + byte_offset;
}

void Cursor::advance() {
    this->row_num += 1;
    if(this->row_num >= this->table.num_rows) {
        this->end_of_table = true;
    }
}

Cursor Cursor::from_start(Table &table) {
    return Cursor(table, 0, false);
}

Cursor Cursor::from_end(Table &table) {
    return Cursor(table, table.num_rows, true);
}