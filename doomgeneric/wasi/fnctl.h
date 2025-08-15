// <fnctl.h>

#if !defined(O_NONBLOCK)
#define F_GETFL  3
#define F_SETFL  4
#define O_NONBLOCK    04000

SCOPE("fnctl")
int fcntl(int fd, int cmd, ...)
Begin
#if defined(implementation)
    va_list args;
    va_end(args);
    return -1;
}
#endif
End
#endif

