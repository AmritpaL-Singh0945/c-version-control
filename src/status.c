#include <stdio.h>
#include <stdlib.h>
#include "cvc.h"

int cvc_status(void) {
    FILE *index_file = fopen(".cvc/index", "r");
    if (!index_file) {
        printf("No repository found, or index is missing.\n");
        return 1;
    }

    char current_branch[256] = "unknown";
    get_current_branch(current_branch);

    // Check if the file is empty
    fseek(index_file, 0, SEEK_END);
    if (ftell(index_file) == 0) {
        printf("On branch %s\n\nNo files staged for commit.\n", current_branch);
        fclose(index_file);
        return 0;
    }
    fseek(index_file, 0, SEEK_SET); // Reset to beginning

    printf("On branch %s\n\nChanges to be committed:\n", current_branch);
    char hash[256];
    char filename[256];
    
    // Read the index line by line and print beautifully
    while (fscanf(index_file, "%255s %255s", hash, filename) == 2) {
        printf("  staged: %s\n", filename);
    }
    
    fclose(index_file);
    return 0;
}
