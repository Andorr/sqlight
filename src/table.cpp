#include <iostream>
#include <cstring>

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
        set_node_root(root_node, true);
    }
}

Table::~Table() {
    close();
}

void Table::close() {
    // Flush memory
    for(uint32_t i = 0; i <= pager->num_pages; i++) {
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

void Table::create_new_root(uint32_t right_child_page_num) {
    /*
        Handle spliutting the root.
        Old root copied to new page, becomes left child.
        Address of right child passed in.
        Re-initialize root page to contain the new root node.
        New root node points to two children
    */

    void *root = pager->get_page(root_page_num);
    void *right_child = pager->get_page(right_child_page_num);
    uint32_t left_child_page_num = pager->get_unused_page_num();
    void* left_child = pager->get_page(left_child_page_num);

    // Left child has data coped from old root
    std::memcpy(left_child, root, PAGE_SIZE);
    set_node_root(left_child, false);

    // Root node is a new internal node with one key and two children.
    initialize_internal_node(root);
    set_node_root(root, true);
    *internal_node_num_keys(root) = 1;
    *internal_node_child(root, 0) = left_child_page_num;
    uint32_t left_child_max_key = get_node_max_key(left_child);
    *internal_node_key(root, 0) = left_child_max_key;
    *internal_node_right_child(root) = right_child_page_num;
}

