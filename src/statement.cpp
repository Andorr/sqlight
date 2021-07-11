#include "row.h"
#include "statement.h"

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
    if(table.num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    row_to_insert.serialize(table.row_slot(table.num_rows));
    table.num_rows++;
    std::cout << "Executed." << std::endl;
    return EXECUTE_SUCCESS;
}

ExecutionResult Statement::execute_select(Table &table) {
    for(uint32_t i = 0; i < table.num_rows; i++) {
        std::shared_ptr<Row> row = Row::deserialize(table.row_slot(i));
        std::cout << *row << std::endl;
    }
    return EXECUTE_SUCCESS;
}

std::ostream& operator<<(std::ostream &strm, const Statement &s) {
    return strm << s.type;
}