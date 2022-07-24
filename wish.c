#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#include<sys/types.h>
#include<sys/wait.h>

#define BUFFER 80
#define LISTMAXSIZE 1000

// Used functions
int Input(char *);
int parse_String(char *, char **);
void execute(char **, int);

// Main
int main(int argc, char **argv) {
    // Error messages
    char error_message_arguments[50] = "Too many given files/arguments: max 1\n";
    char error_message_file_open[50] = "File opening failed\n";

    char *arguments[LISTMAXSIZE];
    char input[BUFFER];
    int input_value;
    int size;
    // Check if too many arguments
    if (argc > 2) {
        write(STDERR_FILENO, error_message_arguments, strlen(error_message_arguments));
        exit(1);
    }
    // Check if commands are read from a file rather than free-use commands.
    if (argc == 2) {
        // Executing commands line after line
        FILE *file;
        char *input_line = NULL;
        size_t line_buf_size = 0;
        ssize_t line_size;

        if ((file = fopen(argv[1], "r"))==NULL) {
            write(STDERR_FILENO, error_message_file_open, strlen(error_message_file_open));
            exit(1);
        }
        line_size = getline(&input_line, &line_buf_size, file);
        while (line_size >= 0) {
            // parsing command for execution
            input_line[strlen(input_line)-1] = '\0';
            size = parse_String(input_line, arguments);

            // Executing commands
            execute(arguments, size);

            // Clearing commands and arguments for new run.
            memset(arguments, 0, sizeof(arguments));

            // Get next line
            line_size = getline(&input_line, &line_buf_size, file);

        }

        // Free allocated memory
        free(input_line);
        input_line == NULL;
        fclose(file);
        // Program exits afterwards
        return 0;
    }

    // Main loop for free-use mode
    while (1) {
        if ((input_value = Input(input)) == 0) {
            break;
        }
        // parsing command for execution
        size = parse_String(input, arguments);

        // Executing commands
        execute(arguments, size);

        // Clearing commands and arguments for new run.
        memset(arguments, 0, sizeof(arguments));

    }
    return 0;
}
// Function for reading input (not file)
int Input(char *line) {
    char input[BUFFER];
    
    printf("\nwish> ");
    scanf(" %[^\n]s", input);
    getchar();

    if (strlen(input) == 0) {
        return 0;
    }
    else {
        strcpy(line, input);
        return 1;
    }
}
// Parses commands into a given char array
int parse_String(char *string, char **arguments) {
    int i = 0;
    int size = 0;

    while (1) {
        arguments[i] = strsep(&string, " ");
        if (arguments[i] == NULL) {
            break;
        }
        size++;
        i++;
    }
    return size;
}

// Execute command for arguments
void execute(char **arguments, int size) {
    // error messages
    char error_message_exit[50] = "Too many arguments\n";
    char error_message_chdir_1[50] = "Wrong argument count: should be 1\n";
    char error_message_chdir_2[50] = "Failed to switch directory\n";
    char error_message_fork[50] = "Forking failed\n";
    char error_message_exec[50] = "Execution failed\n";

    // Own commands
    int own = 2, i, arg = 0;
    char *ownCommands[own];
    ownCommands[0] = "exit";
    ownCommands[1] = "cd";

    // Check if command being run is own command:
    for (i = 0; i < own; i++) {
        if (strcmp(arguments[0], ownCommands[i]) == 0) {
            arg = i + 1;
            break;
        }
    }
    // Execute own command
    if (arg > 0) {
        switch(arg) {
        case 1:
            // exit if only no arguments
            if (size == 1) {
                exit(0);
            }
            else {
                write(STDERR_FILENO, error_message_exit, strlen(error_message_exit));
            }
            return;
        case 2:
            // chdir if only one argument
            if (size == 2) {
                int success;
                success = chdir(arguments[1]);
                if (success == -1) {
                    write(STDERR_FILENO, error_message_chdir_2, strlen(error_message_chdir_2));
                }
            }
            else {
                write(STDERR_FILENO, error_message_chdir_1, strlen(error_message_chdir_1));
            }
            return;
        default:
            break;
        }
        return;
    }

    // If command was not own, it is executed normally below

    // Forking and executing a child process
    pid_t p1;
    p1 = fork();
    if (p1 == -1) {
        write(STDERR_FILENO, error_message_fork, strlen(error_message_fork));
        return;
    }
    else if (p1 == 0) {
        if ((execvp(arguments[0], arguments)) < 0) {
            write(STDERR_FILENO, error_message_exec, strlen(error_message_exec));
            exit(0);
        }
    }
    else {
        wait(NULL);
        return;
    }
}