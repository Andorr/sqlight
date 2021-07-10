#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <memory>

#include "input.h"
#include "util.h"

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
        return prepare_insert();
    }
    else if(this->buffer.rfind("select", 0) == 0) {
        auto s = std::make_shared<Statement>(STATEMENT_SELECT);
        return std::make_tuple(s, PREPARE_SUCCESS);
    }
       
    return std::make_tuple(nullptr, PREPARE_UNRECOGNIZED_STATEMENT);
}

std::tuple<std::shared_ptr<Statement>, PrepareResult> InputBuffer::prepare_insert() {    
    std::vector<std::string> tokens = util::string_split(buffer);   
    if(tokens.size() != 4) {
        return std::make_tuple(nullptr, PREPARE_SYNTAX_ERROR);
    }
    
    Row row;
    try {
        row.id = std::stoi(tokens[1]);
    } catch(std::exception &err) {
        std::cout << "Error parsing int" << std::endl;
    }

    row.username = tokens[2]; 
    if(row.username.size() > COLUMN_USERNAME_SIZE) {
        return std::make_tuple(nullptr, PREPARE_STRING_TOO_LONG);
    }

    row.email = tokens[3]; 
    if(row.email.size() > COLUMN_EMAIL_SIZE) {
        return std::make_tuple(nullptr, PREPARE_STRING_TOO_LONG);
    }

    auto s = std::make_shared<Statement>(STATEMENT_INSERT, row);
    return std::make_tuple(s, PREPARE_SUCCESS);
}