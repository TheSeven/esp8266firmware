#include "global.h"
#include "sys/time.h"

extern "C"
{
#include "user_interface.h"
}


unsigned int read_usec_timer()
{
    return system_get_time();
}
