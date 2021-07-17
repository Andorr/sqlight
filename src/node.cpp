#include <iostream>

#include "storage.h"
#include "node.h"

// Leaf Node Access Field
uint32_t* leaf_node_num_cells(void *node) {
    return (uint32_t*)(node) + LEAF_NODE_NUM_CELLS_OFFSET;
}

uint32_t* leaf_node_cell(void *node, uint32_t cell_num) {
    return (uint32_t*)(node) + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t* leaf_node_key(void  *node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num);
}

uint32_t* leaf_node_value(void  *node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

void initialize_leaf_node(void* node) {
    set_node_type(node, NODE_LEAF);
    *leaf_node_num_cells(node) = 0;
}

NodeType get_node_type(void *node) {
    uint32_t value = *(((uint32_t*)(node) + NODE_TYPE_OFFSET));
    return (NodeType)value;
}

void set_node_type(void *node, NodeType type) { 
    uint8_t value = type;
    *(((uint32_t*)(node) + NODE_TYPE_OFFSET)) = value;
}


void print_constants() {
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

void print_leaf_node(void *node) {
    uint32_t num_cells = *leaf_node_num_cells(node);
    printf("leaf (size %d)\n", num_cells);
    for(uint32_t i = 0; i < num_cells; i++) {
        uint32_t key = *leaf_node_key(node, i);
        printf("  - %d : %d\n", i, key);
    }
}