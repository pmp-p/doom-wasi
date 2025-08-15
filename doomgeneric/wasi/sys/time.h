
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>


#if !defined(__clang__)
struct
timeval {
    int64_t tv_sec;   // seconds
    int32_t tv_usec;  // microseconds
};

#endif

struct
timezone {
    int tz_minuteswest; // Minutes west of Greenwich (UTC)
    int tz_dsttime;     // Type of daylight saving time correction
};

SCOPE("gettimeofday")
int gettimeofday(struct timeval *tv, struct timezone *tz)
Begin
#if defined(implementation)
    if (!tv) {
        errno = EINVAL;
        return -1;
    }

    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        return -1;
    }

    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;

    // WASI doesn't support timezone info, so we ignore tz
    if (tz) {
        tz->tz_minuteswest = 0;
        tz->tz_dsttime = 0;
    }

    return 0;
#endif
End
