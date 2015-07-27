#ifndef __ANSI_COMMON_H__
#define __ANSI_COMMON_H__
#include <errno.h>
#include <string.h>
#define ANSI_EXIT_IF_TRUE(clause, message) if(clause) { \
    fprintf(stderr, message); \
    exit(-1); \
}
#define ANSI_EXIT_IF_TRUE_V2(clause) if(clause) { \
    fprintf(stderr, "%s\n", strerror(errno)); \
    exit(-1); \
}
#endif
