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
        this->end_of_table = true;
    }
}

void Cursor::insert(uint32_t key, Row* value) {
    void *node = table.pager->get_page(page_num);

    int32_t num_cells = *leaf_node_num_cells(node);
    if(num_cells >= LEAF_NODE_MAX_CELLS) {
        // Node is full
        printf("TODO: Need to implement splitting a leaf node.\n");
        exit(EXIT_FAILURE);
    }

    if(cell_num < num_cells) {
        // We want to insert it where an existing cell lives. Make a room for a new cell.
        for(uint32_t i = num_cells; i > cell_num; i--) {
            std::memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);   
        }
    }

    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cell_num)) = key;
    value->serialize(leaf_node_value(node, cell_num));   
}

Cursor Cursor::from_start(Table &table) {
    void *root_node = table.pager->get_page(table.root_page_num);
    uint32_t num_cells = *leaf_node_num_cells(root_node);
    return Cursor(table, table.root_page_num, 0, num_cells == 0);
}