#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int osd_show(char *msg) {
    char *osdargv[] = {
        "notify-send",
        "-e",
        "-t", "1000",
        "-r", "9876",
        "-a", "rogctl",
        "-h", "string:x-canonical-private-synchronous:rogctl",
        msg,
        NULL
    };

    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Error: failed to fork osd program.\n");
        return 0;
    } else if (pid == 0) {
        if ( execvp("notify-send",  osdargv) < 0) {
            fprintf(stderr, "Error: failed to launch osd program.\n");
            _exit(1);
        }
    } else {
        waitpid(pid, NULL, 0);
    }
    return 1;
}
