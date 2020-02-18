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

// EVENT_FLOOR_1-4 placed in beginning to use numerical value
#define FOREACH_EVENT(EVENT)                \
        EVENT(EVENT_FLOOR_1)                \
        EVENT(EVENT_FLOOR_2)                \
        EVENT(EVENT_FLOOR_3)                \
        EVENT(EVENT_FLOOR_4)                \
        EVENT(EVENT_HW_INIT_DONE)           \
        EVENT(EVENT_STOP_BUTTON_PRESSED)    \
        EVENT(EVENT_STOP_BUTTON_RELEASED)   \
        EVENT(EVENT_TIMER_TIMEOUT)          \
        EVENT(EVENT_OBSTRUCTION_ACTIVE)     \
        EVENT(EVENT_QUEUE_NOT_EMPTY)        \
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
 * @brief Sets initial state of state machine.
 * 
 * @note Should only be called once at startup of program  
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
 * @param event The event to be handled by the state machine
 */
void fsm_dispatch(fsm_event_t event);

/**
 * @brief Returns current state of state machine
 * 
 * @return fsm_state_t The current state of the state machine
 */
fsm_state_t fsm_get_state();


#endif // FSM_H