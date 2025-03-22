// Nour Helmy
// 202202012

#include "my_shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(){
    char line[BUFFER_LEN]; //get command line
    char* argv[100]; //user command
    char* bin= "/bin/"; //set path at bin
    char path[1024]; //full file path
    int argc; //arg count

    while(1){
        printf("My shell>> "); //print shell prompt
        if (read_command(line) == 0 ){
            printf("\n"); break;
        } // CRTL+D pressed

        if (strcmp(line, "exit") == 0) break; //exit

        // Check if the input contains a pipe before tokenization
        if (strchr(line, '|') != NULL) {
            // Handle pipelines (no tokenization needed)
            pipeline(line);
            continue;
        }

        argc = build_args (line,argv); // build program argument - tokenizes input into individual arguments

        // Check if the input contains a variable assignment
        if (strchr(line, '=') != NULL) {
            argc = build_args(line, argv);
            handle_variable(argv, argc);
            continue; // Skip to next loop
        }
        
        int original_stdout = dup(STDOUT_FILENO);
        int original_stdin = dup(STDIN_FILENO);

        // input output redirection
        io_redirection(argv);

        set_program_path (path,bin,argv[0]); // set program full path - ex: /bin/ls

        if (strcmp(argv[0], "cd") == 0){
            change_directory(argv);
            continue;
        }

        int pid= fork(); //fork child

        if(pid==0){ //Child
            execve(path,argv,0); // if failed process is not replaced
            // then print error message
            fprintf(stderr, "Child process could not do execve\n");
        }  
        else wait(NULL); //Parent

        // Restore original stdout and stdin after command execution
        dup2(original_stdout, STDOUT_FILENO);
        dup2(original_stdin, STDIN_FILENO);
        close(original_stdout); // Close the saved stdout descriptor
        close(original_stdin); // Close the saved stdin descriptor
        
    }
    return 0;
}
