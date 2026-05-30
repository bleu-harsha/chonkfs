/*
* .__                   __      _____       
* ____ |  |__   ____   ____ |  | ___/ ____\______
* _/ ___\|  |  \ /  _ \ /    \|  |/ /\   __\/  ___/
* \  \___|   Y  (  <_> )   |  \    <  |  |  \___ \ 
* \___  >___|  /\____/|___|  /__|_ \ |__| /____  >
* \/     \/            \/     \/           \/ 
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
#include <unistd.h>
#include <fcntl.h>

void nultility(FILE *file);
void datatize(char *data, FILE *file);
void randomize(FILE *file);
void defaultize(FILE *file);
void show_help();
void allocate_malloc();
void read_data();
void read_size();
void output_file();
uint64_t convert_to_bytes(const char *size_str);
uint64_t size_to_reach = 0;

int main(int argc, char *argv[]) {
    bool unn = true;   
    if (argc < 2) {
        show_help();
        return EXIT_FAILURE;
    }

    // First Pass: Extract configurations like --size and --help, and check if we need to defaultize
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            show_help();
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "--null") == 0 || 
            strcmp(argv[i], "--random") == 0 || 
            strcmp(argv[i], "--data") == 0 ){
            unn = false; 
        }
        if (strcmp(argv[i], "--size") == 0){
            if (i + 1 < argc) {
                size_to_reach = convert_to_bytes(argv[i + 1]);
                printf("Setting size to reach: %llu bytes\n", (unsigned long long)size_to_reach);
                i++; 
            } else {
                fprintf(stderr, "Error: --size option requires an argument.\n");
                return EXIT_FAILURE;
            }
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
        // Second Pass: Execute the file inflation operations sequentially
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
                    i++; 
                } else {
                    fprintf(stderr, "Error: --data option requires an argument.\n");
                    fclose(file);
                    return EXIT_FAILURE;
                }
            }
            else if (strcmp(argv[i], "--size") == 0) {
                i++; // Safely skip the size value payload during the sequential build phase
            }
            else {
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                show_help();
                fclose(file);
                return EXIT_FAILURE;
            }
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Optimized to append null bytes using fast 1MB chunk buffering
void nultility(FILE *file){
    fseek(file, 0, SEEK_END);
    long current_size = ftell(file);
    
    if (size_to_reach == 0 || (uint64_t)current_size >= size_to_reach) return;
    
    uint64_t bytes_to_add = size_to_reach - (uint64_t)current_size;
    
    // Allocate a 1MB buffer chunk on the Heap filled with zeroes
    size_t chunk_size = 1024 * 1024;
    char *chunk = calloc(chunk_size, 1);
    
    if (!chunk) {
        // Fallback safety: write byte-by-byte if heap memory allocation fails
        char null_byte = 0;
        for (uint64_t i = 0; i < bytes_to_add; i++) {
            fputc(null_byte, file);
        }
        return;
    }

    // Stream the zeroed buffer chunks rapidly into the file
    while (bytes_to_add > 0) {
        size_t to_write = (bytes_to_add > chunk_size) ? chunk_size : (size_t)bytes_to_add;
        fwrite(chunk, 1, to_write, file);
        bytes_to_add -= to_write;
    }

    free(chunk);
}

////////////////////////////////////////////////////////////////////////////////////////////
void datatize(char *data, FILE *file){
    fseek(file, 0, SEEK_END);
    long current_size = ftell(file);

    if(size_to_reach == 0 || (uint64_t)current_size >= size_to_reach) return;
    uint64_t bytes_to_add = size_to_reach - (uint64_t)current_size;

    size_t chunk_size = 1024 * 1024;
    char *chunk = malloc(chunk_size);
    if (!chunk){
    // Fallback safety: write byte-by-byte if heap memory allocation fails
    char *ptr = data;
    for(uint64_t i = 0; i < bytes_to_add;i++){
        fputc(*ptr, file);
        i ++;
    }
    return;
}

    size_t data_len = strlen(data);
    size_t offset = 0;
    size_t to_write = 0;
    while( bytes_to_add > 0 ){
    to_write = bytes_to_add > chunk_size?chunk_size:(size_t)bytes_to_add;
        for(size_t i = 0; i < to_write; i++){
            chunk[i] = data[(offset + i ) % data_len];
            offset = (offset + 1 )% data_len;
        }
    fwrite(chunk, 1,to_write,file);
    bytes_to_add -= to_write;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// Optimized to append random noise using fast 1MB chunk buffering
void randomize(FILE *file){
    fseek(file, 0, SEEK_END);
    long current_size = ftell(file);
    
    if (size_to_reach == 0 || (uint64_t)current_size >= size_to_reach) return;

    uint64_t bytes_to_add = size_to_reach - (uint64_t)current_size;
    srand((unsigned int)time(NULL));

    // Allocate a 1MB buffer chunk on the Heap
    size_t chunk_size = 1024 * 1024;
    char *chunk = malloc(chunk_size);
    
    if (!chunk) {
        // Fallback safety: write byte-by-byte if heap memory allocation fails
        for(uint64_t i = 0; i < bytes_to_add; i++){
            fputc(rand() % 256, file);
        }
        return;
    }

    // Fill buffer chunk with random bytes iteratively and stream it out
    while (bytes_to_add > 0) {
        size_t to_write = (bytes_to_add > chunk_size) ? chunk_size : (size_t)bytes_to_add;
        
        for (size_t i = 0; i < to_write; i++) {
            chunk[i] = rand() % 256;
        }
        
        fwrite(chunk, 1, to_write, file);
        bytes_to_add -= to_write;
    }

    free(chunk);
}

////////////////////////////////////////////////////////////////////////////////////////////
void defaultize(FILE *file){
    printf("No options specified. Running defaultize to double file payload...\n");
    fseek(file, 0, SEEK_END);
    long original_size = ftell(file);
    if (original_size <= 0){
        // If file is completely empty and no size target was set, add a minimal fallback size
        if (size_to_reach == 0) size_to_reach = 1024; 
        randomize(file); 
        return;
    }
    
    char *buffer = malloc(original_size);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed for duplication\n");
        return;
    }
    
    fseek(file, 0, SEEK_SET);
    size_t read_bytes = fread(buffer, 1, original_size, file);

    fseek(file, 0, SEEK_END);
    fwrite(buffer, 1, read_bytes, file);
    free(buffer);
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
    fprintf(stderr, "  --default                     Add the files data over\n");
    fprintf(stderr, "  --size <int,(MB/GB/TB/KB)>    Set total size target\n");
    fprintf(stderr, "  --out <fiiename>                Specify output file (default: input file)\n");
    fprintf(stderr, "  --append                      Append data to existing file\n");
    fprintf(stderr, "  --read <filename>               Read data hidden with  \"--data\"\n");
    fprintf(stderr, "  --readsize <int,(MB/GB/TB/KB)>     Read specific size from file (default: all)\n");
    fprintf(stderr, "  --help                        Show this instruction panel\n");
    fprintf(stderr, "By default the program will generate the file with its size doubled with its contents doubled  \"\n");
    fprintf(stderr, "Example: chonkfs <file> --null --data \"hello world\" --size 100MB\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////
void allocate_malloc(){

}
///////////////////////////////////////////////////////////////////////////////////////////////
void read_data(){

}
////////////////////////////////////////////////////////////////////////////////////////////////
void read_size(){

}
////////////////////////////////////////////////////////////////////////////////////////////////
void output_file(){

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