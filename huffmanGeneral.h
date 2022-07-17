#include <stddef.h>

#ifndef HUFFMANHEADER_H
#define HUFFMANHEADER_H
struct fileCopy {
    size_t len;
    unsigned char* data;
};

struct HuffmanTree {
    int value;
    char character;
    struct HuffmanTree* left;
    struct HuffmanTree* right;
};
#endif