#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include "ansi-common.h"

void usage(char *app) {
    printf("%s <device_file>\n", app);
    exit(-1);
}

#define N_PERIOD 10
int main(int argc, char *argv[]) {
    if( argc < 2 ) usage(argv[0]);
    
    int cnt, cnt1;
    int fd;
    int n;
    int rc;
    char buffer[10];
    struct termios term_settings;
    struct termios new_term_settings;

    struct timeval timeout;

    fd_set readset;
    fd_set writeset;
    fd_set exceptset;
    
    fd = open(argv[1], O_RDWR);
    ANSI_EXIT_IF_TRUE(fd < 0, "Cannot open file");
    tcgetattr(fd, &term_settings);
    new_term_settings = term_settings;
    //cfmakeraw(&new_term_settings);
    cfsetispeed(&new_term_settings, B9600);
    cfsetospeed(&new_term_settings, B9600);

    new_term_settings.c_cflag |= (CREAD |CLOCAL);
    new_term_settings.c_cflag &= ~CSIZE; /* Mask the character size bits */
    new_term_settings.c_cflag |= CS8;    /* Select 8 data bits */
    new_term_settings.c_cflag &= ~CRTSCTS;
    new_term_settings.c_iflag &= ~(IXON | IXOFF | IXANY);
    //options->c_lflag |= (ICANON | ECHO | ECHOE); // Canonical mode

    new_term_settings.c_lflag &= ~(ICANON | ECHO); // RAW mode
    new_term_settings.c_cc[VMIN] = 0;
    new_term_settings.c_cc[VTIME] = 5; // measured in 0.1 second
    tcsetattr(fd, TCSANOW, &new_term_settings);
    cnt = 0;
    cnt1 = 0;
    while(1) {
        if (cnt > 3) {
            fprintf(stdout, "Channel probing failed. Device disconnected\n");
            fflush(stdout);
            break;
        }
        FD_ZERO(&readset);
        FD_SET(fd, &readset);
        FD_ZERO(&writeset);
        FD_SET(fd, &writeset);
        FD_ZERO(&exceptset);
        FD_SET(fd, &exceptset);
        timeout.tv_sec = 0;
        timeout.tv_usec = 20*1000;

        rc = select(fd + 1, &readset, &writeset, &exceptset, &timeout);
        if (rc > 0) {
            if(FD_ISSET(fd, &readset)) {
                n = read(fd, buffer, sizeof(buffer) - 1);
                ANSI_EXIT_IF_TRUE_V2(n < 0);
                if( n > 0 ) {
                    buffer[n] = '\0';
                    fprintf(stdout, "1-%d\n", cnt);fflush(stdout);
                    cnt = 0;
                    write(2, buffer, n);
                    //fprintf(stdout, "%s", buffer);
                    //fflush(stdout);
                }
            }
            if(FD_ISSET(fd, &writeset)) {
                cnt1 = (cnt1 + 1) % N_PERIOD;
                if (cnt1 == 0) {
                    cnt++;
                    fprintf(stdout, "probing %d\n", cnt);
                    fflush(stdout);
                    write(fd, "Q", 1);
                }
            }
            if(FD_ISSET(fd, &exceptset)) {
                fprintf(stdout, "Exception happens...\n");
                fflush(stdout);
            }
        }
        usleep(500*1000);
    }
    return 0;
}
