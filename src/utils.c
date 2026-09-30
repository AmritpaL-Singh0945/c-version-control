#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cvc.h"

// A very simple hashing algorithm (djb2) to generate unique IDs for files.
// It is widely used because it's short, fast, and easy to explain.
unsigned long hash_data(const unsigned char *data, size_t length) {
    unsigned long hash = 5381;
    for (size_t i = 0; i < length; i++) {
        hash = ((hash << 5) + hash) + data[i]; // hash * 33 + current byte
    }
    return hash;
}

// Helper function to figure out which branch we are currently on
void get_current_branch(char *branch_name) {
    FILE *head = fopen(".cvc/HEAD", "r");
    if (!head) return;
    
    char ref[256];
    // .cvc/HEAD contains "ref: refs/heads/branch_name"
    if (fscanf(head, "ref: refs/heads/%255s", ref) == 1) {
        strcpy(branch_name, ref);
    }
    fclose(head);
}

// Checks if a given path matches any pattern in .cvcignore
int is_ignored(const char *path) {
    FILE *ignore_file = fopen(".cvcignore", "r");
    if (!ignore_file) return 0; // If no ignore file exists, nothing is ignored.

    char line[512];
    int ignored = 0;
    while (fgets(line, sizeof(line), ignore_file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;
        
        // Skip empty lines
        if (strlen(line) == 0) continue;

        // Simple match: If the path starts with the ignored pattern
        // Example: line="obj/", path="obj/main.o" -> Matches!
        if (strncmp(path, line, strlen(line)) == 0) {
            ignored = 1;
            break;
        }
    }
    fclose(ignore_file);
    return ignored;
}
