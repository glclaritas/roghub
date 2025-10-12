#ifndef PROFILES_H
#define PROFILES_H

/* apply given profile id */
int apply_profile(int id);

/* toggle to next profile id */
void toggle_profile(void);

/* show profile osd */
int show_profile();

typedef struct {
    char name[32];
    int fanmode;
    int turbo;
    unsigned int max_ghz;
} profile_t;

#endif
