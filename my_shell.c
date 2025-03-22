#include "my_shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// Handle variables
void handle_variable(char *argv[], int argc) {
    for (int i = 0; i < argc; i++) {
        char *eq_pos = strchr(argv[i], '=');
        if (eq_pos != NULL) {
            *eq_pos = '\0'; // Split into name
            setenv(argv[i], eq_pos + 1, 1); // Assign environment variable
        }
    }
}

// function reads input using fgets and stores it in the cmd buffer
size_t read_command(char *cmd){
    if(!fgets(cmd, BUFFER_LEN, stdin)) //get command and put it in line
        return 0; //if user hits CTRL+D break
    size_t length = strlen(cmd); // get command length
    if (cmd[length - 1] == '\n') cmd[length - 1] = '\0'; // clear new line
    return strlen(cmd); // return length of the command read
}

// building arguments: function tokenizes the input command into individual arguments sepreated by spaces using
int build_args(char * cmd, char ** argv) {
    char *token; //split command into separate strings
    token = strtok(cmd," ");
    int i=0;
    while(token!=NULL){// loop for all tokens

        // Handle variable expansion
        if (token[0] == '$') {
            const char *var_value = getenv(token + 1); // Skip the '$'
            if (var_value != NULL) {
                token = strdup(var_value); // Duplicate value
            } else {
                token = ""; // Empty string if var not found
            }
        }

        argv[i]=token; // store token
        token = strtok(NULL," "); // get next token
        i++; // increment number of tokens
    }
    argv[i]=NULL; //set last value to NULL for execvp
    return i; // return number of tokens
}

// function sets full path of the command to be executed
void set_program_path (char * path, char * bin, char * prog) {
    memset (path,0,1024); // intialize buffer
    strcpy(path, bin); //copy /bin/ to file path
    strcat(path, prog); //add program to path
    for(int i=0; i<strlen(path); i++) //delete newline
        if(path[i]=='\n') path[i]='\0';
}

// char **args: arguments - a pointer to an array of strings
void change_directory(char **argv){
    // Get the home directory using getenv()
    char *home = getenv("HOME");

    // if no directory was entered
    if(argv[1] == NULL){
        printf("Missing directory! Changed to home directory.\n");

        if (chdir(home) != 0) { // change to home directory
            perror("cd");
        }
        return;
    }

    // else - change directory
    int cd = chdir(argv[1]);
    // checking if the change directory failed - return 0 for success and -1 if failed
    if (cd != 0){
        // print an error message indicating why the directory change failed
        perror("cd");
    }
}

// input and output redirection
int io_redirection(char **argv) {
    // Loop over the arguments to find the redirection operators
    for (int i = 0; argv[i] != NULL; i++) {
        // If the argument is the output operator
        // strcmp compares two strings the argument and ">", returns 0 if they are equal
        if (strcmp(argv[i], ">") == 0) {
            if (argv[i + 1] != NULL) {
                int file_descriptor = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (file_descriptor < 0) {
                    perror("open");
                    return -1;
                }
                // Remove the redirection operator and the file name from the arguments
                argv[i] = NULL;

                // Redirect standard output to the specified file
                dup2(file_descriptor, STDOUT_FILENO);
                close(file_descriptor); // Close the file descriptor
                break;
            } else {
                printf("Missing filename for output redirection!\n");
                return -1;
            }
        }
        // output redirection to append
        else if (strcmp(argv[i], ">>") == 0){
            if (argv[i + 1] != NULL) {
                // Redirect standard output to the specified file
                int file_descriptor = open(argv[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (file_descriptor < 0) {
                    perror("open");
                    return -1;
                }
                argv[i] = NULL; // Remove the redirection operator from the args

                // Perform the redirection
                dup2(file_descriptor, STDOUT_FILENO);
                close(file_descriptor); // Close the file descriptor
                break;
            }
            else {
                printf("Missing filename for output redirection!\n");
                return -1;
            }
        }
        // If the argument is the input operator
        else if (strcmp(argv[i], "<") == 0) {
            if (argv[i + 1] != NULL) {
                // Redirect standard input to the specified file
                int file_descriptor = open(argv[i + 1], O_RDONLY, 0644);
                if (file_descriptor < 0) {
                    perror("open");
                    return -1;
                }
                // Remove the redirection operator and the file name from the arguments
                argv[i] = NULL;

                // Perform the redirection
                dup2(file_descriptor, STDIN_FILENO);
                close(file_descriptor); // Close the file descriptor
                break;
            } else {
                printf("Missing filename for input redirection!\n");
                return -1;
            }
        }
    }
    return 0;
}

void pipeline(char *argv){
    // array to store up to 10 commands and a null terminator
    char *commands[11];
    int num_commands = 0;

    // split the arguments into commands using pipe
    char *command = strtok(argv, "|");
    while (command != NULL && num_commands < 10) {
        // store each command
        commands[num_commands++] = command; 
        // continue splitting by "|"
        command = strtok(NULL, "|");
    }
    commands[num_commands] = NULL;
    // array to store the pipe file descriptors - 0 read end of the pipe - 1 write end of the pipe
    int pipe_fd[2];
    // process ID 
    pid_t pid;
    // input file descriptor - 0 meaning first command's input comes from stdin
    int in_fd = 0;

    // Save original stdout and stdin
    int original_stdout = dup(STDOUT_FILENO);
    int original_stdin = dup(STDIN_FILENO);

    // loop over commands
    for (int i = 0; i < num_commands; i++){
        // array for individual arguments for each command
        char *arg[100];
        int index = 0;

        // tokenize each command
        char *t_arg = strtok(commands[i], " ");
        while (t_arg != NULL){
            // store each argument
            arg[index] = t_arg;
            index++;
            // continue splitting by space
            t_arg = strtok(NULL, " ");
        }
        // Null-terminated - ending with null pointer to signify the end of the arguments
        arg[index] = NULL;

        handle_variable(arg, num_commands);

        // Handling redirection for the last command
        // ensuring the entire pipeline processes correctly before writing the output to a file (or reading from a file).
        if (i == num_commands - 1) {
            // Handle input/output redirection for the last command
            io_redirection(arg);  
        }

        // creating a pipe for all commands except the last one
        if (i < num_commands - 1){
            pipe(pipe_fd);
        }

        // forking a new process for each command
        pid = fork();
        // if pid is 0 then we are in the child process
        if (pid == 0){
            if (in_fd != 0){
                // redirecting input ot the current pipe
                dup2(in_fd, STDIN_FILENO);
            }

            if (i < num_commands - 1){
                // redirecting output to the current pipe
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }

            // execute the command - replacing current process with new proces
            execvp(arg[0], arg);
        }
        // else if in parent process
        else{
            // wait for the child process to finish executing the current command
            wait(NULL);

            if (i < num_commands - 1){
                // close the write end of the pipe
                close(pipe_fd[1]);
            }

            // saving the read end of the pipe for the next command
            in_fd = pipe_fd[0];
        }
    }

    // Restore original stdout and stdin after command execution
    dup2(original_stdout, STDOUT_FILENO);
    dup2(original_stdin, STDIN_FILENO);
    close(original_stdout); // Close the saved stdout descriptor
    close(original_stdin); // Close the saved stdin descriptor
   
}
