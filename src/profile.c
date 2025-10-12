#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpucontrol.h"
#include "fancontrol.h"
#include "osd.h"
#include "profile.h"

#define LASTPFP_FILE "/tmp/.rogctlpfp"
char cpath[PATH_MAX];

static int readcfg(char *ckey, char *cval);
static int id = 1;

int nprofile = 3;
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
    char tmp[64];
    char *endptr;
    if (!readcfg("name",custom.name)) return 0;

    if (!readcfg("fanmode",tmp))return 0;               // [ silent, balanced, turbo ]
    int ttp = strtoul(tmp, &endptr, 10);                // config reading -> thermal_throttle_policy mapping
    if (*endptr != '\0') return 0;                      // 1 2 3  -> 2 0 1
    ttp = (ttp <= 3 && ttp >= 1) ? (ttp + 1 ) % 3 : 1;  // ( x + 1 ) % 3
    custom.fanmode = ttp;
    
    if (!readcfg("turbo",tmp)) return 0;
    custom.turbo = strtoul(tmp, &endptr, 10);
    if (*endptr != '\0') return 0;

    if (!readcfg("maxghz",tmp)) return 0;
    double ghz = strtod(tmp, &endptr);
    if (*endptr != '\0') return 0;
    custom.max_ghz = (unsigned int)(ghz * 1e6);
    return 1;
}

/* reqid is visible as internal id, not cli arg id
 * reqid = 0..3, cli arg = 1..4                 */
int apply_profile(int reqid) {
    reqid = (reqid < 0 ) ? 1 : reqid % 4;   // not necessary. but guard?
    int pfp_count = 3;
    if (reqid == 3 && has_custompfp()) pfp_count++;

    id = reqid % pfp_count;

    set_turbo(profile_p[id]->turbo);    // turbo value must be applied first
                                        // to get correct cpu max value reading below
                                        // no turbo -> cpumax = cpu base 
    if ( id < 3) {
        profile_p[id]->max_ghz = get_cpu_freq(profile_maxfreqs[id]);
    }
    fanmode_setid(profile_p[id]->fanmode);
    set_cpu_freq(profile_p[id]->max_ghz);

    FILE *fptr = fopen(LASTPFP_FILE, "w");
    if (fptr == NULL)
        return 0;
    if (fptr) {
        fprintf(fptr, "%d\n",id);
        fclose(fptr);
    }
    return 1;
}

void toggle_profile(void) {
    int tid=1;              // defaults to internal id balanced
    FILE *fptr = fopen(LASTPFP_FILE, "r");
    if (fptr) {
        fscanf(fptr,"%d",&tid);
        fclose(fptr);
    }
    tid = ( tid +1 ) % 4;   // allow sending the max value of 3 even if custom profile doesn't exist
                            // apply_profile will handle
    apply_profile(tid);
}

int show_profile() {
    return show_osd(profile_p[id]->name); 
}

static int readcfg(char *pkey, char *pval){
    char *homepath = getenv("HOME");
    if (homepath) {
        snprintf(cpath,sizeof(cpath),"%s/.config/roghub/config",homepath); 
    }

    FILE *cfile;
    cfile = fopen(cpath, "r");
    if (cfile == NULL){
        return 0;
    }

    char line[64];

    while (fgets(line, sizeof(line),cfile)){
        char *fsptr;

        fsptr = strchr(line, '\n');
        if (!fsptr) {
            fprintf(stderr,"Invalid config file!\n");
            fclose(cfile);
            return 0;
        }
        if (fsptr) *fsptr = '\0';

        fsptr = strchr(line, '=');

        if (!fsptr){
            fprintf(stderr,"Invalid config file!\n");
            fclose(cfile);
            return 0;
        }

        if (*(fsptr -1) == ' ') {
            *(fsptr-1) = '\0';
        }
        *fsptr = '\0';
        char *key = line;
        char *val = fsptr+1;
        
        while (*key == ' ') key++;
        while (*val == ' ') val++;


        if (strcmp(pkey,key) == 0){
            snprintf(pval, sizeof(pval), "%s",val);
            fclose(cfile);
            return 1;
        }
    }
    fclose(cfile);
    return 0;
}

