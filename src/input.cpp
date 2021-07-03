#include <iostream>
#include <string>
#include <tuple>
#include <memory>
#include "input.h"


void InputBuffer::read() {
    std::cout << this->prompt;
    std::getline(std::cin, this->buffer);

    if(this->buffer.empty()) {
        std::cout << "Error reading input";
        exit(EXIT_SUCCESS);
    }
}


MetaCommandResult InputBuffer::parse_meta_command() {
    if(this->buffer.rfind(".", 0) != 0) {
        return META_COMMAND_NONE;
    }

    if(this->buffer.rfind(".exit", 0) == 0) {
        return META_COMMAND_EXIT;
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

MetaCommandResult InputBuffer::parse_and_do_meta_command() {
    MetaCommandResult res = this->parse_meta_command();
    switch (res)
    {
    case META_COMMAND_EXIT:
        std::cout << "Exiting..." << std::endl;
        exit(EXIT_SUCCESS);
    case META_COMMAND_UNRECOGNIZED_COMMAND:
        std::cout << "Unrecognized command: '" << this->buffer << "'." << std::endl;
        break;
    default:
        break;
    }

    return res;
}

std::tuple<std::shared_ptr<Statement>, PrepareResult> InputBuffer::parse_statement_type() {
    
    if(this->buffer.rfind("insert", 0) == 0) {
        Row row;
        int args_assigned = std::sscanf(buffer.c_str(), "insert %d %s %s", &row.id, row.username, row.email);
        if(args_assigned < 3) {
            return std::make_tuple(nullptr, PREPARE_SYNTAX_ERROR);
        }
        auto s = std::make_shared<Statement>(STATEMENT_INSERT, row);
        return std::make_tuple(s, PREPARE_SUCCESS);
    }
    else if(this->buffer.rfind("select", 0) == 0) {
        auto s = std::make_shared<Statement>(STATEMENT_SELECT);
        return std::make_tuple(s, PREPARE_SUCCESS);
    }
       
    return std::make_tuple(nullptr, PREPARE_UNRECOGNIZED_STATEMENT);
}
