#include "timer.h"

#include <time.h>
#include <stdint.h>

static time_t m_last_timestamp;

void timer_restart()
{
    m_last_timestamp = time(NULL);
}


bool timer_check_timeout()
{
    time_t new_timestamp = time(NULL);

    if((intmax_t) new_timestamp - (intmax_t) m_last_timestamp >= TIMER_TIMOUT_SECONDS)
    {
        return true;
    }

    return false;
}
