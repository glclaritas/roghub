#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char name[32];
    int fanmode;
    int turbo;
    unsigned int maxkhz;
} config_t;

/* read key values from config file and return pointer to config struct */
config_t* cfg_update(void);

#endif
