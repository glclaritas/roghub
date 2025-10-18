#include <stdio.h>

#include "fancontrol.h"
#include "osd.h"

#define FANMODE_MAX 2
#define FANMODE_MIN 0

char *TTP_FILE="/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy";

static char *fanmodes[] = { "Balanced", "Turbo", "Silent" };    // 0,1,2

int fanmode_getid(){
    FILE *fptr = fopen(TTP_FILE, "r");
    if (!fptr) {
        fprintf(stderr,"Can't read %s. Permission denied!\n",TTP_FILE);
        return 0;   // balanced fanmode value
    }
    int fanmode;
    if (fscanf(fptr,"%1d",&fanmode) != 1) fanmode = 0;
    fclose(fptr);
    return (fanmode >= FANMODE_MIN && fanmode <= FANMODE_MAX) ? fanmode: 0;
}

int fanmode_setid(int targetmode){
    if (targetmode < FANMODE_MIN || targetmode > FANMODE_MAX) {
        fprintf(stderr,"Invalid fanmode!\n");
        return 0;
    }
    FILE *fptr = fopen(TTP_FILE, "w");
    if (!fptr) {
        fprintf(stderr,"Can't write to %s. Permission denied!\n",TTP_FILE);
        return 0;
    }
    if (fprintf(fptr,"%d",targetmode) < 0) {
        fprintf(stderr,"Can't write to %s. Permission denied!\n",TTP_FILE);
        fclose(fptr);
        return 0;
    }
    fclose(fptr);
    return 1;
}

int fanmode_toggle() {
    int nextfanmode = (fanmode_getid() + 1 ) % (FANMODE_MAX + 1);
    if (!fanmode_setid(nextfanmode)){
        fprintf(stderr,"Failed to toggle fan mode.\n");
        return 0;
    }
    return 1;
}
int show_fanmode() {
    return show_osd(fanmodes[fanmode_getid()]);
}
