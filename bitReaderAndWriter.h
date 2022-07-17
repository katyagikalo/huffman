#include <stdint.h>
#include <stddef.h>

#ifndef BITREADERANDWRITER_H
#define BITREADERANDWRITER_H
struct bitWriter
{
    //Because only bytes can be written to the actual FILE, all bits that exceed this limit are stored in the buffer
    unsigned char buffer;
    //This gives Information about which bits of the buffer are currently relevant and have to be stored to the file later
    int bufferSize;
    //This is the in-memory representation of the current state of either the in- or output-file
    unsigned char *memoryFile;
    //Current number of characters in the memoryFile
    size_t fileSize;
};

struct bitReader
{
    //Because only bytes can be written to the actual FILE, all bits that exceed this limit are stored in the buffer
    unsigned char buffer;
    //This gives Information about which bits of the buffer are currently relevant and have to be stored to the file later
    int bufferSize;
    //This is the in-memory representation of the current state of either the in- or output-file
    unsigned char *memoryFile;
    //Total number of characters in the memoryFile
    size_t fileSize;
    //Number of currently read characters in the memoryFile
    size_t bytesRead;
    //Number of filler-bits at the end
    int fillerBitsSize;
};

struct bitWriter* generateBitWriter(size_t sizeOfFile);
void writeBit(struct bitWriter* writer, char bit);
void writeBits(struct bitWriter* writer,  uint8_t nb_bits, char bits[nb_bits]);
void writeBitsZeros(struct bitWriter* writer,  uint8_t nb_bits);
void writeChar(struct bitWriter* writer, char bits);
void closeBitWriter(struct bitWriter* writer, unsigned char* memoryFile);

struct bitReader* generateBitReader(unsigned char* memoryFile, size_t fileSize);
char readBit(struct bitReader* reader);
char readChar(struct bitReader* reader);
void closeBitReader(struct bitReader* reader);
#endif