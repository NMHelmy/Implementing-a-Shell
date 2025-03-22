# My Shell
A simple yet powerful custom shell implementation in C. This shell supports basic command execution, environment variable handling, I/O redirection, and pipeline functionality.

## Features
* **Command Execution:** Execute standard shell commands.
* **Environment Variables:** Handle environment variables (e.g., VAR=value).
* **I/O Redirection:** Supports input (<), output (>), and append (>>) redirection.
* **Pipelines:** Execute multiple commands in a pipeline (e.g., cmd1 | cmd2 | cmd3).
* **Change Directory:** Built-in cd command to change directories.
* **Error Handling:** Robust error handling for invalid commands, directories, or redirections.

## Prerequisites
* GCC: To compile the C code.
* Linux/Unix Environment: The shell is designed to run on Unix-like systems.

## Usage
Clone the Repository:
```
git clone https://github.com/your-username/my-shell.git
cd my-shell
```
Compile the Shell:
```
gcc -o my_shell main.c my_shell.c
```
Run the Shell:
```
./my_shell
```

---

### Example Commands:
Execute a command:
```
ls -l
```
Set an environment variable:
```
MY_VAR=hello
```
Use I/O redirection:
```
echo "Hello, World!" > output.txt
cat < input.txt
```
Use pipelines:
```
ls -l | grep ".txt"
```
Change directory:
```
cd /path/to/directory
```

---

## Code Structure
### Files
* main.c: The main entry point for the shell.
    * Handles user input, command execution, and pipeline detection.
    * Manages the shell loop and exit conditions.
* my_shell.c: Contains the core functionality of the shell.
    * handle_variable: Handles environment variable assignments.
    * read_command: Reads user input.
    * build_args: Tokenizes the input into arguments.
    * set_program_path: Sets the full path for command execution.
    * change_directory: Implements the cd command.
    * io_redirection: Handles input/output redirection.
    * pipeline: Executes commands in a pipeline.

* my_shell.h: Header file containing function declarations and constants.

## Key Functions
* handle_variable: Processes environment variable assignments (e.g., VAR=value).
* read_command: Reads and processes user input.
* build_args: Splits the input command into arguments.
* set_program_path: Constructs the full path for command execution.
* change_directory: Implements the cd command.
* io_redirection: Handles input/output redirection.
* pipeline: Executes commands in a pipeline.

---

## Example Workflow
Start the shell:
```
./my_shell
```
Run commands:
```
MY_VAR=world
echo $MY_VAR > output.txt
cat output.txt
ls -l | grep "output.txt"
cd /home/user
```
Exit the shell:
```
exit
```
