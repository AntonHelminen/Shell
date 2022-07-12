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
void parse_String(char *, char **);
void execute(char **);

// Prints out an array
void printArray(char **array) {
    printf("From printArray: \n");
    printf("\ncmd: %s", array[0]);
    printf("\narg1: %s", array[1]);
    printf("\narg2: %s\n", array[2]);
}

int main(void) {
    // Main loop
    char *arguments[LISTMAXSIZE];
    char input[BUFFER];
    int input_value;
    while (1) {
        if ((input_value = Input(input)) == 0) {
            return 0;
        }
        
        // parsing command for execution
        ////// ERRROORR
        parse_String(input, arguments);
        printf("cmd: %s", arguments[0]);
        
        printf("\narg1: %s", arguments[1]);
        char *string[80] = {"ls", "shello"};
        printArray(string);
        ////// ERRROORR
        // Executing commands
        execute(arguments);
        
        // Clearing commands and arguments for new run.
        memset(arguments, 0, sizeof(arguments));
    }
}
// Function for reading input
int Input(char *line) {
    char input[BUFFER];
    
    printf("\n> ");
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
// Parses commands into a char array
void parse_String(char *string, char **arguments) {
    int i = 0;
    char copy[BUFFER];
    strcpy(copy, string);
    char space_separator[] = " ";
    char *ptr = strtok(copy, space_separator);
    
    while (ptr != NULL) {
        arguments[i] = ptr;
        ptr = strtok(NULL, space_separator);
        i ++;
    }
}

// Execute command for arguments
void execute(char **arguments) {
    // Forking a child process
    pid_t p1;
    p1 = fork();
    printf("'%s'", arguments[0]);
    if (p1 == -1) {
        printf("Forking failed");
        return;
    }
    else if (p1 == 0) {
        if ((execvp(arguments[0], arguments)) < 0) {
            printf("Command execution failed\n");
            exit(0);
        }
    }
    else {
        wait(NULL);
        return;
    }
}