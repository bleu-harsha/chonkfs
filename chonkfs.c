/*
* .__                   __      _____       
* ____ |  |__   ____   ____ |  | ___/ ____\______
* _/ ___\|  |  \ /  _ \ /    \|  |/ /\   __\/  ___/
* \  \___|   Y  (  <_> )   |  \    <  |  |  \___ \ 
* \___  >___|  /\____/|___|  /__|_ \ |__| /____  >
* \/     \/            \/     \/           \/ 
* Make files heavier than they were
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
#include <sys/vfs.h>

void datatize(char *data, FILE *file);
void randomize(FILE *file);
void defaultize(FILE *file);
void show_help();
char *allocate_malloc();
void read_data();
void read_size();
void output_file(int mode); 
uint64_t convert_to_bytes(const char *size_str);

uint64_t size_to_reach = 0;
char *malloc_size = NULL;

struct tasks_config {
    bool random;
    bool has_data;       
    bool run_defaultize;
    bool append_mode;
    char *output_filename;
    char *data_string;   
} tasks;

int main(int argc, char *argv[]) { 

    // Initialize struct components safely
    tasks.random = false;
    tasks.has_data = false;
    tasks.run_defaultize = false;
    tasks.data_string = NULL;
    tasks.append_mode = false;
    tasks.output_filename = NULL;

    if (argc < 2) {
        show_help();
        return EXIT_FAILURE;
    }

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            show_help();
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "--null") == 0){
            fprintf(stderr, "Adding null bytes...\n");
        }
        if (strcmp(argv[i], "--random") == 0){
            tasks.random = true;
        }
        if (strcmp(argv[i], "--data") == 0 ){
            if (i + 1 < argc) {
                tasks.has_data = true;
                tasks.data_string = argv[i + 1];
                i++;
            } else {
                fprintf(stderr, "Error: --data option requires a string payload argument.\n");
                return EXIT_FAILURE;
            }
        }
        if (strcmp(argv[i], "--size") == 0){
            if (i + 1 < argc) {
                size_to_reach = convert_to_bytes(argv[i + 1]);
                fprintf(stderr, "Setting size to reach: %llu bytes\n", (unsigned long long)size_to_reach);
                i++; 
            } else {
                fprintf(stderr, "Error: --size option requires an argument.\n");
                return EXIT_FAILURE;
            }
        }
        if (strcmp(argv[i], "--append") == 0){
            fprintf(stderr, "Appending data to existing file...\n");
            tasks.append_mode = true;
        }
    }

    FILE *file = fopen(argv[1], "ab+");
    if (file == NULL) {
        perror("Error: Could not open working target file"); 
        return EXIT_FAILURE;
    }
    else {
        struct statfs fs_info;
        int response = 0;
        const char *filename = argv[1];
        const char *dot = strrchr(filename, '.');
        
        if (statfs(filename, &fs_info) != 0) {
            perror("Warning: Could not extract filesystem attributes");
        } else {
            // Guard: Detect Read-Only Filesystems (ISO9660 signature match)
            if (fs_info.f_type == 0x9660) {
                fprintf(stderr, "WARNING: Target is located on a Read-Only filesystem structure. Mutation will fail.\n");
                fprintf(stderr, "Are you sure you want to continue? (y/n): ");
                response = getchar();
                if (response != 'y' && response != 'Y') {
                    fprintf(stderr, "Aborting operation safely.\n");
                    fclose(file);
                    return EXIT_SUCCESS;
                }
                while (getchar() != '\n' && getchar() != EOF);
            }
            
            // Guard: Detect raw block/virtual filesystem environments
            if (fs_info.f_type == 0x00001934) {
                fprintf(stderr, "WARNING: Modifying raw or virtual block allocations might cause structural storage corruption.\n");
                fprintf(stderr, "Are you sure you want to continue? (y/n): ");
                response = getchar();
                if (response != 'y' && response != 'Y') {
                    fprintf(stderr, "Aborting operation safely.\n");
                    fclose(file);
                    return EXIT_SUCCESS;
                }
                while (getchar() != '\n' && getchar() != EOF);
            }
        }

        // Guard: Detect System Root Directory Modifications
        if (filename[0] == '/' && strncmp(filename, "/tmp/", 5) != 0) {
            fprintf(stderr, "WARNING: Target file resides inside a critical system root path directory!\n");
            fprintf(stderr, "Are you sure you want to continue? (y/n): ");
            response = getchar();
            if (response != 'y' && response != 'Y') {
                fprintf(stderr, "Aborting operation safely.\n");
                fclose(file);
                return EXIT_SUCCESS;
            }
            while (getchar() != '\n' && getchar() != EOF); 
        }

        // Guard: ISO File Structure Mutation Lockout
        if (dot && strcmp(dot, ".iso") == 0) {
            fprintf(stderr, "WARNING: Target is an ISO disk file structure. Modification may fail.\n");
            fprintf(stderr, "Are you sure you want to continue? (y/n): ");
            response = getchar();
            if (response != 'y' && response != 'Y') {
                fprintf(stderr, "Aborting operation safely.\n");
                fclose(file);
                return EXIT_SUCCESS;
            }
            while (getchar() != '\n' && getchar() != EOF); 
        }

        // Guard: Missing File Type Extensions Check
        if (!dot || strrchr(filename, '/') > dot || strrchr(filename, '\\') > dot) {
            fprintf(stderr, "WARNING: Target file contains no identifiable extension format type.\n");
            fprintf(stderr, "Are you sure you want to continue? (y/n): ");
            response = getchar();
            if (response != 'y' && response != 'Y') {
                fprintf(stderr, "Aborting operation safely.\n");
                fclose(file);
                return EXIT_SUCCESS;
            }
            while (getchar() != '\n' && getchar() != EOF); 
        }

        // Guard: Filesystem Free Space Check
        if (fs_info.f_bavail * fs_info.f_bsize < size_to_reach && size_to_reach > 0) {
            fprintf(stderr, "WARNING: Target filesystem has insufficient free space for the specified size limit.\n");
            fprintf(stderr, "Available space: %llu bytes, Required space: %llu bytes\n", 
                    (unsigned long long)(fs_info.f_bavail * fs_info.f_bsize), 
                    (unsigned long long)size_to_reach);
            fprintf(stderr, "Are you sure you want to continue? (y/n): ");
            response = getchar();
            if (response != 'y' && response != 'Y') {
                fprintf(stderr, "Aborting operation safely.\n");
                fclose(file);
                return EXIT_SUCCESS;
            }
            while (getchar() != '\n' && getchar() != EOF);
        
        // Guard: File Permission Check
        } else if (access(filename, W_OK) != 0) {
            fprintf(stderr, "WARNING: Target file is not writable. Operation may fail.\n");
            fprintf(stderr, "Are you sure you want to continue? (y/n): ");
            response = getchar();
            if (response != 'y' && response != 'Y') {
                fprintf(stderr, "Aborting operation safely.\n");
                fclose(file);
                return EXIT_SUCCESS;
            }
            while (getchar() != '\n' && getchar() != EOF);
        }
    }

    // Execution routine logic paths
    if (tasks.has_data == false && tasks.random == false) {
        defaultize(file);
    } else {
        if (tasks.has_data) {
            datatize(tasks.data_string, file);
        }
        if (tasks.random) {
            randomize(file);
        }
    }

    fclose(file);
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
void datatize(char *data, FILE *file){
    if (!data) return;
    
    fseek(file, 0, SEEK_END);
    long current_size = ftell(file);

    if(size_to_reach == 0 || (uint64_t)current_size >= size_to_reach) return;
    uint64_t bytes_to_add = size_to_reach - (uint64_t)current_size;

    size_t chunk_size = 1024 * 1024;
    char *chunk = allocate_malloc();
    if (!chunk){
        fprintf(stderr, "Error: Memory allocation failed for sequence operations.\n");
        return;
    }

    size_t data_len = strlen(data);
    size_t offset = 0;
    size_t to_write = 0;
    
    while( bytes_to_add > 0 ){
        to_write = bytes_to_add > chunk_size ? chunk_size : (size_t)bytes_to_add;
        for(size_t i = 0; i < to_write; i++){
            chunk[i] = data[offset % data_len];
            offset++; 
        }
        fwrite(chunk, 1, to_write, file);
        bytes_to_add -= to_write;
    }
    free(chunk);
}

////////////////////////////////////////////////////////////////////////////////////////////
void randomize(FILE *file){
    fseek(file, 0, SEEK_END);
    long current_size = ftell(file);
    
    if (size_to_reach == 0 || (uint64_t)current_size >= size_to_reach) return;

    uint64_t bytes_to_add = size_to_reach - (uint64_t)current_size;
    srand((unsigned int)time(NULL));

    size_t chunk_size = 1024 * 1024;
    char *chunk = allocate_malloc(); 
    
    if (!chunk) {
        for(uint64_t i = 0; i < bytes_to_add; i++){
            if (fputc(rand() % 256, file) == EOF) {
                perror("Error: Writing byte stream fell into failure state");
                return;
            }
        }
        return;
    }

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
    fprintf(stderr, "No option arguments recognized. Mirroring file size properties...\n");
    fseek(file, 0, SEEK_END);
    long original_size = ftell(file);
    if (original_size <= 0){
        if (size_to_reach == 0) size_to_reach = 1024; 
        randomize(file); 
        return;
    }
    
    char *buffer = malloc(original_size);
    if (!buffer) {
        fprintf(stderr, "Error: Memory threshold depletion triggered allocation abort.\n");
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
    fprintf(stdout, "Make files heavier than they were.\n");
    fprintf(stderr, "Usage: chonkfs [options]\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --null                        Add null bytes layout structure\n");
    fprintf(stderr, "  --random                      Add random data noise payload\n");
    fprintf(stderr, "  --data <string>               Add specific text data\n");
    fprintf(stderr, "  --copy                        Add the files data blocks over dynamically\n");
    fprintf(stderr, "  --size <int,(MB/GB/TB/KB)>    Set complete file volume cap limits\n");
    fprintf(stderr, "  --out <filename>              Specify distinct output targets\n");
    fprintf(stderr, "  --append                      Append data mutations directly onto source files\n");
    fprintf(stderr, "  --read <filename>             Extract text datasets injected using --data\n");
    fprintf(stderr, "  --readsize <int>              Specify custom size ranges for target scans\n");
    fprintf(stderr, "  --help                        Display user helper documentation mapping panels\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////
char *allocate_malloc() {
    size_t chunk_size = 1024 * 1024;
    return calloc(chunk_size, 1);
}

void read_data(){}
void read_size(){}
void output_file(int mode){}

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