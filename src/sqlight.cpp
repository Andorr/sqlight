#include <iostream>
#include <fstream>
#include <string>
#include <tuple>

#include "input.h"
#include "statement.h"
#include "table.h"
#include "node.h"
#include "sqlight.h"



SQLight SQLight::open(std::string filename) {
    return SQLight(filename);
}

SQLight::SQLight(std::string filename) {
	Table *table = new Table(filename);
	InputBuffer input = InputBuffer();

	while(true) {
		input.read();

		// Parse and execute meta command
		MetaCommandResult res = input.parse_meta_command();
        switch (res)
        {
            case META_COMMAND_EXIT:
                delete table;
                std::cout << "Exiting..." << std::endl;
                exit(EXIT_SUCCESS);
			case META_COMMAND_CONSTANTS:
				printf("Constants:\n");
				print_constants();
				continue;
			case META_COMMAND_BTREE:
				printf("Tree:\n");
				table->pager->print_tree(0, 0);
				continue;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                std::cout << "Unrecognized command: '" << input.buffer << "'." << std::endl;
                continue;
            default:
                break;
        }

		// Input is not a meta command, exec statement
		auto result = input.parse_statement_type();
		
		switch(std::get<PrepareResult>(result)) {
			case PREPARE_SUCCESS:
				break;
			case PREPARE_STRING_TOO_LONG:
				printf("String is too long.\n");
				continue;
			case PREPARE_UNRECOGNIZED_STATEMENT:
				printf("Unrecognized keyword.\n");
				continue;
			case PREPARE_SYNTAX_ERROR: {
				printf("Syntax error: %s\n", input.buffer.c_str());
				continue;
			}
		}

		auto statement = std::get<std::shared_ptr<Statement>>(result);
		ExecutionResult exec_result = statement->execute(*table);
		switch (exec_result)
		{
		case EXECUTE_SUCCESS:
			break;
		case EXECUTE_TABLE_FULL:
			std::cout << "Error: table full";
			std::cout << std::endl;
			break;
		case EXECUTE_DUPLICATE_KEY:
			std::cout << "Error: Duplicate key." << std::endl;
			break;
		case EXECUTE_NONE:
			std::cout << "Invalid statement type";
			std::cout << std::endl;
			break;
		}
	}
}