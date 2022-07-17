#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "bitReaderAndWriter.h"

static void writeOutBuffer(struct bitWriter* writer) {
    (writer->memoryFile)[writer->fileSize] = writer->buffer;
    writer->fileSize++;
    writer->bufferSize = 0;
}

static bool readInBuffer(struct bitReader* reader) {
    if (reader->bytesRead == reader->fileSize) {
        return true;//This means that the end if the file was reached
    } else {
        reader->buffer = (reader->memoryFile)[(reader->bytesRead)];
        reader->bytesRead++;
        reader->bufferSize = 8;

        //For the last bit, custom buffer size is needed in case not all bits from the last byte are actual data
        if (reader->bytesRead == reader->fileSize) {
            reader->bufferSize = 8 - (reader->fillerBitsSize);
        }

        return false;
    }
}

// - - - - - - - - - - - - "public" methods from here on - - - - - - - - - - - - 

struct bitWriter* generateBitWriter(size_t sizeOfFile) {
    struct bitWriter* reader = malloc(sizeof(struct bitWriter));
    if (reader == NULL) {
        fprintf(stderr, "The space for the bitReaderAndWriter couldn't be allocated!\n");
    }

    reader->memoryFile = malloc(sizeOfFile);
    if (reader->memoryFile == NULL) {
        fprintf(stderr, "The space for the digital file couldn't be allocated!\n");
    }

    reader->bufferSize = 0;
    reader->fileSize = 0;

    return reader;
}

void writeBit(struct bitWriter* writer, char bit) {
    writer->buffer <<= 1;
    bit &= 0x01;
    writer->buffer |= bit;
    writer->bufferSize++;

    if (writer->bufferSize == 8) {
        writeOutBuffer(writer);
    }
}

void writeBits(struct bitWriter* writer, uint8_t nb_bits, char bits[nb_bits]) {
    for (int pos = 0; pos < nb_bits; pos++) {
        writeBit(writer, bits[pos]);
    }
}
void writeBitsZeros(struct bitWriter* writer, uint8_t nb_bits) {
    for (int pos = 0; pos < nb_bits; pos++) {
        writeBit(writer, '0');
    }
}

void writeChar(struct bitWriter* writer, char bits) {
    int restSizeBuffer = 8-(writer->bufferSize);

    char thisBufferPart = bits;
    thisBufferPart >>= (writer->bufferSize);

    switch (restSizeBuffer) {
        case 1:
            thisBufferPart &= 0x01;
            writer->buffer <<= 1;
            break;
        case 2:
            thisBufferPart &= 0x03;
            writer->buffer <<= 2;
            break;
        case 3:
            thisBufferPart &= 0x07;
            writer->buffer <<= 3;
            break;
        case 4:
            thisBufferPart &= 0x0F;
            writer->buffer <<= 4;
            break;
        case 5:
            thisBufferPart &= 0x1F;
            writer->buffer <<= 5;
            break;
        case 6:
            thisBufferPart &= 0x3F;
            writer->buffer <<= 6;
            break;
        case 7:
            thisBufferPart &= 0x7F;
            writer->buffer <<= 7;
            break;
        case 8:
            thisBufferPart &= 0xFF;//TODO reduzieren
            writer->buffer <<= 8;
            break;
    }
    writer->buffer |= thisBufferPart;

    writeOutBuffer(writer);

    unsigned char nextBufferPart = bits;
    writer->buffer = nextBufferPart;
    switch (restSizeBuffer) {
        case 1:
            writer->bufferSize = 7;
            break;
        case 2:
            writer->bufferSize = 6;
            break;
        case 3:
            writer->bufferSize = 5;
            break;
        case 4:
            writer->bufferSize = 4;
            break;
        case 5:
            writer->bufferSize = 3;
            break;
        case 6:
            writer->bufferSize = 2;
            break;
        case 7:
            writer->bufferSize = 1;
            break;
        //case 8 is irellevant because we wouldn't do anything anyways
    }
}

void closeBitWriter(struct bitWriter* writer, unsigned char* memoryFile) {
    writer->buffer <<= 8 - (writer->bufferSize);
    writeOutBuffer(writer);

    for (size_t pos = 0; pos < writer->fileSize; pos++) {
        memoryFile[pos] = (writer->memoryFile)[pos];
    }

    free(writer->memoryFile);
    free(writer);
}

struct bitReader* generateBitReader(unsigned char *memoryFile, size_t fileSize) {
    struct bitReader* reader = malloc(sizeof(struct bitReader));
    if (reader == NULL) {
        fprintf(stderr, "The space for the bitReader couldn't be allocated!\n");
    }

    reader->memoryFile = malloc(fileSize);

    if (reader->memoryFile == NULL) {
        fprintf(stderr, "The space for the digital file couldn't be allocated!\n");
    }
    
    for (size_t pos = 0; pos < fileSize; pos++) {
        (reader->memoryFile)[pos] = memoryFile[pos];
    }
    
    reader->fileSize = fileSize;

    reader->fillerBitsSize = 0;

    reader->bytesRead = 0;

    reader->bufferSize = 0;

    reader->buffer = 0;

    return reader;
}

char readBit(struct bitReader* reader){
    if (reader->bufferSize == 0) {
        if (readInBuffer(reader)) {
            return 'x'; //This means that the end of the file (In bits) was reached
        }
    }

    char out = reader->buffer & 0x80;
    reader->buffer <<= 1;
    reader->bufferSize--;
    return (out >> 7) & 0x01;
}

char readChar(struct bitReader* reader){
    char reconstructedByte = reader->buffer;
    int amountBitsLeftToLoad = 8-(reader->bufferSize);

    readInBuffer(reader);

    char bitsLeftToLoad = reader->buffer;

    switch (amountBitsLeftToLoad) {
        case 1:
            bitsLeftToLoad >>= 7;
            bitsLeftToLoad &= 0x01;
            break;
        case 2:
            bitsLeftToLoad >>= 6;
            bitsLeftToLoad &= 0x03;
            break;
        case 3:
            bitsLeftToLoad >>= 5;
            bitsLeftToLoad &= 0x07;
            break;
        case 4:
            bitsLeftToLoad >>= 4;
            bitsLeftToLoad &= 0x0F;
            break;
        case 5:
            bitsLeftToLoad >>= 3;
            bitsLeftToLoad &= 0x1F;
            break;
        case 6:
            bitsLeftToLoad >>= 2;
            bitsLeftToLoad &= 0x3F;
            break;
        case 7:
            bitsLeftToLoad >>= 1;
            bitsLeftToLoad &= 0x7F;
            break;
        case 8:
            bitsLeftToLoad >>= 0;
            bitsLeftToLoad &= 0xFF;
            break;
    }

    reconstructedByte |= bitsLeftToLoad;

    reader->buffer <<= amountBitsLeftToLoad;
    reader->bufferSize -= amountBitsLeftToLoad;

    return reconstructedByte;
}

void closeBitReader(struct bitReader* reader) {
    free(reader->memoryFile);
    free(reader);
}