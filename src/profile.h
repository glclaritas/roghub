#ifndef PROFILES_H
#define PROFILES_H

/* apply given profile id */
int profile_apply(int id);

/* toggle to next profile id */
void profile_toggle(void);

/* show profile osd */
int profile_display();

typedef struct {
    char name[32];
    int fanmode;
    int turbo;
    unsigned int max_khz;
} profile_t;

#endif
