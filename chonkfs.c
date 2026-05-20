/*
*        .__                   __      _____       
*   ____ |  |__   ____   ____ |  | ___/ ____\______
* _/ ___\|  |  \ /  _ \ /    \|  |/ /\   __\/  ___/
* \  \___|   Y  (  <_> )   |  \    <  |  |  \___ \ 
*  \___  >___|  /\____/|___|  /__|_ \ |__| /____  >
*      \/     \/            \/     \/           \/ 
*
* compile : gcc chonkfs.c -o chonkfs
* usage   : ./chonkfs [options]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>

void nultility(FILE *file);
void datatize(char *data, FILE *file);
void randomize(FILE *file);
void defaultize(FILE *file);
void show_help();
uint64_t convert_to_bytes(const char *size_str);
uint64_t size_to_reach = 0;

int main(int argc, char *argv[]) {
    bool unn = true;     
    if (argc < 2) {
        show_help();
        return EXIT_FAILURE;
    }

    // Check argv elements first to determine if we run defaultize, help, or manual options
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            show_help();
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "--null") == 0 || 
            strcmp(argv[i], "--random") == 0 || 
            strcmp(argv[i], "--data") == 0 ){
            unn = false; //  Necessary options detected, do not defaultize
        }
    }

    FILE *file = fopen(argv[1], "ab+");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (unn == true) {
        defaultize(file);
    }
    else {
        for (int i = 2; i < argc; i++){
        
            if (strcmp(argv[i], "--null") == 0){
                printf("Adding null bytes...\n");
                nultility(file);
            } 
        
            else if (strcmp(argv[i], "--random") == 0){
                printf("Adding random data...\n");
                randomize(file);
            } 
        
            else if (strcmp(argv[i], "--data") == 0){
                if (i + 1 < argc) {
                    printf("Adding data: %s\n", argv[i + 1]);
                    datatize(argv[i + 1], file);
                    i++; // Skip the data argument value string
                } else {
                    fprintf(stderr, "Error: --data option requires an argument.\n");
                    fclose(file);
                    return EXIT_FAILURE;
                }
            }
        
            else if (strcmp(argv[i], "--size") == 0){
                if (i + 1 < argc) {
                    // Convert the size argument to bytes and store it in the global variable
                    size_to_reach = convert_to_bytes(argv[i + 1]);
                    printf("Setting size to reach: %llu\n", (unsigned long long)size_to_reach);
                    i++; // Skip the size numerical value
                } else {
                    fprintf(stderr, "Error: --size option requires an argument.\n");
                    fclose(file);
                    return EXIT_FAILURE;
                }
            }
            else {
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                show_help();
                fclose(file);
                return EXIT_FAILURE;
            }
        }
    }

    // Suppress unused warning for size_to_reach until file generator math is hooked up
    (void)size_to_reach;

    fclose(file);
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
void nultility(FILE *file){
    // Put code to add zero bytes here
    fseek(file, 0, SEEK_END);
}
////////////////////////////////////////////////////////////////////////////////////////////
void datatize(char *data, FILE *file){
    // Put code to add specified custom data string here
    (void)data;
    (void)file;
}
////////////////////////////////////////////////////////////////////////////////////////////
void randomize(FILE *file){
    // Put code to add random noise here
    (void)file;
}
////////////////////////////////////////////////////////////////////////////////////////////
void defaultize(FILE *file){
    // Put code to add default data (e.g., duplicate existing content) here
    (void)file;
    printf("No options specified. Running defaultize to double file payload...\n");
}
////////////////////////////////////////////////////////////////////////////////////////////
void show_help() {
    fprintf(stdout, "        .__                   __      _____       \n");
    fprintf(stdout, "   ____ |  |__   ____   ____ |  | ___/ ____\\______\n");
    fprintf(stdout, " _/ ___\\|  |  \\ /  _ \\ /    \\|  |/ /\\   __\\/  ___/\n");
    fprintf(stdout, " \\  \\___|   Y  (  <_> )   |  \\    <  |  |  \\___ \\ \n");
    fprintf(stdout, "  \\___  >___|  /\\____/|___|  /__|_ \\ |__| /____  >\n");
    fprintf(stdout, "      \\/     \\/            \\/     \\/           \\/ \n\n");
    fprintf(stdout, "make files heavier than they were\n");
    fprintf(stderr, "Usage: chonkfs [options]\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --null                        Add null bytes\n");
    fprintf(stderr, "  --random                      Add random data\n");
    fprintf(stderr, "  --data <string>               Add specific text data\n");
    fprintf(stderr, "  --size <int,(MB/GB/TB/KB)>     Set total size target\n");
    fprintf(stderr, "  --help                        Show this instruction panel\n");
    fprintf(stderr, "By default the program will generate the file with its size doubled\"\n");
    fprintf(stderr, "Example: chonkfs <file> --null --data \"son\" --size 100MB\n");
}
////////////////////////////////////////////////////////////////////////////////////////////////

uint64_t convert_to_bytes(const char *size_str) {
    char *endptr;

    uint64_t value = strtoull(size_str, &endptr, 10);

    if (*endptr == '\0') {
        return value;
    }

    while (*endptr == ' ') {
        endptr++;
    }

    char unit1 = toupper((unsigned char)endptr[0]);
    char unit2 = endptr[0] != '\0' ? toupper((unsigned char)endptr[1]) : '\0';

    if (unit1 == 'K' && unit2 == 'B') {
        return value * 1024ULL;
    } 
    else if (unit1 == 'M' && unit2 == 'B') {
        return value * 1024ULL * 1024ULL;
    } 
    else if (unit1 == 'G' && unit2 == 'B') {
        return value * 1024ULL * 1024ULL * 1024ULL;
    } 
    else if (unit1 == 'T' && unit2 == 'B') {
        return value * 1024ULL * 1024ULL * 1024ULL * 1024ULL;
    } 
    else {
        fprintf(stderr, "Invalid size format: %s\n", size_str);
        show_help();
        exit(EXIT_FAILURE);
    }
}