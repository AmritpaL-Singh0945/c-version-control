#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cvc.h"

int cvc_checkout(const char *branch_name) {
    // 1. Check if the branch exists
    char branch_path[256];
    snprintf(branch_path, sizeof(branch_path), ".cvc/refs/heads/%s", branch_name);
    FILE *branch_ref = fopen(branch_path, "r");
    if (!branch_ref) {
        printf("Error: Branch '%s' does not exist.\n", branch_name);
        return 1;
    }
    
    char commit_hash[256];
    if (fscanf(branch_ref, "%255s", commit_hash) != 1) {
        printf("Error: Branch is empty.\n");
        fclose(branch_ref);
        return 1;
    }
    fclose(branch_ref);

    // 2. Open that commit object
    char commit_path[256];
    snprintf(commit_path, sizeof(commit_path), ".cvc/objects/%s", commit_hash);
    FILE *commit_obj = fopen(commit_path, "r");
    if (!commit_obj) {
        printf("Error: Commit object missing.\n");
        return 1;
    }

    // 3. Clear our staging area and rebuild it with the checked-out files
    FILE *index_file = fopen(".cvc/index", "w");
    char line[512];
    int reading_files = 0;
    
    // Parse the commit file line-by-line
    while (fgets(line, sizeof(line), commit_obj)) {
        if (strncmp(line, "---", 3) == 0) {
            reading_files = 1;
            continue;
        }
        
        if (reading_files) {
            char file_hash[256];
            char file_name[256];
            if (sscanf(line, "%255s %255s", file_hash, file_name) == 2) {
                // Restore the file from our database back into the working folder
                char obj_path[256];
                snprintf(obj_path, sizeof(obj_path), ".cvc/objects/%s", file_hash);
                
                FILE *obj_f = fopen(obj_path, "rb");
                FILE *out_f = fopen(file_name, "wb");
                
                if (obj_f && out_f) {
                    char buffer[1024];
                    size_t bytes;
                    while ((bytes = fread(buffer, 1, sizeof(buffer), obj_f)) > 0) {
                        fwrite(buffer, 1, bytes, out_f);
                    }
                    if (index_file) fprintf(index_file, "%s %s\n", file_hash, file_name);
                }
                
                if (obj_f) fclose(obj_f);
                if (out_f) fclose(out_f);
            }
        }
    }
    fclose(commit_obj);
    if (index_file) fclose(index_file);

    // 4. Update the HEAD file to say we are now on this branch
    FILE *head = fopen(".cvc/HEAD", "w");
    if (head) {
        fprintf(head, "ref: refs/heads/%s\n", branch_name);
        fclose(head);
    }

    printf("Switched to branch '%s'\n", branch_name);
    return 0;
}
