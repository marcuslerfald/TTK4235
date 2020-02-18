/**
 * @file timer.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-02-18
 * 
 * @copyright Copyright (c) 2020
 * 
 * 
 */
#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

#define TIMER_TIMOUT_SECONDS 3

/**
 * @brief Saves current unix timestamp
 * 
 */
void timer_restart();

/**
 * @brief Checks if timer has timed out
 * 
 * @return true  More than TIMER_TIMEOUT_SECONDS seconds since last call to timer_restart()
 * @return false Otherwise
 */
bool timer_check_timeout();

#endif // TIMER_H