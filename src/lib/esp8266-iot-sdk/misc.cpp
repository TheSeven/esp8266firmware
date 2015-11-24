#include "global.h"
#include "misc.h"
#include "c_types.h"
#include "sys/init.h"

extern "C" void ICACHE_FLASH_ATTR user_init()
{
    sys_init();
}
