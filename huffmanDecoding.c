#include <stdio.h>
#include <stdlib.h>

#include "huffmanDecoding.h"

void free_tree(struct node* root) {
    if(root->left != NULL) {
        free_tree(root->left);
        free_tree(root->right);
    }

    free(root);
}

void build_tree(struct node* root, struct bitReader* reader) {
    char current = readBit(reader);
    if(current == 1) {
        root->value = readChar(reader);
        return;
    }
    else if(current == 0) {
        root->left = malloc(sizeof(struct node));
        root->left->left = NULL;
        root->left->right = NULL;

        root->right = malloc(sizeof(struct node));
        root->right->left = NULL;
        root->right->right = NULL;

        build_tree(root->left, reader);
        build_tree(root->right, reader);

        return;
    }
}

struct fileCopy* huffman_decode(size_t len, const char data[len]) {
    struct fileCopy* res = malloc(sizeof(struct fileCopy));

    struct bitReader* reader = generateBitReader(data, len);
    reader->fillerBitsSize = readChar(reader);

    unsigned nb_characters = readChar(reader);

    struct bitWriter* writer = generateBitWriter(len);
    char current_bit;

    if(nb_characters == 1) {
        char c = readChar(reader);
        current_bit = readBit(reader);

        while (current_bit != 'x') {
            writeChar(writer, c);
            current_bit = readBit(reader);
        }
    } else {
        struct node *root = malloc(sizeof(struct node));
        build_tree(root, reader);
        current_bit = readBit(reader);
        struct node *current_node;

        current_node = root;

        while (current_bit != 'x') {
            if (current_node->left == NULL) {
                writeChar(writer, current_node->value);
                current_node = root;
            } else {
                if (current_bit == 0) current_node = current_node->left;
                else current_node = current_node->right;
                current_bit = readBit(reader);
            }
        }
        writeChar(writer, current_node->value);
        free_tree(root);
    }

    res->len = writer->fileSize;
    res->data = malloc(res->len);

    closeBitReader(reader);
    closeBitWriter(writer, res->data);

    return res;
}