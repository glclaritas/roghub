#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "cpucontrol.h"
#include "fancontrol.h"
#include "osd.h"
#include "profile.h"

static int id = 1;

profile_t silent = {
    .name = "Silent",
    .fanmode = 2,
    .turbo = 0,
};
profile_t balanced = {
    .name = "Balanced",
    .fanmode = 0,
    .turbo = 0,
};
profile_t turbo = {
    .name = "Turbo",
    .fanmode = 1,
    .turbo = 1,
};

char *profile_maxfreqs[3] = {
    CPU_MIN_FILE,
    CPU_MAX_FILE,
    CPU_MAX_FILE
};

profile_t custom;
profile_t *profile_p[4] = { &silent, &balanced, &turbo, &custom};

static int has_custompfp() {
    char *endptr, *cfgval;
    cfgval = cfg_read("name");
    if (cfgval == NULL) {
        printf("name is %s\n",cfgval);
        return 0;
    }
    snprintf(custom.name, sizeof(custom.name), "%s", cfgval);

    cfgval = cfg_read("fanmode");
    if (cfgval == NULL) return 0;                       // [silent, balanced, turbo]
    int ttp = strtoul(cfgval, &endptr, 10);             // config reading -> thermal_throttle_policy mapping
    if (cfgval == endptr) return 0;                     // 1 2 3  -> 2 0 1
    if (ttp > 3 || ttp < 1) return 0;                   // (x+1)%3
    ttp = (ttp + 1 ) % 3;
    custom.fanmode = ttp;
    
    cfgval = cfg_read("turbo");
    if (cfgval == NULL) return 0;
    custom.turbo = strtoul(cfgval, &endptr, 10);
    if (cfgval == endptr) return 0;

    cfgval = cfg_read("maxghz");
    double ghz = strtod(cfgval, &endptr);
    if (cfgval == endptr) return 0;
    custom.max_ghz = (unsigned int)(ghz * 1e6);
    return 1;
}

int profile_apply(int extid) {
    int intid = extid - 1;
    int pfp_count = 3;
    if (intid == 3 && has_custompfp()) pfp_count++;

    id = intid % pfp_count;

    cpu_set_turbo(profile_p[id]->turbo);    // turbo value must be applied first
                                            // to get correct cpu max value reading below
                                            // no turbo -> cpumax = cpu base 
    if (id < 3) {
        profile_p[id]->max_ghz = cpu_get_freq(profile_maxfreqs[id]);
    }
    fanmode_setid(profile_p[id]->fanmode);
    cpu_set_freq(profile_p[id]->max_ghz);

    char lastpfp_path[PATH_MAX];
    if (getenv("XDG_RUNTIME_DIR") != NULL) {
        snprintf(lastpfp_path, sizeof(lastpfp_path), "%s/.roghubpfp", getenv("XDG_RUNTIME_DIR"));
    }else if (getenv("UID") != NULL) {
        snprintf(lastpfp_path, sizeof(lastpfp_path), "/tmp/%s.roghubpfp", getenv("UID"));
    }else snprintf(lastpfp_path, sizeof(lastpfp_path), "/tmp/roghubpfp");

    FILE *fptr = fopen(lastpfp_path, "w");
    if (fptr == NULL)
        return 0;
    if (fptr) {
        fprintf(fptr, "%d\n",id);
        fclose(fptr);
    }
    return 1;
}

void profile_toggle(void) {
    int tid=1;              // defaults to internal id balanced
    char lastpfp_path[PATH_MAX];
    if (getenv("XDG_RUNTIME_DIR") != NULL) {
        snprintf(lastpfp_path, sizeof(lastpfp_path), "%s/.roghubpfp", getenv("XDG_RUNTIME_DIR"));
    }else if (getenv("UID") != NULL) {
        snprintf(lastpfp_path, sizeof(lastpfp_path), "/tmp/%s.roghubpfp", getenv("UID"));
    }else snprintf(lastpfp_path, sizeof(lastpfp_path), "/tmp/roghubpfp");
    FILE *fptr = fopen(lastpfp_path, "r");
    if (fptr) {
        if (fscanf(fptr,"%d",&tid) != 1)
            fclose(fptr);
    }
    tid = ( tid +1 ) % 4;   // allow sending the max value of 3 even if custom profile doesn't exist
                            // apply_profile will handle
    profile_apply(tid);
}

int profile_display() {
    return osd_show(profile_p[id]->name); 
}
