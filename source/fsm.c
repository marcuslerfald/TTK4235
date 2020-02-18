/**
 * @file fsm.c
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

#include "fsm.h"

#include <stdbool.h>
#include <stdio.h>

#include "hardware.h"

#include "utility.h"


// Strings for both events and states, for debugging purposes
static const char *EVENT_STRING[] = {
    FOREACH_EVENT(GENERATE_STRING)
};

static const char *STATE_STRING[] = { 
    FOREACH_STATE(GENERATE_STRING)
};

// Typedefs
typedef struct fsm_t fsm_t;

typedef void (*state_function)(fsm_event_t event);

struct fsm_t {
    state_function state;
    fsm_state_t state_name;
    int current_floor;
    fsm_direction_t direction;
};

// FSM functions
static void fsm_transition(state_function new_state);

// State functions
static void state_init                  (fsm_event_t event);
static void state_unknown_floor         (fsm_event_t event);
static void state_idle                  (fsm_event_t event);
static void state_moving                (fsm_event_t event);
static void state_door_open             (fsm_event_t event);
static void state_emergency_stop        (fsm_event_t event);
static void state_emergency_stop_nowhere(fsm_event_t event);
static void state_emergency_stop_floor  (fsm_event_t event);

// FSM Instance
static fsm_t fsm;

// 
void fsm_init()
{
    fsm.state = &state_init;
}

void fsm_dispatch(fsm_event_t event)
{
    //printf("%s\n", EVENT_STRING[event]);
    (fsm.state)(event);
}

fsm_state_t fsm_get_state()
{
    return fsm.state_name;
}

// FSM functions
static void fsm_transition(state_function new_state)
{
    fsm_dispatch(EVENT_EXIT);

    fsm.state = new_state;

    fsm_dispatch(EVENT_ENTRY);

    printf("State transition to : %s\n", STATE_STRING[fsm_get_state()]);
}

// State Functions
static void state_init(fsm_event_t event)
{
    switch(event)
    {
        case EVENT_ENTRY:
            fsm.state_name = STATE_INIT;
        break;
        case EVENT_HW_INIT_DONE:
            fsm_transition(&state_unknown_floor);
        break;

        case EVENT_EXIT:

        break;

        default:
        break;
    }
}

static void state_unknown_floor(fsm_event_t event)
{
    switch(event)
    {
        case EVENT_ENTRY:
            fsm.state_name = STATE_UNKNOWN_FLOOR;
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
        break;

        // Implicit falltrough intentional,
        // handled the same way
        case EVENT_FLOOR_1:
        case EVENT_FLOOR_2:
        case EVENT_FLOOR_3:
        case EVENT_FLOOR_4:
            fsm.current_floor = (int)event;
            fsm_transition(&state_idle);
        break;

        case EVENT_EXIT:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
        break;

        default:
        break;
    }
}

static void state_idle(fsm_event_t event)
{
    switch(event)
    {
        case EVENT_ENTRY:
            fsm.state_name = STATE_IDLE;
        break;

        case EVENT_QUEUE_NOT_EMPTY:
            fsm_transition(&state_moving);
        break;

        case EVENT_EXIT:

        break;

        default:
        break;
    }
}

static void state_moving(fsm_event_t event)
{
    switch(event)
    {
        case EVENT_ENTRY:
            fsm.state_name = STATE_MOVING;

            // Determine direction to move
            if()
            {
                hardware_command_movement(HARDWARE_MOVEMENT_UP);
            }
            else
            {
                hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
            }
        break;

        // Implicit falltrough intentional,
        // handled the same way
        case EVENT_FLOOR_1:
        case EVENT_FLOOR_2:
        case EVENT_FLOOR_3:
        case EVENT_FLOOR_4:
            fsm.current_floor = (int)event;

            // Check if should stop
            if(...)
            {
                elevator_call_remove_request();
                fsm_transition(&state_door_open);
            }  

        break;

        case EVENT_EXIT:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
        break;
        default:
        break;
    }
}

static void state_door_open(fsm_event_t event)
{
    switch(event)
    {
        case EVENT_ENTRY:
            fsm.state_name = STATE_DOOR_OPEN;
            hardware_command_door_open(true);
            timer_restart();
        break;

        case EVENT_OBSTRUCTION_ACTIVE:
            timer_restart();
        break;

        case EVENT_TIMER_TIMEOUT:
            // Check if items in queue
            if(queue_not_empty())
            {
                fsm_transition(&state_moving);
            }
            else
            {
                fsm_transition(&state_idle);
            }
        break;

        case EVENT_EXIT:
            hardware_command_door_open(false);
        break;

        default:
        break;
    }
}

static void state_emergency_stop(fsm_event_t event)
{
    switch(event)
    {
        case EVENT_ENTRY:
            fsm.state_name = STATE_EMERGENCY_STOP;
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            // CLEAR ALL REQUESTS
        break;

        case EVENT_STOP_BUTTON_RELEASED:
        { 
            if(utility_on_floor())
            {
                fsm_transition(&state_emergency_stop_floor);
            }
            else
            {
                fsm_transition(&state_emergency_stop_nowhere);
            }
        }   
        break;

        case EVENT_EXIT:

        break;

        default:
        break;
    }
}

static void state_emergency_stop_nowhere(fsm_event_t event)
{
    switch(event)
    {
        case EVENT_ENTRY:
            fsm.state_name = STATE_EMERGENCY_STOP_NOWHERE;
        break;

        case EVENT_STOP_BUTTON_RELEASED:
            fsm_transition(&state_idle);
        break;

        case EVENT_EXIT:

        break;

        default:
        break;
    }
}

static void state_emergency_stop_floor(fsm_event_t event)
{
    switch(event)
    {
        case EVENT_ENTRY:
            fsm.state_name = STATE_EMERGENCY_STOP_FLOOR;
            hardware_command_door_open(true);
        break;

        case EVENT_STOP_BUTTON_RELEASED:
            fsm_transition(&state_door_open);
        break;

        case EVENT_EXIT:
            
        break;

        default:
        break;
    }
}
