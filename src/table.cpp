#include <iostream>

#include "storage.h"
#include "table.h"
#include "util.h"
#include "pager.h"
#include "node.h"
#include "cursor.h"

Table::Table(std::string filename) {
    Pager *pager = new Pager(filename);
    this->pager = pager;
    this->root_page_num = 0;
    
    if(this->pager->num_pages == 0) {
        // New database file. Initialize page 0 as leaf node.
        void *root_node = this->pager->get_page(0);
        initialize_leaf_node(root_node);
    }
}

Table::~Table() {
    close();
}

void Table::close() {
    // Flush memory
    for(uint32_t i = 0; i < pager->num_pages; i++) {
        if(pager->pages[i] == NULL) {
            continue;
        }
        pager->flush(i);
        pager->pages[i] = NULL;
    }

    delete pager;
}

Cursor* Table::find(uint32_t key) {
    void *root_node = pager->get_page(root_page_num);

    if(get_node_type(root_node) == NODE_LEAF) {
        return leaf_node_find(root_page_num, key);
    } else {
        printf("TODO: Need to implement searching an internal node\n");
        exit(EXIT_FAILURE);
    }
}

Cursor* Table::leaf_node_find(uint32_t page_num, uint32_t key) {
    void *node = pager->get_page(page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    Cursor* cursor = new Cursor(*this, page_num, 0, false);

    uint32_t min_index = 0;
    uint32_t one_past_max_index = num_cells;
    while (one_past_max_index != min_index) {
        uint32_t index = (min_index + one_past_max_index) / 2;
        uint32_t key_at_index = *leaf_node_key(node, index);
        if(key == key_at_index) {
            cursor->cell_num = index;
            return cursor;
        }
        if(key < key_at_index) {
            one_past_max_index = index;
        } else {
            min_index = index + 1;
        }
    }

    cursor->cell_num = min_index;
    return cursor;
}