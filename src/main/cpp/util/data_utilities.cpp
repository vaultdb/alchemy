//
// Created by Jennie Rogers on 8/5/20.
//

#include <cstddef>
#include <assert.h>
#include "data_utilities.h"

unsigned char DataUtilities::reverse(unsigned char b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}


signed char DataUtilities::boolsToByte(bool *src) {
    signed char dst = 0;

    for(int i = 0; i < 8; ++i) {
        dst |= (src[i] << i);
    }

    return dst;
}

bool *DataUtilities::bytesToBool(int8_t *bytes, int byteCount) {
    bool *ret = new bool[byteCount * 8];


    bool *writePos = ret;

    for(int i = 0; i < byteCount; ++i) {
        uint8_t b = bytes[i];
        for(int j = 0; j < 8; ++j) {
            *writePos = ((b & (1<<j)) != 0);
            ++writePos;
        }
    }
    return ret;
}

int8_t *DataUtilities::boolsToBytes(bool *src, int bitCount) {
    int byteCount = bitCount / 8;
    assert(bitCount % 8 == 0); // no partial bytes supported

    int8_t *result = new int8_t[byteCount];

    bool *cursor = src;

    for(int i = 0; i < byteCount; ++i) {
        result[i] = boolsToByte(cursor);
        cursor += 8;
    }

    return result;

}

