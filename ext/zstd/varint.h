#include <stdint.h>
#include <stddef.h>

size_t varint_encoded_size(uint64_t value);

size_t varint_encode(uint64_t value, char* buffer);

size_t varint_decode(uint64_t* value, const char* buffer);
