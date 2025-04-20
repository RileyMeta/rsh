#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER 256

char *return_pwd() {
    static char result[1024];
    char cwd[1024];
    const char *home = getenv("HOME");

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if (strncmp(cwd, home, strlen(home)) == 0) {
            snprintf(result, sizeof(result), "~%s", cwd + strlen(home));
        } else {
            snprintf(result, sizeof(result), "%s", cwd);
        }
        return result;
    } else {
        perror("getcwd() error");
        return NULL;
    }
}

char *return_uname() {
    char *uname = getenv("USER");

    if (uname != NULL) {
        return uname;
    } else {
        perror("getusername");
        return NULL;
    }
}

char *return_hname() {
    char *hostname = malloc(BUFFER);

    if (gethostname(hostname, BUFFER) != 0) {
        perror("gethostname");
        return NULL;
    }
    return hostname;
}
