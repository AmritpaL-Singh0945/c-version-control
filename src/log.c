#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cvc.h"

int cvc_log(void) {
    char current_branch[256] = "";
    get_current_branch(current_branch);
    if (strlen(current_branch) == 0) {
        printf("Error: Not on any branch.\n");
        return 1;
    }

    char current_commit[256];
    
    // Start at the current branch's latest commit
    char ref_path[256];
    snprintf(ref_path, sizeof(ref_path), ".cvc/refs/heads/%s", current_branch);
    
    FILE *head_ref = fopen(ref_path, "r");
    if (!head_ref) {
        printf("Error: Could not find branch '%s'.\n", current_branch);
        return 1;
    }
    if (fscanf(head_ref, "%255s", current_commit) != 1) {
        printf("No commits yet.\n");
        fclose(head_ref);
        return 0;
    }
    fclose(head_ref);

    // If the branch file is empty or explicitly says "none"
    if (strcmp(current_commit, "none") == 0 || strlen(current_commit) == 0) {
        printf("No commits yet.\n");
        return 0;
    }

    // Traverse the commit history backwards
    while (strcmp(current_commit, "none") != 0) {
        char commit_path[256];
        snprintf(commit_path, sizeof(commit_path), ".cvc/objects/%s", current_commit);
        
        FILE *commit_obj = fopen(commit_path, "r");
        if (!commit_obj) {
            printf("Error: Could not find commit object %s\n", current_commit);
            return 1;
        }

        char parent[256] = "none";
        char line[512];
        char message[512] = "";

        // Parse our plain-text commit file line by line
        while (fgets(line, sizeof(line), commit_obj)) {
            if (strncmp(line, "parent: ", 8) == 0) {
                sscanf(line, "parent: %255s", parent);
            } else if (strncmp(line, "message: ", 9) == 0) {
                // Copy the message but remove the newline at the end
                strcpy(message, line + 9);
                message[strcspn(message, "\n")] = 0; 
            } else if (strncmp(line, "---", 3) == 0) {
                // We reached the staged files section, we can stop reading for the simple log
                break; 
            }
        }
        fclose(commit_obj);

        // Print it nicely like real git log
        printf("commit %s\n", current_commit);
        printf("    %s\n\n", message);

        // Move to the parent commit for the next iteration
        strcpy(current_commit, parent);
    }

    return 0;
}
