#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "huffmanGeneral.h"

#ifndef HUFFMAN_ENCODE_H
#define HUFFMAN_ENCODE_H

struct fileCopy* huffman_encode(size_t len, const char data[len], int version);
#endif