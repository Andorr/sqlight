#include <cstring>
#include <memory>

#include "table.h"
#include "util.h"

void* Row::serialize(void *destination) {
    uint8_t *dest = (uint8_t*)destination;
    std::memcpy(dest + ID_OFFSET, &id, ID_SIZE);


    util::serialize_string(username, dest + USERNAME_OFFSET);
    util::serialize_string(email, dest + EMAIL_OFFSET);
    return dest;
}

std::shared_ptr<Row> Row::deserialize(void *source) {
    uint8_t *src = (uint8_t*)source;
    std::shared_ptr<Row> r = std::make_shared<Row>();
    
    std::memcpy(&(r->id), src + ID_OFFSET, ID_SIZE);

    r->username = util::deserialize_string(r->username, src + USERNAME_OFFSET);
    r->email = util::deserialize_string(r->email, src + EMAIL_OFFSET);

    return r;
}

std::ostream &operator<<(std::ostream &strm, const Row &r) {
    strm << "Row(" << r.id << ", " << r.username << ", " << r.email << ")";
    return strm;
}

Table::Table(std::string filename) {
    Pager *pager = new Pager(filename);
    this->num_rows = pager->file_length / ROW_SIZE;
    this->pager = pager;
}

Table::~Table() {
    close();
}

void* Table::row_slot(uint32_t row_num)  {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = this->pager->get_page(page_num);
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return ((uint8_t*)page) + byte_offset;
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

Pager::Pager(std::string filename) {
    // Make sure file exists
    this->file.open(filename,
        std::ios_base::out | std::ios::binary | std::fstream::app
    );
    this->file.close();

    this->file.open(filename,
        std::ios_base::out | std::ios::in | std::ios::binary
    );

    this->file.seekg (0, this->file.end);
    auto file_size = this->file.tellg();

    this->file_length = file_size;
    
    
    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        this->pages[i] = NULL;
    }
}


Pager::~Pager() {
    close();
}

void* Pager::get_page(uint32_t page_num) {
    if(page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d\n", TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if(pages[page_num] == NULL) {
        // Cache miss. Allocate memory and load from file
        void *page = malloc(PAGE_SIZE);
        uint32_t num_pages = file_length / PAGE_SIZE; // TODO: Fix this! Do not use file_length to check this.
        
        // We might save a partial page at the end of the file
        if(file_length % PAGE_SIZE) {
            num_pages++;
        }

        if(page_num <= num_pages && num_pages != 0) {
            file.seekg(page_num*PAGE_SIZE, std::ios::beg);
            file.readsome((char*)page, PAGE_SIZE);
            if(errno > 0) {
                printf("Error reading file: %d - %s\n", errno, std::strerror(errno));
                exit(EXIT_FAILURE);
            }
            file.clear();
        }

        pages[page_num] = page;
    }

    return pages[page_num];
}

void Pager::flush(uint32_t page_num, uint32_t size) {
    if(pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    file.seekp(page_num * PAGE_SIZE, std::ios::beg);
    
    if(errno > 0) {
        printf("Error seeking: %d - %s\n", errno, std::strerror(errno));
        exit(EXIT_FAILURE);
    }
    file.write((char*)pages[page_num], size);
    if(errno > 0) {
        printf("Error writing: %d - %s\n", errno, std::strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void Pager::close() {
    file.close();
    if(errno != 0) {
        printf("Error closing db file\n");
        exit(EXIT_FAILURE);
    }

    for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = pages[i];
        if(page) {
            free(page);
            pages[i] = NULL;
        }    
    }
}