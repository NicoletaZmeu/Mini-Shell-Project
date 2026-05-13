
/*
Simpleshell.c is the file for main loop logic of the shell,
it handles user input and calls the corresponding functions.
it handles redirection and external commands
It also handles batch mode if a file is provided as an argument.

Initally the logic involved a big if else statement but in stage 2 submission i made an array of function pointers

Comments are placed in clusters above or under functioin names so it doesnt break the flow of the code
Apologies for the placements of some comments, I tried to keep the code as clean as possible
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "simpleshell.h" // include the header and function prototypes

int main(int argc, char *argv[])
{

    char fullpath[1024]; // set the "shell" environment variable to the full path of the shell executable

    if (realpath(argv[0], fullpath) != NULL) // check if realpath is successful
    {
        setenv("shell", fullpath, 1);
    }

    // set up buffers for user input and parsed arguments
    char buffer[MAX_BUFFER];
    char *args[MAX_ARGS];

    /* -------- Interactive mode -------- */

    if (argc < 2) // check if no batch file is provided as an argument -> enter interactive mode
    {
        while (1)
        {
            // save original input & output
            int saved_in = dup(STDIN_FILENO);
            int saved_out = dup(STDOUT_FILENO);
            // the functions used in this file are explained in utility.c

            print_prompt(buffer, MAX_BUFFER); // print the prompt
            if (!read_line(buffer, MAX_BUFFER))
                break;                // read user input, if no input break the loop and exit
            parse_line(buffer, args); // parse the input line into arguments

            // this checks if i/o redirection is present
            // if present the functions handle the redirection
            handle_input_redirection(args);
            handle_output_redirection(args);

            // parses the args again to handle redirection
            trim_redirection(args);
            // execute the commands from the user
            execute_command(args, fullpath);
            // bug fix: ensures all buffered output is written immediately
            fflush(NULL);
            // restore to the original input output
            restore_redirection(saved_in, saved_out);
            // ready for new iteration
        }
    }
    
    
    /* -------- Batch mode (Stage 1 requirement) --------*/

    else
    {
        // if batchfile is provided, open the file for reading 'r', file is argv[1]
        FILE *batchfile = fopen(argv[1], "r");

        // if file cannot be opened print an error and exit the program
        if (!batchfile)
        {
            perror("Batch file");
            exit(1);
        }

        while (fgets(buffer, MAX_BUFFER, batchfile)) // read each line one by one until the end of the file
        {
            // save original input & output
            int saved_in = dup(STDIN_FILENO);
            int saved_out = dup(STDOUT_FILENO);

            buffer[strcspn(buffer, "\n")] = '\0'; // remove the newline character from the end of the line

            parse_line(buffer, args); // parse the line into arguments

            // this checks if i/o redirection is present
            // if present the functions handle the redirection
            handle_input_redirection(args);
            handle_output_redirection(args);

            // parses the args again to handle redirection
            trim_redirection(args);
            // execute the commands from the user
            execute_command(args, fullpath);
            // restore to the original input output
            restore_redirection(saved_in, saved_out);
            // ready for new iteration
        }

        fclose(batchfile); // once the file end is reached close the file
    }

    return 0;
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