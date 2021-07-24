#include <iostream>

#include "sqlight.h"

int main(int argc, char** argv) {
	std::string filename;
	if(argc < 2) {
		filename = "sqlight.db";
	} else {
		filename = std::string(argv[1]);
	}

	SQLight::open(std::string(filename));

	return 0;
}