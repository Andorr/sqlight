#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

typedef enum {
    PREPARE_SUCCESS, 
    PREPARE_UNRECOGNIZED_STATEMENT 
} PrepareResult;

typedef enum {
    STATEMENT_NONE,
    STATEMENT_INSERT,
    STATEMENT_SELECT,
} StatementType;

class Statement {
    private:
        StatementType type;

    public:
        Statement(StatementType type): type(type) {};
        void execute();
};

#endif