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
void get_input(int *argc, char argv[10][256]){
    char input[2560];
    printf("chonkfs> ");

    if (fgets(input, sizeof(input), stdin ) == NULL) {
        fprintf(stderr, "Error reading input.\n");
        exit(EXIT_FAILURE);
    }

    input[strcspn(input, "\n")] = '\0'; // Remove newline character
    char *token = strtok(input, " ");

    while (token != NULL) {

        if (*argc >= 10) {
            fprintf(stderr, "Too many arguments. Maximum is 10.\n");
            exit(EXIT_FAILURE);
        }

        strncpy(argv[*argc], token, 255);
        argv[*argc][255] = '\0'; // Ensure null-termination
        (*argc)++;
        token = strtok(NULL, " ");

    }
}

int main(){
    //cli parser btw
    // add null bytes,random data, said data,said data
    // --null
    // --random
    // --data "data to add"
    // size to reach
    int argc;
    char argv[10][256];
    get_input( &argc, argv);
    int size_to_reach = 0;

    if (argc == 0) {
        fprintf(stderr, "No command entered.\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[0], "chonkfs") != 0){
        fprintf(stderr, "Invalid command. Expected 'chonkfs'.\n");
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--null") == 0){
            printf("Adding null bytes...\n");
            nulity();
        } else if (strcmp(argv[i], "--random") == 0){
            printf("Adding random data...\n");
            randomize();
        } else if (strcmp(argv[i], "--data") == 0){
            if (i + 1 < argc) {
                printf("Adding data: %s\n", argv[i + 1]);
                datatize(argv[i +1]);
                i++; // Skip the next argument since it's the data
            } else {
                fprintf(stderr, "Error: --data option requires an argument.\n");
                return EXIT_FAILURE;
            }
        }else if (strcmp(argv[i], "--size") == 0){
            if (i + 1 < argc) {
                size_to_reach = atoi(argv[i + 1]);
                printf("Setting size to reach: %d\n", size_to_reach);
                i++; // Skip the next argument since it's the size
            }else if(strcmp(argv[i],"--help") == 0) {
                show_help();
                return EXIT_SUCCESS;
            } else {
                fprintf(stderr, "Error: --size option requires an argument.\n");
                return EXIT_FAILURE;
            }
        
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            show_help();
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void nultity(){}

void datatize(char *data){

}

void randomize(){

}

void show_help() {

}