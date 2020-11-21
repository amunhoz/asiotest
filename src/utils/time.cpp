
#include <utils/time.h>

uint64_t getCurrentMs64() {
#ifdef WIN32
    return GetTickCount();
#else
    struct timeval time;
    gettimeofday(&time, NULL);
    return uint64_t((time.tv_sec * 1000) + (time.tv_usec / 1000));
#endif
}

uint32_t getCurrentMs() {
#ifdef WIN32
    return GetTickCount();
#else
    struct timeval time;
    gettimeofday(&time, NULL);
    return uint32_t((time.tv_sec * 1000) + (time.tv_usec / 1000));
#endif
}