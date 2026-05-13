/*

Utility.c contains the implementation of the functions declared in simpleshell.h,
which are used in simpleshell.c to execute the shell functionality

Comments are placed in clusters above or under functioin names so it doesnt break the flow of the code
Apologies for the placements of some comments, I tried to keep the code as clean as possible
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "simpleshell.h"
#include <fcntl.h>

// create a struct to represent a command, with a name and a function pointer to the corresponding function
typedef struct
{
    char *name;
    void (*function)(char **);
} command;

// create an array of commands, it is easier to maintain and read than a big if else statement
command commands[] = {
    {"clr", builtin_clr},
    {"quit", builtin_quit},
    {"cd", builtin_cd},
    {"dir", builtin_dir},
    {"environ", builtin_environ},
    {"echo", builtin_echo},
    {"help", builtin_help},
    {"pause", builtin_pause},
    {NULL, NULL} // sentinel value to mark the end
};

// declare the environ variable to use it
extern char **environ;

/*----prompt-----*/
void print_prompt(char *buffer, int size)
{
    if (getcwd(buffer, size) != NULL) // get the current working directory and put it in the buffer, check if successful
    {
        printf("%s>", buffer);
        fflush(stdout); // Ensure the prompt is printed immediately before waiting for user input
    }
    else
    {
        perror("getcwd"); // if getcwd fails, print an error message
    }
}

