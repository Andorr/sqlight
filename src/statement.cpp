#include "row.h"
#include "statement.h"
#include "node.h"
#include "cursor.h"

ExecutionResult Statement::execute(Table &table) {
    switch (this->type)
    {
    case STATEMENT_INSERT:
        return execute_insert(table);
    case STATEMENT_SELECT:
        return execute_select(table);
    default:
        return EXECUTE_NONE;
    }

}

ExecutionResult Statement::execute_insert(Table &table) {
    void *node = table.pager->get_page(table.root_page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    if(num_cells >= LEAF_NODE_MAX_CELLS) {
        return EXECUTE_TABLE_FULL;
    }

    uint32_t key_to_insert = row_to_insert.id;
    Cursor* cursor = table.find(key_to_insert);

    if(cursor->cell_num < num_cells) {
        uint32_t key_at_index = *leaf_node_key(node, cursor->cell_num);
        if(key_at_index == key_to_insert) {
            return EXECUTE_DUPLICATE_KEY;
        }
    }

    cursor->insert(row_to_insert.id, &row_to_insert);
    std::cout << "Executed." << std::endl;
    return EXECUTE_SUCCESS;
}

ExecutionResult Statement::execute_select(Table &table) {
    Cursor cursor = Cursor::from_start(table);

    while(!cursor.end_of_table) {
        std::shared_ptr<Row> row = Row::deserialize(cursor.value());
        std::cout << *row << std::endl;
        cursor.advance();
    }

    return EXECUTE_SUCCESS;
}

std::ostream& operator<<(std::ostream &strm, const Statement &s) {
    return strm << s.type;
}