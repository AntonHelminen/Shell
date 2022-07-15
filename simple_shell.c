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

// Main
int main(void) {
    // Main loop
    char *arguments[LISTMAXSIZE];
    char input[BUFFER];
    int input_value;
    while (1) {
        alarm(15);
        if ((input_value = Input(input)) == 0) {
            break;
        }
        alarm(0);
        // parsing command for execution
        parse_String(input, arguments);

        // Executing commands
        execute(arguments);
        
        // Clearing commands and arguments for new run.
        memset(arguments, 0, sizeof(arguments));

    }
    return 0;
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
// Parses commands into a given char array
void parse_String(char *string, char **arguments) {
    int i = 0;
    while (1) {
        arguments[i] = strsep(&string, " ");
        if (arguments[i] == NULL) {
            break;
        }
        i++;
    }
}

// Execute command for arguments
void execute(char **arguments) {
    // Forking a child process
    pid_t p1;
    p1 = fork();
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
