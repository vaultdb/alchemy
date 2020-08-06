//
// Created by Jennie Rogers on 8/5/20.
//

#ifndef DATA_UTILITIES_H
#define DATA_UTILITIES_H


#include <cstdint>

class DataUtilities {

    // convert 8 bits to a byte
    static int8_t boolsToByte(bool *src);

    // reverse the order of bits in a byte
    static unsigned char reverse(unsigned char b);


public:
    static bool *bytesToBool(int8_t* bytes, int byteCount);
    static int8_t *boolsToBytes(bool *src, int bitCount);
};


#endif // DATA_UTILITIES_H
