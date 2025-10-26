#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cpucontrol.h"

#define CPU_FREQ_PATH "/sys/devices/system/cpu"
#define SCALING_MAX_FILE "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq"
#define SCALING_DRIVER_FILE "/sys/devices/system/cpu/cpu0/cpufreq/scaling_driver"

unsigned int get_cpu_freq(const char *path) {
    unsigned int freq;
    FILE *fptr = fopen(path, "r");
    if(!fptr) {
        fprintf(stderr, "Can't read cpu frequency values.\n");
        return 0;
    }
    if (fscanf(fptr, "%d",&freq) != 1) {
        fprintf(stderr,"Failed to read %s.\n",path);
        fclose(fptr);
        return 0;
    }
    fclose(fptr);
    return freq;
}

int set_cpu_freq(unsigned int khz) {
    if (khz == get_cpu_freq(SCALING_MAX_FILE)) {
        return 0;
    }
    if (khz > get_cpu_freq(CPU_MAX_FILE) || khz < get_cpu_freq(CPU_MIN_FILE) || khz == 0){
        fprintf(stderr,"Target cpu frequency is not a valid value. %ul\n", khz);
        return 0;
    }
    char path[128];
    int ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    for (int i = 0; i < ncpu; i++){
        snprintf(path, sizeof(path), CPU_FREQ_PATH "/cpu%d/cpufreq/scaling_max_freq",i);
        FILE *fptr = fopen(path, "w");
        if (fptr == NULL){
            fprintf(stderr,"Can't set cpu frequencies. Permission denied!\n");
            return 0;
        }
        fprintf(fptr,"%u",khz);
        fclose(fptr);
    }
    return 1;
}

int set_turbo(int val) {
    if ( val != 0 && val != 1 ) {
        fprintf(stderr, "Invalid boost mode value. %d\n",val);
        return 0;
    }
    char governor[32];
    char boost_path[128];
    FILE *fptr = fopen(SCALING_DRIVER_FILE, "r");
    if (!fptr) {
        fprintf(stderr, "Can't read cpu boost state.\n");
        return 0;
    }
    if (fscanf(fptr, "%s", governor) != 1) {
        printf("Can't read %s.Exiting\n",SCALING_DRIVER_FILE);
        fclose(fptr);
        return 0;
    }
    fclose(fptr);
    if (strcmp(governor, "intel_pstate") == 0){
        snprintf(boost_path, sizeof(boost_path), "/sys/devices/system/cpu/intel_pstate/no_turbo");
        val = (val + 1) % 2;            // flip intel pstate takes !true input
    }else snprintf(boost_path, sizeof(boost_path), "/sys/devices/system/cpu/cpufreq/boost");
    
    fptr = fopen(boost_path, "w");
    if (!fptr) {
        fprintf(stderr, "Can't change cpu boost state.\n");
        return 0;
    }
    fprintf(fptr,"%d",val);
    fclose(fptr);
    return 1;
}