/*-----read line-----*/
int read_line(char *buffer, int size)
{
    // read line from standard input / keyboard
    // check if input is received, otherwise (NULL) return 0
    if (fgets(buffer, size, stdin) == NULL)
    {
        return 0;
    }

    // else swap the newline with null terminator and return 1 - input successfully read
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

/*-----parse line-----*/
int parse_line(char *buffer, char *args[])
{
    int arg_count = 0;

    // use strtok to split the input line into tokens based on the defined separators
    // strtok returns a pointer to the next token, or NULL if there are no more tokens
    char *token = strtok(buffer, SEPARATORS);

    while (token != NULL && arg_count < MAX_ARGS - 1)
    // add the token to args using arg_count as the index, then increment arg_count
    // then extract the next token by calling strtok
    {
        args[arg_count++] = token;
        token = strtok(NULL, SEPARATORS);
    }

    // add a NULLL pointer at the end of the array
    args[arg_count] = NULL;
    return arg_count;
}

/*------echo-------*/
void builtin_echo(char *args[])
{
    // loop through the parsed arguments starting at 1
    // print each argument and check if it is NOT the last one and print space
    // if it is the last one, print a newline instead

    for (int i = 1; args[i] != NULL; i++)
    {
        printf("%s", args[i]);
        if (args[i + 1] != NULL)
        {
            printf(" ");
        }
    }
    printf("\n");
}

/*-------clear--------*/
void builtin_clr(char *args[])
{
    (void)args;
    // simpleshell.c checks if command is clear and system calls to clear the terminal
    system("clear");
}

/*-----change directory------*/
void builtin_cd(char *args[])
{

    // buffer to hold the path
    // check if a second argument is not provided, just 'cd'
    char buffcwd[1024];
    if (args[1] == NULL)
    {

        // if no path provided, we'll print the current dir
        // check if function is successful, if so print the current directory
        if (getcwd(buffcwd, sizeof(buffcwd)) != NULL)
        {
            printf("%s\n", buffcwd);
        }
        else
        // error handling if getcwd fails, print an error message
        {
            perror("getcwd");
        }
        return;
    }

    // if path provided as the second argument args[1], we will change directory
    // chdir returns 0 if successful
    // check if unsuccessful
    if (chdir(args[1]) != 0)
    {
        perror("cd");
        return; // leave the function if chdir fails
    }

    // if chdir is successful, print the new current directory
    // update the PWD environment variable to new current directory
    if (getcwd(buffcwd, sizeof(buffcwd)) != NULL)
    {
        setenv("PWD", buffcwd, 1);
    }
}

/*--------dir--------*/
void builtin_dir(char *args[])
{
    char command[1024]; // buffer to create command

    if (args[1])
    {                               // checks if second argument (path) is provided
        strcpy(command, "ls -al "); // add system command
        strcat(command, args[1]);   // add the path
    }
    else
    {
        strcpy(command, "ls -al ."); // if no path just list current directory
    }
    system(command); // execute the command
}

/*------environ-------*/
void builtin_environ(char *args[])
{
    (void)args;
    for (int i = 0; environ[i] != NULL; i++) // loop through the variables until we reach the end
        printf("%s\n", environ[i]);          // print each environment variable
}

/*------help-------*/
void builtin_help(char *args[])
{
    (void)args;
    system("more ./manual/readme.txt"); // system function to call more-command to display readme.txt
}

/*------pause------*/
void builtin_pause(char *args[])
{
    (void)args;
    int c; // variable to hold the character input from the user

    printf("Press Enter to continue..."); // prompt the user to press Enter
    fflush(stdout);                       // make sure the prompt is printed before waiting for input
    while ((c = getchar()) != '\n' && c != EOF)
        ; // wait for the user to press Enter or end of file, ignore any other input
}

/*-------quit--------*/
void builtin_quit(char *args[])
{
    (void)args;
    printf("Exiting simpleshell...\n"); // print a message before exiting
    exit(0);                            // exit the program with a success status code
}

/*-----------external commands------------*/
void external_command(char *args[], char *fullpath)
{
    // initialise variables
    pid_t pid;
    int rc;
    int dont_wait = 0;

    // search for the & background execution symbol
    for (int i = 0; args[i] != NULL; i++)
    {
        if (strcmp(args[i], "&") == 0)
        {
            dont_wait = 1;  // if found mark as background execution, dont wait for it
            args[i] = NULL; // remove the symbol from the args, to separate the command
            break;
        }
    }

    pid = fork(); // create a new process

    switch (pid)
    {
    // error handler
    case -1:
        perror("fork: no such command");
        return;
    // child process
    case 0:
        setenv("parent", fullpath, 1);                  // set variable path of the child to match the parent
        execvp(args[0], args);                          // execute the command
        perror("fork, unsuccessful command execution"); // the program should not reach this line if exec is successful
        exit(1);                                        // this means it failed and error is pushed
    // parent process
    default:
        if (!dont_wait)           // checks if it is not a background job, only true when dont wait = 0 -> foregroung, negates to 1 -> true
            waitpid(pid, &rc, 0); // waits for the child process
    }
    return;
}

/*----------execute command-----------*/
void execute_command(char *args[], char *fullpath)
{
    // check if any input was provided, if not return to print prompt again
    if (args[0] == NULL)
        return;

    // flags if the command was found in in-built commands
    int found = 0;

    for (int i = 0; commands[i].name != NULL; i++) // iterate through the commands array
    {
        if (!strcmp(args[0], commands[i].name)) // check if the command matches any of the built-in commands
        {
            commands[i].function(args); // call the corresponding function
            found = 1;                  // mark as found
            break;                      // break the loop to avoid calling external_command
        }
    }

    if (!found) // if the command was not found run external command handler
    {
        external_command(args, fullpath); // if the command does not match any built-in command, call the external_command function
    }
    return;
}

/*----------check for input file----------*/
int handle_input_redirection(char *args[])
{
    for (int i = 0; args[i] != NULL; i++)
    {
        // check for input redirection
        if (strcmp(args[i], "<") == 0)
        {
            if (args[i + 1] == NULL) // check if a input file name is provided
            {
                perror("Missing input file\n");
                return 0; // return 0 to indicate an error
            }
            int in_file = open(args[i + 1], O_RDONLY); // open the file O_RDONLY means read only
            if (in_file < 0)                           // check if opened successfully
            {
                perror("Input redirection failed\n");
                return 0;
            }
            int opened = dup2(in_file, STDIN_FILENO); // redirect standard input to the file
            if (opened < 0)                           // check if dup2 was successful
            {
                perror("Input redirection failed\n");
                close(in_file);
                return 0;
            }
            close(in_file); // close the file, the standard input is still redirected to it
        }
    }
    return 1; // return 1 to indicate success
}

/*----------check for input file----------*/
int handle_output_redirection(char *args[])
{

    for (int i = 0; args[i] != NULL; i++)
    {
        int out_file;

        if (strcmp(args[i], ">") == 0) // search for the output file symbol
        {
            if (args[i + 1] == NULL) // check if output file is provided
            {
                perror("Missing output file\n");
                return 0;
            }
            // system call to open the file, write only, create if doesnt exist, truncate/ clear contents, and set permissions to 0644
            out_file = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        else if (strcmp(args[i], ">>") == 0) // check for append to output file symbol
        {
            if (args[i + 1] == NULL) // check if an output file name was provided
            {
                perror("Missing output file\n");
                return 0;
            }

            // system ccall to open file, write only, create if it doesnt exist, append, and set permissions to 0644
            out_file = open(args[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
        else // if no output file found in the arguments then skip iterration completely
        {
            continue;
        }

        // if symbol found check if opened successfully
        if (out_file < 0)
        {
            perror("Output redirection failed \n"); // error handling if opened unsuccessfully
            return 0;
        }
        // replace file descriptor 1 (stdout) with out_file
        int opened = dup2(out_file, STDOUT_FILENO);
        // errror handler
        if (opened < 0)
        {
            perror("Output redirection failed \n");
            close(out_file);
            return 0;
        }
        // safely close the file, output will still be redirected by the system
        close(out_file);
    }
    return 1;
}

/*-----------parse line after redirection------------*/
void trim_redirection(char *args[])
{
    // we assume the command is in the format: command arg1 arg2 ... < inputfile > outputfile
    // hence we can safelly cut short the command to the last arg
    for (int i = 0; args[i] != NULL; i++)
    {
        // search for the symbols
        if (!strcmp(args[i], "<") || !strcmp(args[i], ">") || !strcmp(args[i], ">>"))
        {
            args[i] = NULL; // set the redirection symbol to NULL
            return;         // return because now we have the command separated
        }
    }
}

/*-----------restore original stdin & stdout-----------*/
void restore_redirection(int saved_stdin, int saved_stdout)
{
    // function to restore original standard input and output using the saved file descriptors
    // these are saved in the main function
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    // close the files
    close(saved_stdin);
    close(saved_stdout);
}

/*
-------------------------------------------------------
Name: Nicoleta Zmeu
Student Number: 33531

I confirm that this work is entirely my own and that
I have read and understood the DCU Academic Integrity
Policy.
-------------------------------------------------------
*/