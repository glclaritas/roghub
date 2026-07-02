#include <bits/posix2_lim.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

static char cpath[PATH_MAX];
static config_t config;

config_t* cfg_update(void){
    char *xdg = getenv("XDG_CONFIG_HOME");
    char *home = getenv("HOME");

    if (xdg) {
        snprintf(cpath,sizeof(cpath),"%s/roghub/config",xdg); 
    } else if (home) {
        snprintf(cpath,sizeof(cpath),"%s/.config/roghub/config",home); 
    } else {
        fprintf(stderr, "Error: can't detemine config path.\n");
        return 0;
    }

    FILE *cfile;
    cfile = fopen(cpath, "r");
    if (cfile == NULL){
        return 0;
    }

    char line[LINE_MAX];
    int read_value_count=0;
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

        if (fsptr > line && *(fsptr -1) == ' ') {
            *(fsptr-1) = '\0';
        }
        *fsptr = '\0';
        char *key = line;
        char *val = fsptr+1;
        
        while (*key == ' ') key++;
        while (*val == ' ') val++;

        if (strcmp(key, "name") == 0) {
            snprintf(config.name, sizeof(config.name), "%s", val);
            read_value_count++;
        }
        else if (strcmp(key, "maxghz") == 0) {
            config.maxkhz = atof(val) * 1e6;
            read_value_count++;
        }
        else if (strcmp(key, "fanmode") == 0) {
            config.fanmode = (atoi(val)+1) % 3;
            read_value_count++;
        }
        else if (strcmp(key, "turbo") == 0) {
            config.turbo = atoi(val);
            read_value_count++;
        }
    }
    fclose(cfile);
    if (read_value_count == 4) {
        return &config;
    } else return NULL;
}
