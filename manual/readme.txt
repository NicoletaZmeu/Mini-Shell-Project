
Hello,

BUILT-IN COMMANDS
cd  - changes directory, if no argument passed print current working directory
clr  - clears the terminal (ctrl + L)
dir - prints the contents of a given directory
environ - prints the environment variables
echo - prints to standard output
help - prints the content of the current file
pause - pauses the shell until Enter is pressed
quit - quits the shell, exits the program

LIMITATIONS:
- The shell assumes redirection symbols are separated by spaces
- No support for pipes (|)
- No advanced parsing (e.g. quotes)
- Minimal error handling for invalid commands


INTRO
simpleshell.h - file for headers and function prototypes
utility.c - file for functions
simpleshell.c - file for 'main' function where the shell logic is placed.


The shell supports:
- Execution of internal (built-in) commands
- Execution of external programs
- Input and output redirection
- Batch mode execution from a file
- Background process execution using '&'


RUNNING THE PROGRAM
Run make in the terminal and an executable file will be made in the bin dir.
You can run it as ./'PATH'/simpleshell e.g. ./Stage1/bin/simpleshell


---- INTERACTIVE MODE
enter ./'PATH'/simpleshell without additional arguments and it will trigger interactive mode
in this mode the shell is waiting for user input through stdin - keyboard


---- BATCHFILE MODE
This means that the shell will read commands and execute them from a file.
The file can be anywhere just run the appropriate PATH
e.g. ./simpleshell ./bin/batch.txt



-------------------------------------------------------
Name: Nicoleta Zmeu
Student Number: 33531

I confirm that this work is entirely my own and that
I have read and understood the DCU Academic Integrity
Policy.
-------------------------------------------------------
