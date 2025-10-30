#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

static char cpath[2048];

int cfg_read(char *ckey, char *out_buffer, size_t buffer_size){
    char *homepath = getenv("HOME");
    if (!homepath) {
        fprintf(stderr, "$HOME not set. Can't load config.\n");
        return 0;
    }
    snprintf(cpath,sizeof(cpath),"%s/.config/roghub/config",homepath); 

    FILE *cfile;
    cfile = fopen(cpath, "r");
    if (cfile == NULL){
        return 0;
    }

    char line[128];
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

        if (strcmp(key,ckey) == 0){
            snprintf(out_buffer, buffer_size, "%s",val);
            fclose(cfile);
            return 1;
        }
    }
    fclose(cfile);
    return 0;
}
