#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef MIN_HEAP
#define MIN_HEAP

struct Heap{
    struct HuffmanTree** arr;
    int count;
    int capacity;
};

struct HuffmanTree* buildTree(uint16_t nb_chars, struct HuffmanTree* nodes[]);
#endif