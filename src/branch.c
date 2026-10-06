#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "cvc.h"

int cvc_branch(const char *branch_name) {
    char current_branch[256] = "";
    get_current_branch(current_branch);

    if (branch_name == NULL) {
        // List all branches
        DIR *dir = opendir(".cvc/refs/heads");
        if (!dir) {
            printf("Error: Could not open branches directory.\n");
            return 1;
        }
        
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') continue; // Skip hidden files
            
            // Mark the currently active branch with a star!
            if (strcmp(entry->d_name, current_branch) == 0) {
                printf("* %s\n", entry->d_name);
            } else {
                printf("  %s\n", entry->d_name);
            }
        }
        closedir(dir);
        return 0;
    } else {
        // Create a new branch
        char new_path[256];
        snprintf(new_path, sizeof(new_path), ".cvc/refs/heads/%s", branch_name);
        
        FILE *new_ref = fopen(new_path, "w");
        if (!new_ref) {
            printf("Error: Could not create branch.\n");
            return 1;
        }

        // Copy the current commit hash into the new branch file
        char curr_path[256];
        snprintf(curr_path, sizeof(curr_path), ".cvc/refs/heads/%s", current_branch);
        FILE *curr_ref = fopen(curr_path, "r");
        
        if (curr_ref) {
            char hash[256];
            if (fscanf(curr_ref, "%255s", hash) == 1) {
                fprintf(new_ref, "%s\n", hash);
            }
            fclose(curr_ref);
        }
        fclose(new_ref);
        
        printf("Branch '%s' created.\n", branch_name);
        return 0;
    }
}
