#ifndef MY_SHELL
#define MY_SHELL

#include <stddef.h>

#define BUFFER_LEN 1024

// Functions
void handle_variable(char *argv[], int argc);
size_t read_command(char *cmd);
int build_args(char *cmd, char **argv);
void set_program_path(char *path, char *bin, char *prog);
void change_directory(char **argv);
int io_redirection(char **argv);
void pipeline(char *argv);

#endif 
