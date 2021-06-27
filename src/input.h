#ifndef INPUT_H
#define INPUT_H
#include <iostream>
#include "statement.h"

typedef enum {
    META_COMMAND_NONE,
    META_COMMAND_EXIT,
    META_COMMAND_UNRECOGNIZED_COMMAND,
} MetaCommandResult;

class InputBuffer {

    private:
        std::string buffer;

    public:
        std::string prompt = "db > ";

        InputBuffer() {
            buffer = "";
        }

    void read();
    MetaCommandResult parse_meta_command();
    MetaCommandResult parse_and_do_meta_command();
    std::tuple<StatementType, PrepareResult> parse_statement_type();

};

#endif