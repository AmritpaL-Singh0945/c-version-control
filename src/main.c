#include <stdio.h>
#include <string.h>
#include "cvc.h"

void print_usage() {
    printf("Usage: cvc <command>\n");
    printf("Commands:\n");
    printf("  init    Initialize a new empty cvc repository\n");
    printf("  add     Add file contents to the staging area\n");
    printf("  commit  Commit staged files to history (-m \"message\")\n");
    printf("  status  Show the working tree status\n");
    printf("  log     Show commit logs\n");
    printf("  branch  List or create branches\n");
    printf("  checkout Switch branches\n");
    printf("  merge   Fast-forward merge a branch\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        return cvc_init();
    } else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Error: 'add' requires a filename (e.g., cvc add main.c)\n");
            return 1;
        }
        return cvc_add(argv[2]);
    } else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 4 || strcmp(argv[2], "-m") != 0) {
            printf("Error: 'commit' requires a message (e.g., cvc commit -m \"My message\")\n");
            return 1;
        }
        return cvc_commit(argv[3]);
    } else if (strcmp(argv[1], "status") == 0) {
        return cvc_status();
    } else if (strcmp(argv[1], "log") == 0) {
        return cvc_log();
    } else if (strcmp(argv[1], "branch") == 0) {
        if (argc == 2) {
            return cvc_branch(NULL);
        } else {
            return cvc_branch(argv[2]);
        }
    } else if (strcmp(argv[1], "checkout") == 0) {
        if (argc < 3) {
            printf("Error: 'checkout' requires a branch name\n");
            return 1;
        }
        return cvc_checkout(argv[2]);
    } else if (strcmp(argv[1], "merge") == 0) {
        if (argc < 3) {
            printf("Error: 'merge' requires a branch name\n");
            return 1;
        }
        return cvc_merge(argv[2]);
    } else {
        printf("Unknown command: %s\n", argv[1]);
        print_usage();
        return 1;
    }

    return 0;
}
