#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

static char cpath[2048];
char cval[32];

char* cfg_read(char *ckey){
    char *homepath = getenv("HOME");
    if (!homepath) {
        fprintf(stderr, "$HOME not set. Can't load config.\n");
        return NULL;
    }
    snprintf(cpath,sizeof(cpath),"%s/.config/roghub/config",homepath); 

    FILE *cfile;
    cfile = fopen(cpath, "r");
    if (cfile == NULL){
        return NULL;
    }

    char line[128];
    while (fgets(line, sizeof(line),cfile)){
        char *fsptr;

        fsptr = strchr(line, '\n');
        if (!fsptr) {
            fprintf(stderr,"Invalid config file!\n");
            fclose(cfile);
            return NULL;
        }
        if (fsptr) *fsptr = '\0';

        fsptr = strchr(line, '=');

        if (!fsptr){
            fprintf(stderr,"Invalid config file!\n");
            fclose(cfile);
            return NULL;
        }

        if (fsptr > line && *(fsptr -1) == ' ') {
            *(fsptr-1) = '\0';
        }
        *fsptr = '\0';
        char *key = line;
        char *val = fsptr+1;
        
        while (*key == ' ') key++;
        while (*val == ' ') val++;

        if (strcmp(key,ckey) == 0){
            snprintf(cval, sizeof(cval), "%s",val);
            fclose(cfile);
            return cval;
        }
    }
    fclose(cfile);
    return NULL;
}

int cfg_set(char *key, char *val) {
    return 0;
}
