#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h> // Needed for MAX_INPUT
#include <stdbool.h>

#include "sysinfo.h"

#define MAX_ARGS 64

const char VERSION[] = "0.1";

void printPS1(bool interrupt);

void handle_sigint(int sig) {
    printPS1(true);
}

void printPS1(bool interrupt) {
    // change this to an if, that checks
    // a configuration file and cycles through
    // normal = final instance/current DIR
    // extended = ~/dir
    // exact = full path, from root
    char *username = return_uname();
    char *hostname = return_hname();
    char *pwd = return_pwd();

    if (!interrupt)
        printf("[%s@%s %s]$ ", username, hostname, pwd);
    else
        printf("\n[%s@%s %s]$ ", username, hostname, pwd);

    fflush(stdout);
}

void parse_input(char *input, char **args, char **out_file) {
    *out_file = NULL;
    int arg_count = 0;
    char *token = strtok(input, " ");

    while (token != NULL && arg_count < MAX_ARGS - 1) {
        if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                *out_file = token;
            }
            break;
        }
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }

    args[arg_count] = NULL;
}

void shell_loop() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char *out_file = NULL;

    char *username = return_uname();
    char *hostname = return_hname();
    char *pwd = return_pwd();

    signal(SIGINT, handle_sigint);

    do {
        printPS1(false);

        if (!fgets(input, MAX_INPUT, stdin)) break;
        input[strcspn(input, "\n")] = 0;

        parse_input(input, args, &out_file);

        if (args[0] == NULL) continue;
        if (strcmp(args[0], "exit") == 0) break;

        pid_t pid = fork();
        if (pid == 0) {
            // Reset SIGINT to default behavior after use on Child Process
            signal(SIGINT, SIG_DFL);

            // Handle output redirection
            if (out_file) {
                int fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("open");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            execvp(args[0], args);
            perror("execvp");
            exit(1);
        } else if (pid > 0) {
            wait(NULL);
        } else {
            perror("fork");
        }
    } while (1);
}

int main() {
    shell_loop();
    return 0;
}
