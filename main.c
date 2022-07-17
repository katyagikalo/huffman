#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include "bitReaderAndWriter.h"
#include "huffmanGeneral.h"
#include "huffmanDecoding.h"
#include "huffmanEncoding.h"

const char* info_message =
        " - - - - - - - - - - - - - - - - - - - - - - - - - - - - Usage: - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n"
        "Examples:\n"
        " %s input.txt                     <- Encodes the input.txt file and stores the result in a file called \"output.txt\".\n"
        " %s input.txt -V1 -d -o OutF.txt  <- Decodes the input.txt file using version 1 of the algorithm and stores the result in a file called \"OutF.txt\".\n"
        " %s -h                            <- Shows a description of all options and examples (this text) and exits the program.\n"
        "\n"
        "Positional arguments:\n"
        "  file     The input-file.\n"
        "\n"
        "Optional arguments:\n"
        "  -V X     Defines which version of encoding to use. (Irrelevant on decoding) (Default/main implementation: X = 1)\n"
        "  -B X     If set, the runtime is recorded, value represents how often the program is repeated. (default: X = 1)\n"
        "  -o file  Defines the output file. (default: file = output)\n"
        "  -d       If set, the input will be decoded, else it will be encoded.\n"
        "  -h       Shows a description of all options and examples (this text) and exits the program.\n"
        "  --help   Shows a description of all options and examples (this text) and exits the program.\n";

void print_info_message(const char* programName) {
    fprintf(stderr, info_message, programName, programName,programName);
}

//Methods without measurement
struct fileCopy* main_huffman_encode(size_t len, const char data[len], int version){
    return huffman_encode(len, data, version);
}

struct fileCopy* main_huffman_decode(size_t len, const char data[len]){
    return huffman_decode(len, data);
}

//Methods with measurement
struct fileCopy* main_huffman_encode_measurement(size_t len, const char data[len], int numberOfRepetitioins, int version){
    printf("Time for encoding is beeing measured:\n");
    printf("Run   | Duration (in seconds)\n");

    double totalTime = 0;

    struct fileCopy* result;

    struct timespec start;
    struct timespec end;


    clock_gettime(CLOCK_MONOTONIC, &start);
    result = huffman_encode(len, data, version);
    clock_gettime(CLOCK_MONOTONIC, &end);


    double time = end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);

    totalTime += time;
    
    printf("1     | %f\n", time);

    for (int run = 2; run <= numberOfRepetitioins; run++){
        clock_gettime(CLOCK_MONOTONIC, &start);
        huffman_encode(len, data, version);
        clock_gettime(CLOCK_MONOTONIC, &end);
           
        time = end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);

        if (run < 10) {
            printf("%d     | %f\n", run, time);
        } else if (run < 100) {
            printf("%d    | %f\n", run, time);
        } else if (run < 1000) {
            printf("%d   | %f\n", run, time);
        } else {
            printf("%d  | %f\n", run, time);
        }

        totalTime += time;
    }

    double averageTime = totalTime / numberOfRepetitioins;
    printf("Total time: %f s, Average time: %f s\n",totalTime, averageTime);

    return result;
}

struct fileCopy* main_huffman_decode_measurement(size_t len, const char data[len], int numberOfRepetitioins){
    printf("Time for decoding is beeing measured:\n");
    printf("Run   | Duration (in seconds)\n");

    double totalTime = 0;

    struct fileCopy* result;

    struct timespec start;
    struct timespec end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    result = huffman_decode(len, data);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double time = end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);
    
    printf("1     | %f\n", time);

    totalTime += time;

    for (int run = 2; run <= numberOfRepetitioins; run++){
        clock_gettime(CLOCK_MONOTONIC, &start);
        huffman_decode(len, data);
        clock_gettime(CLOCK_MONOTONIC, &end);
            
        time = end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);

        if (run < 10) {
            printf("%d     | %f\n", run, time);
        } else if (run < 100) {
            printf("%d    | %f\n", run, time);
        } else if (run < 1000) {
            printf("%d   | %f\n", run, time);
        } else {
            printf("%d  | %f\n", run, time);
        }
        

        totalTime += time;
    }

    double averageTime = totalTime / numberOfRepetitioins;
    printf("Total time: %f s, Average time: %f s\n",totalTime, averageTime);

    return result;
}

