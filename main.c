#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termio.h>

#define TKN_DELIM " \t\n\r"
#define TKN_BUFF_SIZE 64


int cshExit(char **args) {
    return 0;
}


int cshExecute(char **args) {
    pid_t cpid;
    int status;

    if (strcmp(args[0], "exit") == 0) {
        return cshExit(args);
    }
    if (strcmp(args[0], "cd") == 0) {
        chdir(args[1]);
        return 1;
    }

    cpid = fork();

    if (cpid == 0) {
        if (execvp(args[0], args) < 0){
            printf("csh: command not found: %s\n", args[0]);
            exit(EXIT_FAILURE);
        }
    } else if (cpid < 0)
        printf("Error forking\n");
    else {
        waitpid(cpid, &status, WUNTRACED);
    }
    return 1;
}


char** splitLine(char* line) {
    int buffSize  = 64, position = 0;
    char **tokens = malloc(sizeof(char *) * buffSize);
    char* token;

    if (!tokens) {
        fprintf(stderr, "Allocation error!\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TKN_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffSize) {
            buffSize += TKN_BUFF_SIZE;
            tokens = realloc(tokens, sizeof(char *) * buffSize);
            if (!tokens) {
                fprintf(stderr, "Allocation error!\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TKN_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}


char* readLine() {
    int buffSize = 1024;
    char* buffer = malloc(sizeof(char) * buffSize);

    int c;
    int position = 0;

    if (!buffer) {
        fprintf(stderr, "Allocation error!\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= buffSize) {
            buffSize += 1024;
            buffer = realloc(buffer, buffSize);

            if (!buffer) {
                fprintf(stderr, "Allocation error!\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}


void loop() {
    char *line, *cwd;
    char **args;

    int status = 1;
    do {
        cwd = getcwd(NULL, 0);
        printf("%s > ", cwd);
        line = readLine();
        args = splitLine(line);
        status = cshExecute(args);
        free(line);
        free(args);
        free(cwd);
    } while (status);
}


int main()
{
    loop();
    return 0;
}
