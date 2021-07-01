#include "table.h"

#include <cstring>
#include <memory>

void* Row::serialize(void *destination) {
    uint8_t *dest = (uint8_t*)destination;
    std::memcpy(dest + ID_OFFSET, &id, ID_SIZE);
    std::memcpy(dest + USERNAME_OFFSET, username, USERNAME_SIZE);
    std::memcpy(dest + EMAIL_OFFSET, email, EMAIL_SIZE);
    return dest;
}

std::shared_ptr<Row> Row::deserialize(void *source) {
    uint8_t *src = (uint8_t*)source;
    std::shared_ptr<Row> r = std::make_shared<Row>();
    std::memcpy(&(r->id), src + ID_OFFSET, ID_SIZE);
    std::memcpy((r->username), src + USERNAME_OFFSET, USERNAME_SIZE);
    std::memcpy((r->email), src + EMAIL_OFFSET, EMAIL_SIZE);
    return r;
}

std::ostream &operator<<(std::ostream &strm, const Row &r) {
    strm << "Row(" << r.id << ", " << r.username << ", " << r.email << ")";
    return strm;
}

Table::Table() {
    this->num_rows = 0;
    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        this->pages[i] = NULL;
    }
}

void* Table::row_slot(uint32_t row_num)  {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void* page = pages[page_num];
    if(page == NULL) {
        page = pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return ((uint8_t*)page) + byte_offset;
}