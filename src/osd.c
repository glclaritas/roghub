#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int show_osd(char *msg) {
    if (getenv("WAYLAND_DISPLAY") != NULL){
        char *osdargv[] = {
            "swayosd-client",
            "--custom-message",
            msg,
            NULL
        };
        if (execvp("swayosd-client", osdargv) < 0) {
            fprintf(stderr,"Requires SwayOSD!\n");
            return 0;
        }
    }
    return 1;
}
