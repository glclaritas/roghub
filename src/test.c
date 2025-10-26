#include <stdio.h>

#include "cpucontrol.h"
#include "fancontrol.h"

int main() {
    puts("Testing cpu control error handling"); 

    // non existence cpu freq readings
    unsigned int val = cpu_get_freq("/no/path"); 
    if (val != 0) {
        fprintf(stderr, "cpu reading test failed.\n");
        return 1;
    }
    puts("Invalid path cpu freq reading test: PASSED"); 

    // setting cpu freq to invalid values
    int ret = cpu_set_freq(0);
    if (ret != 0) {
        fprintf(stderr, "cpu freq setting test failed.\n");
        return 1;
    }
    puts("Invalid cpu freq(0) test: PASSED"); 
    ret = cpu_set_freq(-1);
    if (ret != 0) {
        fprintf(stderr, "cpu freq setting test failed.\n");
        return 1;
    }
    puts("Invalid cpu freq(-1) test: PASSED"); 
    ret = cpu_set_freq(999999999ul);
    if (ret != 0) {
        fprintf(stderr, "cpu freq setting test failed.\n");
        return 1;
    }
    puts("Invalid cpu freq(999999999) test: PASSED"); 

    // setting turbo to invalid values
    ret = cpu_set_turbo(-1);
    if (ret != 0) {
        fprintf(stderr, "boostmode setting test failed!\n");
        return 1;
    }
    puts("Invalid boost mode(-1) setting test: PASSED!");
    ret = cpu_set_turbo(2);
    if (ret != 0) {
        fprintf(stderr, "boostmode setting test failed!\n");
        return 1;
    }
    puts("Invalid boost mode(2) setting test: PASSED!");

    puts("Testing fan control error handling");
    TTP_FILE="/no/path";
    int fid = fanmode_getid();
    if (fid != 0) {
        fprintf(stderr, "fanmode value reading test failed!\n");
        return 1;
    }
    puts("Invalid fanmode path reading test: PASSED");
    // setting fanmode to invalid values
    ret = fanmode_setid(-1);
    if (ret != 0) {
        fprintf(stderr, "fanmode value setting test failed!\n");
        return 1;
    }
    puts("Invalid fanmode value(-1) setting test: PASSED");
    ret = fanmode_setid(0);
    if (ret != 0) {
        fprintf(stderr, "fanmode value setting test failed!\n");
        return 1;
    }
    puts("Testing permission denied test: PASSED");
    ret = fanmode_setid(3);
    if (ret != 0) {
        fprintf(stderr, "fanmode value setting test failed!\n");
        return 1;
    }
    puts("Invalid fanmode value(3) setting test: PASSED");
    ret = fanmode_toggle();
    if (ret != 0) {
        fprintf(stderr, "fanmode toggling test failed!\n");
        return 1;
    }
    puts("Invalid fanmode toggle setting test: PASSED");

    puts("All tests success");
    return 0;
}
