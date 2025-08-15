#pragma once

#if !defined(implementation)
#   pragma message "proto libc extra"
#   define SCOPE(name) extern
#   define Begin
#   define End ;
#else
#   pragma message "linking libc extra"
#   define SCOPE(name) __attribute__((export_name(name)))
#   define Begin {
#   define End }
#endif

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>


// sys/time.h:
#include "wasi/sys/time.h"


#if !defined(__clang__)

// <sys/iotctl.h>
#include "wasi/sys/ioctl.h"


// <termios.h>
#include "wasi/termios.h"

// <linux/kd.h>
#define 	KB_84		0x01
#define 	KB_101		0x02
#define KDSKBMODE	0x4B45	/* sets current keyboard mode */
#define KDGKBTYPE	0x4B33
#define KDGKBMODE	0x4B44
#define	K_MEDIUMRAW	0x02

// <stdio.h>
SCOPE("rename")
int rename(const char *oldpath, const char *newpath)
Begin
#if defined(implementation)
    // don't lie
    return -1;
#endif
End

// <unistd.h>
typedef unsigned long __useconds_t;


SCOPE("usleep")
int usleep (__useconds_t __useconds)
Begin
#if defined(implementation)
    // lie !
    return 0;
#endif
End


SCOPE("sscanf")
int sscanf(const char *input, const char *format, ...)
Begin
#if defined(implementation)
    va_list args;
    va_start(args, format);

    const char *f = format;
    const char *s = input;
    int assigned = 0;

    while (*f && *s) {
        if (*f == '%') {
            f++;
            if (*f == 'd') {
                int *iptr = va_arg(args, int *);
                *iptr = atoi(s);
                while (isdigit(*s) || *s == '-') s++;
                assigned++;
            } else if (*f == 'f') {
                float *fptr = va_arg(args, float *);
                *fptr = atof(s);
                while (isdigit(*s) || *s == '.' || *s == '-') s++;
                assigned++;
            } else if (*f == 's') {
                char *str = va_arg(args, char *);
                while (*s && !isspace(*s)) {
                    *str++ = *s++;
                }
                *str = '\0';
                assigned++;
            }
            f++;
        } else {
            if (*f == *s) {
                f++;
                s++;
            } else {
                break;
            }
        }
    }

    va_end(args);
    return assigned;
#endif
End




#endif // !__clang__


// <stdlib.h>
SCOPE("system")
int system(const char *command)
Begin
#if defined(implementation)
    // lie !q
    return 0;
#endif
End

