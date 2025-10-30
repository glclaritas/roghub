#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>

/* read key value and write to out_buffer */
int cfg_read(char *key, char *out_buffer, size_t buffer_size);

#endif
