#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#define ASCII_HASH          0x23    // #
#define ASCII_SEMICOLON     0x3B    // ;
#define ASCII_NUL           0x00    // \0
#define ASCII_LF            0x0A    // \n
#define ASCII_DELIM         0x3D    // =
#define ASCII_SPACE         0x20    // whitespace

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
        fprintf(stderr, "Error: can't determine config path.\n");
        return 0;
    }
    
    FILE *file = fopen(cpath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: can't open file: %s.\n", cpath);
        return 0;
    }
    
    int current_line = 0;
    int read_value_count = 0;
    char *delim = NULL, *key = NULL, *val = NULL, *lastchar = NULL, *comment = NULL;
    const char *comments = ";#";
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while((nread = getline(&line, &len, file)) != -1) {
        ++current_line;
        if (line[0] == ASCII_SEMICOLON || line[0] == ASCII_HASH || line[0] == ASCII_SPACE || line[0] == ASCII_LF || line[0] == ASCII_DELIM) continue;

        line[nread-1] = ASCII_NUL;
        lastchar = &line[nread-2];

        delim = strchr(line, ASCII_DELIM);
        if (delim == NULL) {
            delim = lastchar =  NULL;
            continue;
        }

        comment = strpbrk(line, comments);
        if (comment && *(comment-1) == ASCII_SPACE) {
            lastchar = comment-1; 
        }

        key = line;
        val = delim+1;

        // trim spaces around =
        while(*(delim-1) == ASCII_SPACE) {
            delim--;
        }
        *delim = ASCII_NUL;
        while(*val == ASCII_SPACE) val++;

        // trim leading spaces from keys
        while(*key == ASCII_SPACE) {
            key++;
        }

        // trim trailing spaces from values
        while(*lastchar == ASCII_SPACE) {
            *lastchar = ASCII_NUL;
            lastchar--;
        }

        // empty key
        if (key[0] == ASCII_NUL) {
            delim = key = val = lastchar = NULL;
            continue;
        }

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

        printf("Read config '%s':'%s'\n", key, val);

        // reset
        delim = key = val = lastchar = NULL;
    }

    fclose(file);
    if (read_value_count == 4) {
        return &config;
    } else return NULL;
}
