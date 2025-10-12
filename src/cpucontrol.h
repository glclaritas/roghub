#ifndef CPUCONTROL_H
#define CPUCONTROL_H

#define CPU_MAX_FILE "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
#define CPU_MIN_FILE "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq"

/* enable or disable cpu boost mode 
 * 0 disable, 1 enable*/
int set_turbo(int turbo);

/* get cpu freq from path in khz*/
unsigned int get_cpu_freq(const char *path);

/* set cpu scaling max freq to kz */
int set_cpu_freq(unsigned int khz);

#endif
