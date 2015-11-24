#include "global.h"

extern "C"
{
    #include "user_interface.h"
    #include "osapi.h"
}

#include "lib/esp8266-iot-sdk/espmissingincludes.h"
#include "sys/stdarg.h"
#include "sys/util.h"


__attribute__((noreturn)) void powerdown()
{
    while (true) system_deep_sleep(0xffffffff);
}

/*__attribute__((noreturn))*/ void reset()
{
    static ETSTimer timer;
    memset(&timer, 0, sizeof(timer));
    os_timer_setfn(&timer, (void(*)(void*))0x40000080, NULL);
    os_timer_arm(&timer, 1000, 0);
}

int critsec_level;

void enter_critical_section()
{
    ets_intr_lock();
    critsec_level++;
}

void leave_critical_section()
{
    if (!--critsec_level) ets_intr_unlock();
}

uint32_t get_critsec_state()
{
    return critsec_level;
}

inline void* memcpy(void* dst, const void* src, size_t len)
{
    return os_memcpy(dst, src, len);
}

inline void* memmove(void* dst, const void* src, size_t len)
{
    return os_memmove(dst, src, len);
}

inline void* memset(void* dst, int val, size_t len)
{
    return os_memset(dst, val, len);
}

inline int memcmp(const void* a, const void* b, size_t len)
{
    return os_memcmp(a, b, len);
}

inline size_t strlen(const char* string)
{
    return os_strlen(string);
}

int strcmp(const char* s1, const char* s2)
{
    return ets_strcmp(s1, s2);
}

int vsnprintf(char* buf, size_t size, const char *fmt, va_list ap)
{
    return ets_vsnprintf(buf, size, fmt, ap);
}
