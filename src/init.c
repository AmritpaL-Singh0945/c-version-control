#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cvc.h"

int cvc_init(void) {
    // Check if .cvc already exists
    struct stat st = {0};
    if (stat(".cvc", &st) == 0) {
        printf("Error: Repository already initialized.\n");
        return 1;
    }

    // Create the .cvc directory and subdirectories
    if (mkdir(".cvc", 0755) != 0) {
        perror("Failed to create .cvc directory");
        return 1;
    }

    if (mkdir(".cvc/objects", 0755) != 0) {
        perror("Failed to create .cvc/objects directory");
        return 1;
    }

    if (mkdir(".cvc/refs", 0755) != 0) {
        perror("Failed to create .cvc/refs directory");
        return 1;
    }

    if (mkdir(".cvc/refs/heads", 0755) != 0) {
        perror("Failed to create .cvc/refs/heads directory");
        return 1;
    }

    // Create a HEAD file to point to main branch
    FILE *head_file = fopen(".cvc/HEAD", "w");
    if (head_file != NULL) {
        fprintf(head_file, "ref: refs/heads/main\n");
        fclose(head_file);
    } else {
        perror("Failed to create .cvc/HEAD");
        return 1;
    }

    printf("Initialized empty cvc repository in .cvc/\n");
    return 0;
}
