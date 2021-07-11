#ifndef SQLIGHT_H
#define SQLIGHT_H

#include <string>

class SQLight {

    public:
        static SQLight open(std::string filename);

	private:
		SQLight(std::string filename);

};

#endif