#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <memory>

// Row properties
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

class Row {
    public:
        uint32_t id;
        char username[COLUMN_USERNAME_SIZE];
        char email[COLUMN_EMAIL_SIZE];

        Row(){};
        // ~Row() {
        //     std::cout << "Deref Row" << std::endl;
        // }

        void* serialize(void *destination);
        static std::shared_ptr<Row> deserialize(void *source);
        friend std::ostream &operator<<(std::ostream &strm, const Row &r);
};

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

// Table properties
const uint32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100
const uint32_t ROWS_PER_PAGE = PAGE_SIZE/ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE; 

class Table {

    public:
        uint32_t num_rows;
        void* pages[TABLE_MAX_PAGES];
        Table();
        void* row_slot(uint32_t row_num);
};


#endif