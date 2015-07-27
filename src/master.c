#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "ansi-common.h"

struct {
    volatile int is_on;
    volatile int is_reset;
    volatile int is_pending;
} ptt_status;

void usage(char *app) {
    printf("%s <device_emu_file>\n", app);
    exit(-1);
}

void *master_thread(void *p_data) {
    int fdm;
    int rc;
    int n;
    char buffer[10];
    int *f_quit = 0;
    fd_set readset;
    fd_set writeset;
    struct timeval timeout;

    fdm = posix_openpt(O_RDWR);
    ANSI_EXIT_IF_TRUE(fdm < 0, "Cannot create pseudo-terminal master\n");

    rc = grantpt(fdm);
    ANSI_EXIT_IF_TRUE(rc != 0, "Cannot grantpt (change access right)\n");

    rc = unlockpt(fdm);
    ANSI_EXIT_IF_TRUE(rc != 0, "Cannot unlockpt (unlock slave side)\n");

    printf("Pseudo terminal file: %s\n", ptsname(fdm));

    f_quit = p_data;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10*1000;
    while(!(*f_quit)) {
        FD_ZERO(&readset);
        FD_SET(fdm, &readset);
        FD_ZERO(&writeset);
        FD_SET(fdm, &writeset);
        rc = select(fdm + 1, &readset, &writeset, NULL, &timeout);
        if( rc > 0 ) {
            if( FD_ISSET(fdm, &readset) ) {
                n = read(fdm, buffer, sizeof(buffer));
                buffer[n] = '\0';
                if( n > 0 ) {
                    if (ptt_status.is_on) write(fdm, "L1", 2);
                    else write(fdm, "L0", 2);
                }
                else ANSI_EXIT_IF_TRUE_V2(n < 0);
            }
            else if ( FD_ISSET(fdm, &writeset) ) {
                if(ptt_status.is_pending) {
                    if (ptt_status.is_reset) {
                        write(fdm, "ready\n", 6);
                        ptt_status.is_reset = 0;
                    }
                    else {
                        if (ptt_status.is_on) write(fdm, "L1", 2);
                        else write(fdm, "L0", 2);
                    }
                    ptt_status.is_pending = 0;
                }
            }
        }
        usleep(100*1000);
    }
    printf("main thread end\n");
    return NULL;
}

int app_main() {
    int f_quit = 0;
    char buffer[10];
    char cmd[2];
    int len;

    pthread_t thread;

    // UI thread
    f_quit = 0;
    pthread_create(&thread, NULL, &master_thread, (void*)&f_quit);
    ptt_status.is_pending = 0;
    ptt_status.is_on = 0;
    while(!f_quit) {
        fgets(buffer, sizeof(buffer), stdin);
        len = strlen(buffer);
        sscanf(buffer, "%1s", cmd);
        switch (cmd[0]) {
            case 'x':
            case 'X':
                f_quit = 1;
                break;
            case 't':
            case 'T':
                if (ptt_status.is_pending != 0) {
                    printf("Previous command is pending\n");
                }
                else {
                    ptt_status.is_on = 1;
                    ptt_status.is_pending = 1;
                }
                break;
            case 'r':
            case 'R':
                if (ptt_status.is_pending != 0) {
                    printf("Previous command is pending\n");
                }
                else {
                    ptt_status.is_on = 0;
                    ptt_status.is_pending = 1;
                }
                break;
            case 'e':
            case 'E':
                if (ptt_status.is_pending != 0) {
                    printf("Previous command is pending\n");
                }
                else {
                    ptt_status.is_on = 0;
                    ptt_status.is_reset = 1;
                    ptt_status.is_pending = 1;
                }
                break;
            default:
                printf("Received: %c\n", cmd[0]);
                break;
        }
    }
    pthread_join(thread, NULL);
    return 0;
}

int main(int argc, char *argv[]) {
    /*if( argc < 2) {
        usage(argv[0]);
    }*/
    return app_main();
}
