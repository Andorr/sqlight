#ifndef INPUT_H
#define INPUT_H
#include <iostream>
#include "statement.h"

typedef enum {
    META_COMMAND_NONE,
    META_COMMAND_CONSTANTS,
    META_COMMAND_BTREE,
    META_COMMAND_EXIT,
    META_COMMAND_UNRECOGNIZED_COMMAND,
} MetaCommandResult;

class InputBuffer {

    public:
        std::string buffer;
        std::string prompt = "db > ";

        InputBuffer() {
            buffer = "";
        }

    void read();
    MetaCommandResult parse_meta_command();
    std::tuple<Statement*, PrepareResult> parse_statement_type();
    std::tuple<Statement*, PrepareResult> prepare_insert();
};

#endif