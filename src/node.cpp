#include <iostream>
#include <cstring>

#include "storage.h"
#include "node.h"


/*
    COMMON NODE
*/
NodeType get_node_type(void *node) {
    uint32_t value = *(((uint8_t*)(node) + NODE_TYPE_OFFSET));
    return (NodeType)value;
}

void set_node_type(void *node, NodeType type) { 
    uint8_t value = type;
    *(((uint8_t*)(node) + NODE_TYPE_OFFSET)) = value;
}


void print_constants() {
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}


bool is_node_root(void* node) {
    uint8_t value = *((uint8_t*)(node) + IS_ROOT_OFFSET);
    return (bool)value;
}
void set_node_root(void* node, bool is_root) {
    uint8_t value = is_root;
    *((uint8_t*)(node) + IS_ROOT_OFFSET) = value;
}

uint32_t get_node_max_key(void* node) {
    switch(get_node_type(node)) {
        case NODE_INTERNAL:
            return *internal_node_key(node, *internal_node_num_keys(node) - 1);
        case NODE_LEAF:
            return *leaf_node_key(node, *leaf_node_num_cells(node) - 1);
    }
    return -1;
}

/*
    LEAF _NODE
*/

// Leaf Node Access Field
uint32_t* leaf_node_num_cells(void *node) {
    return (uint32_t*)((uint8_t*)(node) + LEAF_NODE_NUM_CELLS_OFFSET);
}

void* leaf_node_cell(void *node, uint32_t cell_num) {
    return (uint8_t*)(node) + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t* leaf_node_key(void  *node, uint32_t cell_num) {
    return (uint32_t*)leaf_node_cell(node, cell_num);
}

void* leaf_node_value(void  *node, uint32_t cell_num) {
    return (uint8_t*)leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

void initialize_leaf_node(void* node) {
    set_node_type(node, NODE_LEAF);
    *leaf_node_num_cells(node) = 0;
    set_node_root(node, false);
}

/*
    INTERNAL NODE
*/
uint32_t* internal_node_num_keys(void* node) {
    return (uint32_t*)((uint8_t*)(node) + INTERNAL_NODE_NUM_KEYS_OFFSET);
}

uint32_t* internal_node_right_child(void* node) {
    return (uint32_t*)((uint8_t*)(node) + INTERNAL_NODE_RIGHT_CHILD_OFFSET);
}

uint32_t* internal_node_cell(void* node, uint32_t cell_num) {
    return (uint32_t*)((uint8_t*)(node) + INTERNAL_NODE_HEADER_SIZE + cell_num*INTERNAL_NODE_CELL_SIZE);
}

uint32_t* internal_node_child(void* node, uint32_t child_num) {
    uint32_t num_keys = *internal_node_num_keys(node);
    if(child_num > num_keys) {
        printf("Tried to access child_num %d > num_keys %d\n", child_num, num_keys);
        exit(EXIT_FAILURE);
    } else if(child_num == num_keys) {
        return internal_node_right_child(node);
    } else {
        return internal_node_cell(node, child_num);
    }
}

uint32_t* internal_node_key(void* node,  uint32_t key_num) {
    return internal_node_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

void initialize_internal_node(void* node) {
    set_node_type(node, NODE_INTERNAL);
    set_node_root(node, false);
    *internal_node_num_keys(node) = 0;
}