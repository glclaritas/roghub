#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "cpucontrol.h"
#include "fancontrol.h"
#include "osd.h"
#include "profile.h"

static int current_pfp_internal_id;

config_t silent = {
    .name = "Silent",
    .fanmode = 2,
    .turbo = 0,
};
config_t balanced = {
    .name = "Balanced",
    .fanmode = 0,
    .turbo = 0,
};
config_t turbo = {
    .name = "Turbo",
    .fanmode = 1,
    .turbo = 1,
};

char *profile_maxfreqs[3] = {
    CPU_MIN_FILE,
    CPU_MAX_FILE,
    CPU_MAX_FILE
};

config_t custom;
config_t *config_p[4] = { &silent, &balanced, &turbo, &custom};

static int has_custompfp() {
    config_t *config = cfg_update();
    if (config == NULL) return 0;

    custom = *config;
    return 1;
}

int profile_apply(int extid) {
    int intid = extid - 1;
    int pfp_count = 3;
    if (intid == 3 && has_custompfp()) pfp_count++;

    intid = intid % pfp_count;

    cpu_set_turbo(config_p[intid]->turbo);    // turbo value must be applied first
                                            // to get correct cpu max value reading below
                                            // no turbo -> cpumax = cpu base 
    if (intid < 3) {
        config_p[intid]->maxkhz = cpu_get_freq(profile_maxfreqs[intid]);
    }
    if (!fanmode_setid(config_p[intid]->fanmode)) {
        fprintf(stderr, "Failed to apply profile fanmode %d\n", config_p[intid]->fanmode);
        return 0;
    }
    if (!cpu_set_freq(config_p[intid]->maxkhz)) {
        fprintf(stderr, "Failed to apply profile max frequency %u kHz\n", config_p[intid]->maxkhz);
        return 0;
    }
    current_pfp_internal_id = intid;
    return 1;
}

static int profile_get_current() {
    int current_fanmode = fanmode_getid();
    int current_turbo = cpu_get_turbo();
    int current_max_freq = cpu_get_freq(SCALING_MAX_FILE);
    int pfp_count = has_custompfp() ? 4 : 3;

    for (int i = 0; i < 3; i++) {
        int expected_khz = cpu_get_freq(profile_maxfreqs[i]);
        if (config_p[i]->fanmode == current_fanmode &&
                config_p[i]->turbo == current_turbo &&
                expected_khz == current_max_freq) {

            if (pfp_count == 4 &&
                    custom.fanmode == current_fanmode &&
                    custom.turbo == current_turbo &&
                    custom.maxkhz == current_max_freq) {
                return 4;
            }
            return i+1;
        }
    }
    if (pfp_count == 4) {
        if (custom.fanmode == current_fanmode &&
                custom.turbo == current_turbo &&
                custom.maxkhz == current_max_freq) {
            return 4;
        }
    }
    return -1;
}

int profile_toggle() {
    int current_id = profile_get_current();
    if (current_id == -1) {
        fprintf(stderr, "Warning: Current profile does not match any predefined profiles. Switching to Balanced profile.\n");
        return profile_apply(2);
    }
    int next_id = (current_id % 4) + 1;
    return profile_apply(next_id);
}

int profile_display() {
    /* fetching and using profile_get_current() pull outdated info and causes error
     * when this function is called right after profile_apply()
     * so we directly use current_pfp_internal_id here */
    return osd_show(config_p[current_pfp_internal_id]->name); 
}
