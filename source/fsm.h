/**
 * @file fsm.h
 * @brief Finite state machine for an elevator
 * 
 * @note Based on https://barrgroup.com/embedded-systems/how-to/state-machines-event-driven-systems 
 */

#ifndef FSM_H
#define FSM_H

/**
 * @brief All the states the elevator can be in
 */
#define FOREACH_STATE(STATE)                \
        STATE(STATE_INIT)                   \
        STATE(STATE_UNKNOWN_FLOOR)          \
        STATE(STATE_IDLE)                   \
        STATE(STATE_MOVING)                 \
        STATE(STATE_DOOR_OPEN)              \
        STATE(STATE_EMERGENCY_STOP)         \
        STATE(STATE_EMERGENCY_STOP_FLOOR)   \
        STATE(STATE_EMERGENCY_STOP_NOWHERE)

/**
 * @brief All events recognized by the state machine
 * 
 * @note EVENT_FLOOR1-4 placed in beginning to use numerical value
 * @note EVENT_ENTRY and EVENT_EXIT for internal FSM use only
 */
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

/*! Generates enumerated values */
#define GENERATE_ENUM(ENUM) ENUM,

/*! Generates stringified values */
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

/**
 * @brief Retruns current floor of elevator
 * 
 * @return int floor number, 0 indexed
 */
int fsm_get_floor();


#endif // FSM_H