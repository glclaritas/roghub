#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "fancontrol.h"
#include "osd.h"

#define FANMODE_MAX 2
#define FANMODE_MIN 0

char *TTP_FILE="/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy";

static char *fanmodes[] = { "Balanced", "Turbo", "Silent" };    // 0,1,2

int fanmode_getid(){
    int fd, fanmode = 0;
    char c;

    fd = open(TTP_FILE, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr,"Error(1) reading fanmode.\n");
    }

    if (read(fd, &c, 1) != 1) {
        fprintf(stderr,"Error(2) reading fanmode values. Invalid input.\n");
    } else fanmode = c - '0';
    close(fd);
    return (fanmode >= FANMODE_MIN && fanmode <= FANMODE_MAX) ? fanmode: 0;
}

int fanmode_setid(int targetmode){
    int fd, ret = 1;
    targetmode = (targetmode <= FANMODE_MAX && targetmode >= FANMODE_MIN) ? targetmode : 0;
    char c = '0' + targetmode;

    fd = open(TTP_FILE, O_WRONLY | O_CLOEXEC);
    if (fd == -1) {
        fprintf(stderr, "Error(1). Can't change fanmode. Permission denied.\n");
        ret = 0;
    }
    if (write(fd, &c, 1) != 1) {
        fprintf(stderr, "Error(2). Can't change fanmode. Invalid input.\n");
        ret = 0;
    }
    close(fd);
    return ret;
}

int fanmode_toggle() {
    int nextfanmode = (fanmode_getid() + 1 ) % (FANMODE_MAX + 1);
    if (!fanmode_setid(nextfanmode)){
        fprintf(stderr,"Failed to toggle fan mode.\n");
        return 0;
    }
    return 1;
}
int fanmode_display() {
    return osd_show(fanmodes[fanmode_getid()]);
}
