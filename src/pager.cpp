#include <cstring>
#include <memory>


#include "pager.h"
#include "storage.h"
#include "node.h"

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
    this->num_pages = file_size / PAGE_SIZE;
    
    if(file_size % PAGE_SIZE != 0) {
        printf("Db file is not a whole number of pages. Corrupt file.\n");
        exit(EXIT_FAILURE);
    }

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

        if(page_num <= num_pages /* && num_pages != 0 */) {
            file.seekg(page_num*PAGE_SIZE, std::ios::beg);
            file.readsome((char*)page, PAGE_SIZE);
            if(errno > 0) {
                printf("Error reading file: %d - %s\n", errno, std::strerror(errno));
                exit(EXIT_FAILURE);
            }
            file.clear();
        }

        pages[page_num] = page;

        if(page_num >= num_pages) {
            num_pages = page_num + 1;      
        }
    }
    return pages[page_num];
}

/*
Until we start recycling free pages, new pages will always
go onto the end of the database file.
*/
uint32_t Pager::get_unused_page_num() {
    return num_pages;
}

void Pager::flush(uint32_t page_num) {
    if(pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    file.seekp(page_num * PAGE_SIZE, std::ios::beg);
    if(errno > 0) {
        printf("Error seeking: %d - %s\n", errno, std::strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    file.write((char*)pages[page_num], PAGE_SIZE);
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

void Pager::print_tree(uint32_t page_num, uint32_t indentation_level) {
    void *node = get_page(page_num);
    uint32_t num_keys, child;

    auto indent = [](uint32_t level) {
        for(uint32_t i = 0; i < level; i++) {
            printf(" ");
        }   
    };

    switch(get_node_type(node)) {
        case (NODE_LEAF): {
            num_keys = *leaf_node_num_cells(node);
            indent(indentation_level);
            printf("- leaf (size %d)\n", num_keys);
            for (uint32_t i = 0; i < num_keys; i++) {
                indent(indentation_level + 1);
                printf("- %d\n", *leaf_node_key(node, i));
            }

            break;
        }
        case (NODE_INTERNAL): {
            num_keys = *internal_node_num_keys(node);
            indent(indentation_level);
            printf("- internal (size %d)\n", num_keys);
            for (uint32_t i = 0; i < num_keys; i++) {
                child = *internal_node_child(node, i);
                print_tree(child, indentation_level + 1);

                indent(indentation_level + 1);
                printf("- key %d\n", *internal_node_key(node, i));
            }
            child = *internal_node_right_child(node);
            print_tree(child, indentation_level + 1);
            break;
        }
    }
}