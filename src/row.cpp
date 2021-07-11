#include <memory>
#include <cstring>

#include "row.h"
#include "util.h"

void* Row::serialize(void *destination) {
    uint8_t *dest = (uint8_t*)destination;
    std::memcpy(dest + ID_OFFSET, &id, ID_SIZE);


    util::serialize_string(username, dest + USERNAME_OFFSET);
    util::serialize_string(email, dest + EMAIL_OFFSET);
    return dest;
}

std::shared_ptr<Row> Row::deserialize(void *source) {
    uint8_t *src = (uint8_t*)source;
    std::shared_ptr<Row> r = std::make_shared<Row>();
    
    std::memcpy(&(r->id), src + ID_OFFSET, ID_SIZE);

    r->username = util::deserialize_string(r->username, src + USERNAME_OFFSET);
    r->email = util::deserialize_string(r->email, src + EMAIL_OFFSET);

    return r;
}

std::ostream &operator<<(std::ostream &strm, const Row &r) {
    strm << "Row(" << r.id << ", " << r.username << ", " << r.email << ")";
    return strm;
}
