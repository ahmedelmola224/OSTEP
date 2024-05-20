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



### Compilation

To compile the code, use the following command:

```sh
gcc -o wish wish.c
