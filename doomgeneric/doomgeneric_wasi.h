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



SCOPE("sscanf")
int sscanf(const char *input, const char *format, ...)
Begin
#if defined(implementation)
#   include <ctype.h>
#   include <stdlib.h>
#   include <stdarg.h>
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