int main(int argc, char** argv) {
    //Struct which is passed to getopt_long() to map "--help" to "-h"
    static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
    };

    //Name of the program, used for printing info-messages
    const char* programName = argv[0];

    //Version of the implementation to be used
    int version = 1;

    //Number of times the program should run for run-time measurement. (If the value is zero, the run-time isn't measured, which is the default)
    int numberOfRepetitions = 0;

    //Output-file
    const char* outputFileName = "output.txt";

    //Input-file
    const char* inputFileName;
    char* inputData;
    size_t inputLen;

    //Information whether the file should be encoded or decoded (false = encode)
    bool codeMode = false;

    int opt;
    while ((opt = getopt_long(argc, argv, "V:B::o:dh",long_options, NULL)) != -1){
        switch (opt) {
            case 'V':
                version = atoi(optarg);
                if (version > 3 || version < 1) {
                    fprintf(stderr, "The given version doesn't exist!\n");
                    print_info_message(programName);
                    return EXIT_FAILURE;
                }
                break;
            case 'B':
                if (optarg == NULL) {
                    numberOfRepetitions = 1;
                }
                else {
                    numberOfRepetitions = atoi(optarg);
                }
                break;
            case 'd':
                codeMode = true;
                break;
            case 'o':
                outputFileName = optarg;
                break;
            case 'h':
                print_info_message(programName);
                return EXIT_SUCCESS;
            default:
                print_info_message(programName);
                return EXIT_FAILURE;
        }
    }

    if(optind >= argc) {
       //No positional argument found
       fprintf(stderr, "Missing positional argument for input file -- 'file'.\n");
       print_info_message(programName);
       return EXIT_FAILURE;
    }
    inputFileName = argv[optind];

    if (codeMode) {//Decode
        FILE* inputFile = fopen(inputFileName, "r");

        //Getting the size of the input file
        if (fseek(inputFile, 0, SEEK_END)) {
            fprintf(stderr, "The size of the input file couldn't be propperly calculated!\n");
            fclose(inputFile);
            return EXIT_FAILURE;
        }
        inputLen = ftell(inputFile);
        if (inputLen == 0) {
            printf("The input file is empty, there is nothing to decode.\n");
            fclose(inputFile);
            return EXIT_SUCCESS;
        }
        fseek(inputFile, 0, SEEK_SET);

        //Reading the file to memory
        inputData = malloc(inputLen);
        if (inputData == NULL) {
            fprintf(stderr, "The space for the inputData couldn't be allocated!\n");
            fclose(inputFile);
            free(inputData);
            return EXIT_FAILURE;
        }
        size_t resultIn = fread(inputData, 1, inputLen, inputFile);
        if (resultIn != inputLen) {
            fprintf(stderr, "Input file couldn't be read in correctly!\n"
                            "Number of characters read: %lu\n"
                            "Actual number of characters: %lu\n", 
                            resultIn, inputLen);
            fclose(inputFile);
            free(inputData);
            return EXIT_FAILURE;
        }
        fclose(inputFile);

        //Decoding the memory file
        struct fileCopy* decodedFile;
        if (numberOfRepetitions == 0) {
            decodedFile = main_huffman_decode(inputLen, inputData);
        } else {
            decodedFile = main_huffman_decode_measurement(inputLen, inputData, numberOfRepetitions);
        }
        if (decodedFile == NULL) {
            fprintf(stderr, "Decoding returned NULL.\n");
            return EXIT_FAILURE;
        }

        //Writing the result to the output file
        FILE* outputFile = fopen(outputFileName, "w");
        size_t resultOut = fwrite(decodedFile->data, 1, decodedFile->len, outputFile);
        if (resultOut != decodedFile->len) {
            fprintf(stderr, "Output file couldn't be correctly written!\n"
                            "Number of characters written: %lu\n"
                            "Actual number of characters: %lu\n", 
                            resultOut, decodedFile->len);
            free(decodedFile);
            fclose(outputFile);
            free(inputData);                
            return EXIT_FAILURE;
        }

        printf("Input was succesfully decoded and stored to the file \"%s\"\n", outputFileName);
        free(decodedFile);
        fclose(outputFile);
        free(inputData);
        return EXIT_SUCCESS;
    } else {//Encode
        FILE* inputFile = fopen(inputFileName, "r");

        //Getting the size of the input file
        if (fseek(inputFile, 0, SEEK_END)) {
            fprintf(stderr, "The size of the input file couldn't be propperly calculated!\n");
            fclose(inputFile);
            return EXIT_FAILURE;
        }
        inputLen = ftell(inputFile);
        if (inputLen == 0) {
            printf("The input file is empty, there is nothing to encode.\n");
            fclose(inputFile);
            return EXIT_SUCCESS;
        }
        fseek(inputFile, 0, SEEK_SET);

        //Reading the file to memory
        inputData = malloc(inputLen);
        if (inputData == NULL) {
            fprintf(stderr, "The space for the inputData couldn't be allocated!\n");
            fclose(inputFile);
            return EXIT_FAILURE;
        }
        size_t resultIn = fread(inputData, 1, inputLen, inputFile);
        if (resultIn != inputLen) {
            fprintf(stderr, "Input file couldn't be read in correctly!\n"
                            "Number of characters read: %lu\n"
                            "Actual number of characters: %lu\n", 
                            resultIn, inputLen);
            fclose(inputFile);
            return EXIT_FAILURE;
        }
        fclose(inputFile);

        //Encoding the memory file
        struct fileCopy* encodedFile;
        if (numberOfRepetitions == 0) {
            encodedFile = main_huffman_encode(inputLen, inputData, version);
        } else {
            encodedFile = main_huffman_encode_measurement(inputLen, inputData, numberOfRepetitions, version);
        }
        if (encodedFile == NULL) {
            fprintf(stderr, "Encoding return NULL, most likely the given version was not implemented.\n");
            return EXIT_FAILURE;
        }

        //Writing the result to the output file
        FILE* outputFile = fopen(outputFileName, "w");

        size_t resultOut = fwrite(encodedFile->data, 1, encodedFile->len, outputFile);
        if (resultOut != encodedFile->len) {
            fprintf(stderr, "Output file couldn't be correctly written!\n"
                            "Number of characters written: %lu\n"
                            "Actual number of characters: %lu\n", 
                            resultOut, encodedFile->len);
            free(encodedFile);
            fclose(outputFile);                
            return EXIT_FAILURE;
        }

        printf("Input was succesfully encoded and stored to the file \"%s\"\n", outputFileName);
        free(encodedFile);
        fclose(outputFile);
        return EXIT_SUCCESS;
    }
}
