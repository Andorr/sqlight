#include <iostream>
#include <fstream>
#include <tuple>
#include "json.hpp"

#include "input.h"
#include "statement.h"

using json = nlohmann::json;

json read_json(std::string filename);

class Scanner {

	public:
		Scanner();

};

int main() {
	InputBuffer input = InputBuffer();

	while(true) {
		input.read();

		// Parse and execute meta command
		MetaCommandResult res = input.parse_and_do_meta_command();
		if(res == META_COMMAND_UNRECOGNIZED_COMMAND) {
			continue;
		}
		
		// Input is not a meta command, exec statement
		StatementType statement_type;
		PrepareResult result;
		std::tie(statement_type, result) = input.parse_statement_type();
		switch(result) {
			case PREPARE_SUCCESS:
				break;
			case PREPARE_UNRECOGNIZED_STATEMENT:
				printf("Unrecognized keyword.\n");
		}

		Statement s(statement_type);
		s.execute();
	}

	return 0;
}

json read_json(std::string filename) {
	std::fstream data(filename);
	json j;
	data >> j;
	return j;
}