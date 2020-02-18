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

#ifndef FSM_H
#define FSM_H

#define FOREACH_STATE(STATE)                \
        STATE(STATE_INIT)                   \
        STATE(STATE_UNKNOWN_FLOOR)          \
        STATE(STATE_IDLE)                   \
        STATE(STATE_MOVING)                 \
        STATE(STATE_DOOR_OPEN)              \
        STATE(STATE_EMERGENCY_STOP)         \
        STATE(STATE_EMERGENCY_STOP_FLOOR)   \
        STATE(STATE_EMERGENCY_STOP_NOWHERE)

#define FOREACH_EVENT(EVENT)                \
        EVENT(EVENT_HW_INIT_DONE)           \
        EVENT(EVENT_FOUND_FLOOR)            \
        EVENT(EVENT_STOP_BUTTON_PRESSED)    \
        EVENT(EVENT_STOP_BUTTON_RELEASED)   \
        EVENT(EVENT_TIMER_TIMEOUT)          \
        EVENT(EVENT_OBSTRUCTION_ACTIVE)     \
        EVENT(EVENT_REACHED_FLOOR_IN_QUEUE) \
        EVENT(EVENT_ENTRY)                  \
        EVENT(EVENT_EXIT)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum
{
    FOREACH_EVENT(GENERATE_ENUM)
} fsm_event_t;

typedef enum
{
    FOREACH_STATE(GENERATE_ENUM)
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

/**
 * @brief Returns current state of state machine
 * 
 * @return fsm_state_t 
 */
fsm_state_t fsm_get_state();


#endif // FSM_H