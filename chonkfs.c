/*
*        .__                   __      _____       
*   ____ |  |__   ____   ____ |  | ___/ ____\______
* _/ ___\|  |  \ /  _ \ /    \|  |/ /\   __\/  ___/
* \  \___|   Y  (  <_> )   |  \    <  |  |  \___ \ 
*  \___  >___|  /\____/|___|  /__|_ \ |__| /____  >
*      \/     \/            \/     \/           \/ 
*
* compile : gcc  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

void nultility();
void datatize(char *data);
void randomize();
void show_help();

int main(int argc, char *argv[]) {
    int size_to_reach = 0;

    // 1. If the user just runs "./chonkfs" without args, show help
    if (argc < 2) {
        show_help();
        return EXIT_FAILURE;
    }

    // 2. Loop through the arguments passed directly from the terminal launch
    // We start at i = 1 because i = 0 is always the program name execution path
    for (int i = 1; i < argc; i++){
        
        if (strcmp(argv[i], "--null") == 0){
            printf("Adding null bytes...\n");
            nultility();
        } 
        
        else if (strcmp(argv[i], "--random") == 0){
            printf("Adding random data...\n");
            randomize();
        } 
        
        else if (strcmp(argv[i], "--data") == 0){
            if (i + 1 < argc) {
                printf("Adding data: %s\n", argv[i + 1]);
                datatize(argv[i + 1]);
                i++; // Skip the data argument value string
            } else {
                fprintf(stderr, "Error: --data option requires an argument.\n");
                return EXIT_FAILURE;
            }
        }
        
        else if (strcmp(argv[i], "--size") == 0){
            if (i + 1 < argc) {
                size_to_reach = atoi(argv[i + 1]);
                printf("Setting size to reach: %d\n", size_to_reach);
                i++; // Skip the size numerical value
            } else {
                fprintf(stderr, "Error: --size option requires an argument.\n");
                return EXIT_FAILURE;
            }
        }
        
        else if (strcmp(argv[i], "--help") == 0) {
            show_help();
            return EXIT_SUCCESS;
        } 
        
        else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            show_help();
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////
void nultility(){
    // Put code to add zero bytes here
}
////////////////////////////////////////////////////////////////////////////////////////////
void datatize(char *data){
    // Put code to add specified custom data string here
}
////////////////////////////////////////////////////////////////////////////////////////////
void randomize(){
    // Put code to add random noise here
}
////////////////////////////////////////////////////////////////////////////////////////////
void show_help() {
    fprintf(stderr, "Usage: chonkfs [options]\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --null             Add null bytes\n");
    fprintf(stderr, "  --random           Add random data\n");
    fprintf(stderr, "  --data <string>    Add specific text data\n");
    fprintf(stderr, "  --size <bytes>     Set total size target\n");
    fprintf(stderr, "  --help             Show this instruction panel\n");
}
