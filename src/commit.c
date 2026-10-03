#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cvc.h"

int cvc_commit(const char *message) {
    // 1. Read the staging area (.cvc/index)
    FILE *index_file = fopen(".cvc/index", "r");
    if (!index_file) {
        printf("Error: Nothing to commit (index is empty).\n");
        return 1;
    }

    // Read index contents into memory
    fseek(index_file, 0, SEEK_END);
    long index_size = ftell(index_file);
    if (index_size == 0) {
        printf("Error: Nothing to commit (index is empty).\n");
        fclose(index_file);
        return 1;
    }
    fseek(index_file, 0, SEEK_SET);

    char *index_contents = malloc(index_size + 1);
    fread(index_contents, 1, index_size, index_file);
    index_contents[index_size] = '\0';
    fclose(index_file);

    // 2. Get the current branch
    char current_branch[256] = "";
    get_current_branch(current_branch);
    if (strlen(current_branch) == 0) {
        printf("Error: Not on any branch.\n");
        free(index_contents);
        return 1;
    }

    char ref_path[256];
    snprintf(ref_path, sizeof(ref_path), ".cvc/refs/heads/%s", current_branch);

    // Get the parent commit hash from the branch reference
    char parent_hash[256] = "none";
    FILE *head_ref = fopen(ref_path, "r");
    if (head_ref) {
        if (fscanf(head_ref, "%255s", parent_hash) != 1) {
            strcpy(parent_hash, "none");
        }
        fclose(head_ref);
    }

    // 3. Create the commit text
    // We combine the parent hash, the user's message, and the list of staged files
    size_t commit_size = 512 + strlen(message) + index_size;
    char *commit_data = malloc(commit_size);
    
    snprintf(commit_data, commit_size, "parent: %s\nmessage: %s\n---\n%s", parent_hash, message, index_contents);

    // 4. Hash the commit text to get a unique Commit ID
    unsigned long commit_id = hash_data((unsigned char *)commit_data, strlen(commit_data));
    
    // 5. Write the commit object to the objects folder
    char commit_path[256];
    snprintf(commit_path, sizeof(commit_path), ".cvc/objects/%lx", commit_id);
    FILE *commit_obj = fopen(commit_path, "w");
    if (!commit_obj) {
        printf("Error: Could not write commit object.\n");
        free(index_contents);
        free(commit_data);
        return 1;
    }
    fprintf(commit_obj, "%s", commit_data);
    fclose(commit_obj);

    // 6. Update the current branch to point to this new commit
    FILE *main_ref = fopen(ref_path, "w");
    if (main_ref) {
        fprintf(main_ref, "%lx\n", commit_id);
        fclose(main_ref);
    } else {
        printf("Error: Could not update the main branch reference.\n");
    }


    // 7. Clear the staging area (empty the index file) since we committed everything
    FILE *clear_index = fopen(".cvc/index", "w");
    if (clear_index) {
        fclose(clear_index);
    }

    printf("Committed successfully! Commit ID: %lx\n", commit_id);
    
    free(index_contents);
    free(commit_data);
    return 0;
}
