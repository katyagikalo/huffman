#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitReaderAndWriter.h"
#include "huffmanEncoding.h"
#include "minHeap.h"

static void printTree(struct HuffmanTree* root) {
    printf("%c\n", root->character);
    if (root->left != NULL && root->right != NULL) {
        printf("left\n");
        printTree(root->left);
        printf("right\n");
        printTree(root->right);
    }
}

void free_tree(struct HuffmanTree* root) {
    if(root->left != NULL) {
        free_tree(root->left);
        free_tree(root->right);
    }

    free(root);
}

//struct HuffmanTree* generateTree() {
//    struct HuffmanTree* root = malloc(sizeof(struct HuffmanTree));
//    root->left = malloc(sizeof(struct HuffmanTree));
//    root->left->left = malloc(sizeof(struct HuffmanTree));
//    root->left->left->left
//    root->left->right = malloc(sizeof(struct HuffmanTree));
//    root->right = malloc(sizeof(struct HuffmanTree));
//}

struct HuffmanTree* build_tree_naive(size_t len, struct HuffmanTree* nodes[len]){

    struct HuffmanTree* root;
    struct HuffmanTree* temp;
    do {
        root = malloc(sizeof(struct HuffmanTree));
        root->left = nodes[0];
        root->right = nodes[1];
        root->value = nodes[0]->value + nodes[1]->value;
        nodes[0] = root;

        //pushing the second node to the end
        for(int i = 1; i < len - 1; i++){
            temp = nodes[i];
            nodes[i] = nodes[i+1];
            nodes[i+1] = temp;
        }
        //free(nodes[len-1]);
        len--;

        //moving the combined node to the right place
        for(int i = 0; i < len - 1 && nodes[i]->value > nodes[i+1]->value; i++){
            temp = nodes[i];
            nodes[i] = nodes[i+1];
            nodes[i+1] = temp;
        }

    } while(len > 1);

    return nodes[0];
}

static void searchCodes(uint16_t nb_chars, struct HuffmanTree* cur, char codes[256][nb_chars - 1], uint8_t codeLen[256], char code[nb_chars - 1], uint16_t lvl) {
    if (lvl > (nb_chars - 1)) return;
    if (cur->left == NULL && cur->right == NULL) {
        for (int b = 0; b < lvl; b++) {
            codes[(uint8_t) cur->character][b] = code[b];
        }
        codeLen[(uint8_t) cur->character] = lvl;
        return;
    }
    code[lvl] = '0';
    searchCodes(nb_chars, cur->left, codes, codeLen, code, lvl + 1);
    code[lvl] = '1';
    if (cur->right == NULL) return;
    searchCodes(nb_chars, cur->right, codes, codeLen, code, lvl + 1);
} 

static void encodeTree(struct HuffmanTree* cur, struct bitWriter* writer) {
    if (cur->left == NULL && cur->right == NULL) {
        writeBit(writer, '1');
        writeChar(writer, cur->character);
        return;
    }
    writeBit(writer, '0');
    encodeTree(cur->left, writer);
    if (cur->right == NULL) return;
    encodeTree(cur->right, writer);
}

struct fileCopy* huffman_encode(size_t len, const char data[len], int version) {
    uint8_t x = 0; // x is the padding number
    size_t length;
    int ascii[256];
    int i;
    int j;
    for (i = 0; i < 256; i++) {
        ascii[i] = 0;
    }
    uint16_t nb_chars = 0;
    for (j = 0; j < len; j++) {
        if (ascii[(uint8_t) data[j]]++ == 0) nb_chars++;
    }
    if (nb_chars == 1) {
        while ((len + x) % 8 != 0) {
            x++;
        }
        length = (len + x)/8 + 3;
        struct bitWriter* writer = generateBitWriter(length);
        writeChar(writer, x);
        writeChar(writer, 1);
        writeChar(writer, data[0]);
        writeBitsZeros(writer, len + x);
        struct fileCopy* file = malloc(sizeof(struct fileCopy));
        file->data = malloc(length * sizeof(char));
        file->len = length;
        closeBitWriter(writer, file->data);
        return file;
    } else {
        struct HuffmanTree* nodes[nb_chars];
        for (i = 0; i < nb_chars; i++) {
            nodes[i] = malloc(sizeof(struct HuffmanTree));
            nodes[i]->value = 0;
            nodes[i]->left = NULL;
            nodes[i]->right = NULL;
        }
        uint16_t pos = 0;
        uint16_t new_pos;
        for (i = 0; i < 256 && pos < nb_chars; i++) {
            if (ascii[i] > 0) {
                new_pos = pos + 1;
                while(pos > 0 && nodes[pos-1]->value > ascii[i]) {
                    nodes[pos]->value = nodes[pos-1]->value;
                    nodes[pos]->character = nodes[pos-1]->character;
                    pos--;
                }
                nodes[pos]->value = ascii[i];
                nodes[pos]->character = (char) i;
                pos = new_pos;
            }
        }    

        struct HuffmanTree* root;
        switch (version) {
            case 1:
                root = build_tree_naive(nb_chars, nodes);//Katyas tree builder
                break;
            case 2:
                root = buildTree(nb_chars, nodes);
                break;
        }
        
        printTree(root); //TODO Remove

        char codes[256][nb_chars - 1];
        uint8_t codeLen[256];
        char code [nb_chars - 1];
        searchCodes(nb_chars, root, codes, codeLen, code, 0);
        length = (nb_chars * 10) + 15; // 8 for padding number, 8 for nb_chars byte, and the size of the encoded tree is always (10 * nb_chars) - 1
        for (j = 0; j < nb_chars; j++) {
            length += nodes[j]->value * codeLen[(uint8_t) nodes[j]->character]; // we add the size of encoded text
        }
        while ((length + x) % 8 != 0) {
            x++;
        }
        struct bitWriter* writer = generateBitWriter((length + x)/8);
        writeChar(writer, x);
        if (nb_chars == 256) writeChar(writer, (char) 0);
        else writeChar(writer, (char) nb_chars);
        if (nb_chars == 1) writeChar(writer, nodes[0]->character);
        else encodeTree(root, writer);
        for (i = 0; i < len; i++) {
            writeBits(writer, codeLen[(int) data[i]], codes[(int) data[i]]);
        }
        writeBitsZeros(writer, x);

        struct fileCopy* file = malloc(sizeof(struct fileCopy));
        file->len = (length + x) / 8;
        file->data = malloc(file->len * sizeof(char));
        closeBitWriter(writer, file->data);
        free_tree(root);
        return file;
    }
}