#include <iostream>
#include <fstream>
#include <tuple>
// #include "json.hpp"

#include "input.h"
#include "statement.h"
#include "table.h"

// using json = nlohmann::json;

// json read_json(std::string filename);

class Scanner {

	public:
		Scanner();

};

int main() {
	Table table = Table();
	InputBuffer input = InputBuffer();


	while(true) {
		input.read();

		// Parse and execute meta command
		MetaCommandResult res = input.parse_and_do_meta_command();
		if(res == META_COMMAND_UNRECOGNIZED_COMMAND) {
			continue;
		}
		
		// Input is not a meta command, exec statement
		auto result = input.parse_statement_type();
		
		switch(std::get<PrepareResult>(result)) {
			case PREPARE_SUCCESS:
				break;
			case PREPARE_UNRECOGNIZED_STATEMENT:
				printf("Unrecognized keyword.\n");
				continue;
			case PREPARE_SYNTAX_ERROR: {
				printf("Syntax error: %s\n", input.buffer.c_str());
				continue;
			}
		}

		auto statement = std::get<std::shared_ptr<Statement>>(result);
		ExecutionResult exec_result = statement->execute(table);
		switch (exec_result)
		{
		case EXECUTE_SUCCESS:
			break;
		case EXECUTE_TABLE_FULL:
			std::cout << "Error: table full" << std::endl;
			break;
		case EXECUTE_NONE:
			std::cout << "Invalid statement type" << std::endl;
			break;
		}
	}

	return 0;
}

// json read_json(std::string filename) {
// 	std::fstream data(filename);
// 	json j;
// 	data >> j;
// 	return j;
// }