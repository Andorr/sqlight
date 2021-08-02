#ifndef ROW_H
#define ROW_H

#include <memory>

#include "storage.h"

class Row {


    public:
        uint32_t id;
        std::string username = "";
        std::string email = "";

        Row(){};
  
        void* serialize(void *destination);
        static std::shared_ptr<Row> deserialize(void *source);
        friend std::ostream &operator<<(std::ostream &strm, const Row &r);
};

#endif