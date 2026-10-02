#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "cvc.h"

// Helper function to add a single file (our old logic)
int add_single_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open file '%s'\n", filename);
        return 1;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *buffer = NULL;
    if (size > 0) {
        buffer = malloc(size);
        if (!buffer) {
            printf("Error: Memory allocation failed\n");
            fclose(file);
            return 1;
        }
        fread(buffer, 1, size, file);
    }
    fclose(file);

    // Hash the file contents
    unsigned long id = hash_data(buffer, size);
    
    // Save to objects
    char obj_path[256];
    snprintf(obj_path, sizeof(obj_path), ".cvc/objects/%lx", id);

    FILE *obj_file = fopen(obj_path, "wb");
    if (!obj_file) {
        printf("Error: Could not write to object database\n");
        if (buffer) free(buffer);
        return 1;
    }
    if (size > 0) {
        fwrite(buffer, 1, size, obj_file);
    }
    fclose(obj_file);
    if (buffer) free(buffer);

    // Update index
    FILE *index_file = fopen(".cvc/index", "a");
    if (!index_file) {
        printf("Error: Could not update index\n");
        return 1;
    }
    fprintf(index_file, "%lx %s\n", id, filename);
    fclose(index_file);

    printf("Staged '%s' (saved as object %lx)\n", filename, id);
    return 0;
}

int cvc_add(const char *path) {
    struct stat st;
    
    // Check if the path exists
    if (stat(path, &st) != 0) {
        printf("Error: Path '%s' does not exist.\n", path);
        return 1;
    }

    // 1. If it is a directory, traverse it
    if (S_ISDIR(st.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir) {
            printf("Error: Could not open directory '%s'\n", path);
            return 1;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            // Ignore current dir (.), parent dir (..), and our .cvc folder
            if (strcmp(entry->d_name, ".") == 0 || 
                strcmp(entry->d_name, "..") == 0 ||
                strcmp(entry->d_name, ".cvc") == 0) {
                continue;
            }

            // Create the full path to the sub-file or sub-folder
            char full_path[1024];
            
            // Handle if the user typed something like "src/" vs "src"
            if (path[strlen(path) - 1] == '/') {
                snprintf(full_path, sizeof(full_path), "%s%s", path, entry->d_name);
            } else {
                snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            }
            
            // RECURSION: Call this exact same function on the new path!
            cvc_add(full_path);
        }
        closedir(dir);
    } 
    // 2. If it is just a regular file, add it
    else if (S_ISREG(st.st_mode)) {
        add_single_file(path);
    }

    return 0;
}
