#include <cstring>
#include <memory>


#include "pager.h"
#include "storage.h"

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