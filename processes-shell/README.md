# Wish: A Simple Unix Shell

Welcome to Wish, the Wisconsin Shell. This project is a simple Unix shell that provides a command-line interface for executing commands interactively or from batch files. It supports basic shell functionalities such as running commands, handling built-in commands, redirecting output, and executing commands in parallel.

## Objectives

- Familiarize with the Linux programming environment.
- Learn how to deal with processes.
- Gain exposure to the necessary functionality in shells.

## Features

- **Interactive Mode**: Run the shell interactively, where you can type commands and see the results immediately.
- **Batch Mode**: Execute commands from a batch file.
- **Built-in Commands**: Implemented `exit`, `cd`, and `path` commands.
- **Output Redirection**: Redirect the output of commands to files.
- **Parallel Command Execution**: Run multiple commands in parallel using the `&` operator.

### Commands Modes
The shell can be invoked in two modes:
- **Interactive Mode**: Accepts commands from the user in real-time.
 ```sh
  wish>
```
- **Batch Mode**: Reads and executes commands from a specified file.
```sh
prompt> ./wish batch.txt
```
### Command Execution
wish creates a child process for each command:

Parses the command line input.
Uses the fork() system call to create a new process.
Uses execv() to execute the command in the new process.
Waits for the command to complete using wait() or waitpid().

### Built-in Commands
The shell includes built-in commands that are handled internally:

- **exit**: Exits the shell.
- **cd**: Changes the current working directory.
- **path**: Sets the search path for executables.

### Path Management
The shell uses a search path to locate executables:

The initial search path contains /bin.
The path command allows users to modify the search path.
Uses the access() system call to check if a file is executable.

### Output Redirection
The shell supports redirecting the output of a command to a file:

Uses the > character for redirection.
Redirects both standard output and standard error to the specified file.
```sh
wish> ls -la /tmp > output.txt
```

### Parallel Command Execution
The shell allows executing multiple commands in parallel using the & operator:

Executes each command in parallel.
Waits for all parallel commands to complete before returning control to the user.
```sh
wish> cmd1 & cmd2 args1 args2 & cmd3 args1
```

## Usage
To start the shell in interactive mode:

```sh
prompt> ./wish
wish>
```
To run the shell with a batch file:
```sh
prompt> ./wish batch.txt
```
### Example Commands
```sh
Copy code
wish> ls -la
wish> cd /usr/local
wish> path /bin /usr/bin
wish> ls -la /tmp > output.txt
wish> cmd1 & cmd2 args & cmd3
wish> exit
```
### Compilation

To compile the code, use the following command:

```sh
gcc -o -Wall -Werror wish wish.c
```

