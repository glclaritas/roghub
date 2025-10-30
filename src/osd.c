#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int osd_show(char *msg) {
    if (getenv("WAYLAND_DISPLAY") != NULL){
        char *osdargv[] = {
            "swayosd-client",
            "--custom-message",
            msg,
            NULL
        };
        pid_t pid = fork(); 
        if (pid == -1) {
            fprintf(stderr, "Error: Failed to fork for OSD.\n");
            return 0;
        } else if (pid == 0) {
            if (execvp("swayosd-client", osdargv) < 0) {
                fprintf(stderr, "Requires SwayOSD!\n");
                _exit(1);
            }
        } else {
            waitpid(pid, NULL, 0);
        }
    }
    return 1;
}
