/**
 * @file timer.h
 * @brief Implements a timer that times out in @c TIMER_TIMEOUT_SECONDS seconds  
 */
#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

#define TIMER_TIMOUT_SECONDS 3

/**
 * @brief Saves current unix timestamp
 */
void timer_restart();

/**
 * @brief Checks if timer has timed out
 * 
 * @return true  More than @c TIMER_TIMEOUT_SECONDS seconds since last call to timer_restart()
 * @return false Otherwise  
 * 
 * @warning timer_restart() should be called at least once to get known behaviour
 */
bool timer_check_timeout();

#endif // TIMER_H