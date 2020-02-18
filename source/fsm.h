/**
 * @file fsm.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-02-11
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note Based on https://barrgroup.com/embedded-systems/how-to/state-machines-event-driven-systems 
 * 
 */

#ifndef __FSM_H__
#define __FSM_H__

typedef enum
{
    EVENT_HW_INIT_DONE,
    EVENT_FOUND_FLOOR,
    EVENT_STOP_BUTTON_PRESSED,
    EVENT_STOP_BUTTON_RELEASED,
    EVENT_TIMER_TIMEOUT,
    EVENT_OBSTRUCTION_ACTIVE,
    EVENT_REACHED_FLOOR_IN_QUEUE,

    // Not to used outside .c file
    EVENT_ENTRY = 0xFE,
    EVENT_EXIT = 0xFF
} fsm_event_t;

typedef enum
{
    STATE_INIT,
    STATE_UNKNOWN_FLOOR,
    STATE_IDLE,
    STATE_MOVING,
    STATE_DOOR_OPEN,
    STATE_EMERGENCY_STOP,
    STATE_EMERGENCY_STOP_FLOOR,
    STATE_EMERGENCY_STOP_NOWHERE
} fsm_state_t;


/**
 * @brief Sets initial state of state machine
 * 
 */
void fsm_init();

/**
 * @brief Runs one iteration of the state machine
 * 
 */
void fsm_run();

/**
 * @brief Dispatches event to the state machine
 * 
 * @param event 
 */
void fsm_dispatch(fsm_event_t event);


#endif __FSM_H__