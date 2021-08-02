#include <iostream>
#include <cstring>

#include "storage.h"
#include "cursor.h"
#include "node.h"
#include "row.h"

void* Cursor::value() {
    void *page = this->table.pager->get_page(page_num);
    return leaf_node_value(page, cell_num);
}

void Cursor::advance() {
    void *node = table.pager->get_page(page_num);

    cell_num += 1;
    if(cell_num >= (*leaf_node_num_cells(node))) {
        // Advance to next leaf n

        uint32_t next_page_num = *leaf_node_next_leaf(node);
        if(next_page_num == 0) {
            // We are the rightmost leaf.
            end_of_table = true;
        } else {
            page_num = next_page_num;
            cell_num = 0;
        }
    }
}

void Cursor::insert(uint32_t key, Row* value) {
    void *node = table.pager->get_page(page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    if(num_cells >= LEAF_NODE_MAX_CELLS) {
        // Node is full
        split_and_insert(key, value);
        return;
    }

    if(cell_num < num_cells) {
        // We want to insert it where an existing cell lives. Make a room for a new cell.
        for(int32_t i = num_cells; i > cell_num; i--) {
            void *dst = leaf_node_cell(node, i);
            void *src = leaf_node_cell(node, i - 1);
            std::memcpy(dst, src, LEAF_NODE_CELL_SIZE);   
        }
    }

    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cell_num)) = key;
    value->serialize(leaf_node_value(node, cell_num));
}

void Cursor::split_and_insert(uint32_t key, Row* row) {
    /**
     * Create a new node and move half the cells over.
     * Insert the new value in one of the two nodes.
     * Update parent or create a new parent.
     *
     */

    void* old_node = table.pager->get_page(page_num);
    uint32_t old_max = get_node_max_key(old_node);
    uint32_t new_page_num = table.pager->get_unused_page_num();
    void* new_node = table.pager->get_page(new_page_num);
    initialize_leaf_node(new_node);
    *node_parent(new_node) = *node_parent(old_node);
    *leaf_node_next_leaf(new_node) = *leaf_node_next_leaf(old_node);
    *leaf_node_next_leaf(old_node) = new_page_num;

    /**
     * All existing keys plus new key should be divided 
     * evenly between old (left) and new (right) nodes.
     * Starting from the right, move each key to correct position.
     */
    for(int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--) {
        void* destination_node;
        
        if(i >= LEAF_NODE_LEFT_SPLIT_COUNT) {
            destination_node = new_node;
        } else {
            destination_node = old_node;
        }

        uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        void* destination = leaf_node_cell(destination_node, index_within_node);

        if(i == cell_num) {
            row->serialize(leaf_node_value(destination_node, index_within_node));
            *leaf_node_key(destination_node, index_within_node) = key;
        } else if(i > cell_num) {
            std::memcpy(destination, leaf_node_cell(old_node, i - 1), LEAF_NODE_CELL_SIZE);
        } else {
            std::memcpy(destination, leaf_node_cell(old_node, i), LEAF_NODE_CELL_SIZE);
        }
    }

    // Update cell count on both leaf nodes
    *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

    if(is_node_root(old_node)) {
        return table.create_new_root(new_page_num);
    } else {
        uint32_t parent_page_num = *node_parent(old_node);
        uint32_t new_max = get_node_max_key(old_node);
        void *parent = table.pager->get_page(parent_page_num);

        update_internal_node_key(parent, old_max, new_max);
        table.internal_node_insert(parent_page_num, new_page_num);
    }
}

Cursor Cursor::from_start(Table &table) {
    Cursor *cursor = table.find(0);

    void *node = table.pager->get_page(cursor->page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
    cursor->end_of_table = (num_cells == 0);

    return *cursor;
}

