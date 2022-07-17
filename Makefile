CC = gcc
CFLAGS = -Wall -O2
OBJFILES = main.c bitReaderAndWriter.c huffmanDecoding.c huffmanEncoding.c minHeap.c
TARGET = main

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

.PHONY: clean
clean:
	rm -f $(TARGET)