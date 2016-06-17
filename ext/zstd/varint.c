#include "varint.h"

size_t varint_encoded_size(uint64_t value)
{
    uint64_t remaining = value;
    size_t size = 1;
    remaining = (remaining >> 7);
    while (remaining) {
        size += 1;
        remaining = (remaining >> 7);
    }
    return size;
}

size_t varint_encode(uint64_t value, char* buffer)
{
    uint64_t remaining = value;
    size_t pos = 0;
    while (1) {
        uint8_t part = (uint8_t)(remaining & 0x7F);
        remaining = (remaining >> 7);

        if (remaining) {
            part |= 0x80;
            buffer[pos] = (char)part;
            pos += 1;
        } else {
            *buffer = (char)part;
            return pos + 1;
        }
    }
}

size_t varint_decode(uint64_t* outputValue, const char* buffer)
{
    uint64_t value = 0;
    size_t pos = 0;
    while (1) {
        uint8_t part = (uint8_t)buffer[pos];

        if (part & 0x80) {
            value = (value << 7) | (part & 0x7F);
            pos += 1;
        } else {
            value |= part;
            *outputValue = value;
            return pos + 1;
        }
    }
}


