/*
Simpleshell.h is the file for function prototypes, headers and constants used in the shell
Apologies for the placements of some comments, I tried to keep the code as clean as possible
*/
 
#ifndef SIMPLE_SHELL_H
#define SIMPLE_SHELL_H

// set constants for buffer sizes and define separators for parsing
#define MAX_BUFFER 1024
#define MAX_ARGS 64
#define SEPARATORS " \t\n"

// function prototypes for user input handling
void print_prompt(char *buffer, int size);
int read_line(char *buffer, int size);
int parse_line(char *buffer, char *args[]);

// function prototypes for running shell functionality
void builtin_echo(char *args[]);
void builtin_clr(char *args[]);
void builtin_cd(char *args[]);
void builtin_dir(char *args[]);
void builtin_environ(char *args[]);
void builtin_help(char *args[]);
void builtin_pause(char *args[]);
void builtin_quit(char *args[]);
void external_command(char *args[], char *shell_path);
void execute_command(char *args[], char *fullpath);
int handle_input_redirection(char *args[]);
int handle_output_redirection(char *args[]);
void trim_redirection(char *args[]);
void restore_redirection(int in_fd, int out_fd);

#endif

/*
-------------------------------------------------------
Name: Nicoleta Zmeu
Student Number: 33531

I confirm that this work is entirely my own and that
I have read and understood the DCU Academic Integrity
Policy.
-------------------------------------------------------
*/