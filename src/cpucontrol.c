#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cpucontrol.h"

#define CPU_BOOST_PSTATE_PATH "/sys/devices/system/cpu/intel_pstate/no_turbo"
#define CPU_BOOST_PATH "/sys/devices/system/cpu/cpufreq/boost"

unsigned int cpu_get_freq(const char *path) {
    unsigned int freq = 0;
    char buffer[16];
    ssize_t len;
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error-1. Can't read CPU freq.\n");
        return freq;
    }
    len = read(fd, buffer, sizeof(buffer));
    close(fd); 
    if (len <= 0) {
        fprintf(stderr, "Error-2. Can't read CPU freq. Invalid input.\n");
    } else {
        buffer[len - 1] = '\0';
        freq = (unsigned int)strtol(buffer, NULL, 10);
    }
    return freq;
}

int cpu_set_freq(unsigned int khz) {
    if (khz == cpu_get_freq(SCALING_MAX_FILE)) {
        return 1;
    }
    if (khz > cpu_get_freq(CPU_MAX_FILE) || khz < cpu_get_freq(CPU_MIN_FILE) || khz == 0){
        fprintf(stderr,"Target cpu frequency is not a valid value. %u\n", khz);
        return 0;
    }
    char path[128];
    int ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    for (int i = 0; i < ncpu; i++){
        snprintf(path, sizeof(path), CPU_FREQ_PATH "/cpu%d/cpufreq/scaling_max_freq",i);
        int fd = open(path, O_WRONLY | O_CLOEXEC);
        if (fd == -1) {
            fprintf(stderr,"Can't set cpu frequency values.\n");
            return 0;
        }
        char buffer[16];
        ssize_t len = snprintf(buffer, sizeof(buffer), "%u", khz);
        if (write(fd, buffer, len) <= 0 ) {
            fprintf(stderr,"Can't set cpu frequency values. Invalid input.\n");
            close(fd);
            return 0;
        }
        close(fd);
    }
    return 1;
}

static int cpu_get_boostpath(char path[128]) {
    char governor[32];
    int fd;
    ssize_t len;

    fd = open(SCALING_DRIVER_FILE, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Can't read scaling driver.\n");
        return 0;
    }
    len = read(fd, governor, sizeof(governor));
    close(fd);
    if (len <= 0) {
        fprintf(stderr, "Can't read scaling driver. Invalid input.\n");
        return 0;
    }
    if (governor[len-1] == '\n') governor[len-1] = '\0';
    if (strcmp(governor, "intel_pstate") == 0){
    snprintf(path, 128, "%s", CPU_BOOST_PSTATE_PATH);
    }else snprintf(path, 128, "%s", CPU_BOOST_PATH);
    return 1;
}

int cpu_set_turbo(int val) {
    if ( val != 0 && val != 1 ) {
        fprintf(stderr, "Invalid boost mode value. %d\n",val);
        return 0;
    }

    char boost_path[128];
    if (!cpu_get_boostpath(boost_path)) return 0;
    if (strcmp(boost_path, CPU_BOOST_PSTATE_PATH) == 0){
        val = !val;            // flip intel pstate takes !true input
    }
    
    int fd = open(boost_path, O_WRONLY | O_CLOEXEC);
    if (fd == -1) {
        fprintf(stderr, "Can't change cpu boost state at path '%s'.\n", boost_path);
        return 0;
    }
    char c = '0' + val;
    int len = write(fd, &c, 1);
    close(fd);
    if (len != 1) {
        fprintf(stderr, "Can't change cpu boost state. Invalid input.\n");
        return 0;
    }
    return 1;
}

int cpu_get_turbo() {
    char boost_path[128], val;
    if (!cpu_get_boostpath(boost_path)) return -1;

    int fd = open(boost_path, O_RDONLY);
    int len = read(fd, &val, 1);
    if (len != 1) return -1;
    int turbo = val - '0';
    if (strcmp(boost_path, CPU_BOOST_PSTATE_PATH) == 0) turbo = !turbo; // flip intel pstate takes !true input
    return turbo;
}
