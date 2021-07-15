#include "row.h"
#include "statement.h"
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
    if(table.num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }
    Cursor cursor_end = Cursor::from_end(table);


    row_to_insert.serialize(cursor_end.value());
    table.num_rows++;
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