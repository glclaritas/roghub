#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "fancontrol.h"
#include "profile.h"
#include "version.h"

void print_ver() {
    printf("ROG Hub (%s v%s)\n",APP_NAME,APP_VERSION_STRING); 
}

void print_help(const char *pgname) {
    printf("Usage: %s [OPTION]...\n", pgname);
    puts("Change performance and fan profiles.\n");
    puts("Options:");
    puts("  -h, --help                show this help message");
    puts("  -p, --profile [num]       set power profile number (1=Silent, 2=Balanced, 3=Turbo)");
    puts("  -t, --toggle              toggle performance profiles");
    puts("  -f, --fan-toggle          toggle fan(only) profiles");
    puts("  -d, --display             display OSD (requires swayosd)");
    puts("  -v, --version             print version information");
    puts("");
    puts("Config file ~/.config/roghub/config usage example");
    puts("  name=<profile-name>       profile name under 30 characters");
    puts("  maxghz=4.5                valid cpu freq in ghz or falls back to default value");
    puts("  fanmode=1                 fanspeed. valid values are 1, 2, 3");
    puts("  turbo=1                   enable or disable cpu boost. 1 = enabled, 0 = disabled");
}

int main(int argc, char *argv[]) {
    int help = 0;
    int opt;
    int option_index = 0;
    int profile_num = -1;
    int profilearg = 0;
    int togglearg = 0;
    int ftogglearg = 0;
    int displayarg = 0;

    static struct option long_options[] = {
        {"help",        no_argument,        0, 'h'},
        {"version",     no_argument,        0, 'v'},
        {"profile",     required_argument,  0, 'p'},
        {"toggle",      no_argument,        0, 't'},
        {"display",     no_argument,        0, 'd'},
        {"fan-toggle",  no_argument,        0, 'f'},
        {0, 0, 0, 0}
    };

    if (argc < 2) {
        print_help(argv[0]);
        return 0;
    }

    while ((opt = getopt_long(argc, argv, "hp:tdfv", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'v':
                print_ver();
                return 0;
            case 'h':
                help = 1;
                break;

            case 'p': {
                profilearg = 1;
                char *endptr;
                profile_num = strtol(optarg, &endptr, 10);
                if (*endptr != '\0' || profile_num < 1 || profile_num > 4) {
                    fprintf(stderr, "Error: profile number must be a valid digit.\n");
                    fprintf(stderr, "Accepted values: 1, 2 , 3 ,4.\n");
                    fprintf(stderr, "4 = Custom profile, if config doesn't exist, 4 means Silent\n");
                    return 1;
                }
                break;
            }

            case 'f':
                ftogglearg = 1;
                break;

            case 't':
                togglearg = 1;
                break;

            case 'd':
                displayarg = 1;
                break;

            case '?':
                return 1;

            default:
                print_help(argv[0]);
                return 0;
        }
    }
    if (help) {
        print_help(argv[0]);
        return 0;
    }
    if ((togglearg + ftogglearg + profilearg) > 1) {
        fprintf(stderr,"Error: actions[-t/-p/-f] can't be used together!\n");
        return 1;
    }
    if ( displayarg==1 && (profilearg + togglearg + ftogglearg) != 1) {
        fprintf(stderr, "Error: -d/--display must be used with an action!\n");
        return 1;
    }
    if (profilearg){
        profile_apply(profile_num-1);       // -1 to offset internal working id and cli arg id
                                            // interal id: 0 1 2 3
                                            // cli arg id: 1 2 3 4
        if (displayarg) profile_display();
    }

    if (togglearg){
        profile_toggle();
        if (displayarg) profile_display();
    }
    
    if (ftogglearg){
        fanmode_toggle();
        if (displayarg) fanmode_display();
    }

    return 0;
}
