#ifndef CVC_H
#define CVC_H

// Core CVC functionalities
int cvc_init(void);
int cvc_add(const char *filename);
int cvc_commit(const char *message);
int cvc_status(void);
int cvc_log(void);
int cvc_branch(const char *branch_name);
int cvc_checkout(const char *branch_name);
int cvc_merge(const char *branch_name);

// Utilities
unsigned long hash_data(const unsigned char *data, size_t length);
void get_current_branch(char *branch_name);
int is_ignored(const char *path);

#endif // CVC_H
