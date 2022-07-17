#include "bitReaderAndWriter.h"
#include "huffmanGeneral.h"

#ifndef HUFFMAN_DECODE_H
#define HUFFMAN_DECODE_H

struct node {
    char value;

    struct node* left;
    struct node* right;
};

void build_tree(struct node* root, struct bitReader* reader, unsigned tree_length);
struct fileCopy* huffman_decode(size_t len, const char data[len]);
#endif