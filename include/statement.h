#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

#include "row.h"
#include "table.h"


typedef enum {
    PREPARE_SUCCESS, 
    PREPARE_UNRECOGNIZED_STATEMENT, 
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG,
} PrepareResult;

typedef enum {
    STATEMENT_NONE,
    STATEMENT_INSERT,
    STATEMENT_SELECT,
} StatementType;

typedef enum {
    EXECUTE_NONE,
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY,
    EXECUTE_TABLE_FULL,
} ExecutionResult;

class Statement {
    private:
        StatementType type;

    public:
        Row row_to_insert;
        Statement(){};
        Statement(StatementType type): type(type) {};
        Statement(StatementType type, Row row): type(type), row_to_insert(row) {};
        // ~Statement() {
        //     std::cout << "Deref Statement" << std::endl;
        // }
        ExecutionResult execute_insert(Table &table);
        ExecutionResult execute_select(Table &table);
        ExecutionResult execute(Table &table);
        friend std::ostream& operator<<(std::ostream& os, const Statement &s);
};



#endif