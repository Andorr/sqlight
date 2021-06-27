#include "statement.h"

void Statement::execute() {
    switch (this->type)
    {
    case STATEMENT_INSERT:
        std::cout << "INSERT STATEMENT" << std::endl;
        break;
    case STATEMENT_SELECT:
        std::cout << "SELECT STATEMENT" << std::endl;
        break;
    default:
        break;
    }

}