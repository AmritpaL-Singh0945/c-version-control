#include <stdio.h>
#include <string.h>
#include "cvc.h"

int cvc_merge(const char *branch_name) {
    char current_branch[256] = "";
    get_current_branch(current_branch);
    if (strlen(current_branch) == 0) {
        printf("Error: Not on any branch.\n");
        return 1;
    }

    // 1. Get the commit hash of the branch we want to merge in
    char target_path[256];
    snprintf(target_path, sizeof(target_path), ".cvc/refs/heads/%s", branch_name);
    FILE *target_ref = fopen(target_path, "r");
    if (!target_ref) {
        printf("Error: Branch '%s' does not exist.\n", branch_name);
        return 1;
    }

    char merge_commit_hash[256];
    if (fscanf(target_ref, "%255s", merge_commit_hash) != 1) {
        printf("Error: Branch '%s' is empty.\n", branch_name);
        fclose(target_ref);
        return 1;
    }
    fclose(target_ref);

    // 2. Fast-forward: Update our current branch to point to the new commit
    char curr_path[256];
    snprintf(curr_path, sizeof(curr_path), ".cvc/refs/heads/%s", current_branch);
    FILE *curr_ref = fopen(curr_path, "w");
    if (curr_ref) {
        fprintf(curr_ref, "%s\n", merge_commit_hash);
        fclose(curr_ref);
    }

    // 3. Update the working directory by checking out our newly updated branch
    printf("Fast-forwarding '%s' to '%s'...\n", current_branch, branch_name);
    
    // We can reuse our checkout logic to update the files!
    return cvc_checkout(current_branch);
}
