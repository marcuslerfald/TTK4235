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
#include "timer.h"
#include "queue.h"

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
    direction_t direction;
};

// FSM functions
static void fsm_transition(state_function new_state);
static bool elevator_on_floor();

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


static bool elevator_on_floor()
{
    for(int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) 
    {
        if(hardware_read_floor_sensor(floor))
        {
            return true;
        }
    }

    return false;
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
            queue_clear();
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

        case EVENT_STOP_BUTTON_PRESSED:
            fsm_transition(&state_emergency_stop);
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

            int requested_floor = queue_get_next(fsm.current_floor, fsm.direction);
            
            // Determine direction to move
            if(requested_floor > fsm.current_floor)
            {
                hardware_command_movement(HARDWARE_MOVEMENT_UP);
                fsm.direction = DIRECTION_UP;
            }
            else if(requested_floor < fsm.current_floor)
            {
                hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
                fsm.direction = DIRECTION_DOWN;
            }
            else
            {
                fsm_transition(&state_door_open);
            } 
        break;

        case EVENT_STOP_BUTTON_PRESSED:
            fsm_transition(&state_emergency_stop);
        break;

        // Implicit falltrough intentional,
        // handled the same way
        case EVENT_FLOOR_1:
        case EVENT_FLOOR_2:
        case EVENT_FLOOR_3:
        case EVENT_FLOOR_4:
            fsm.current_floor = (int)event;

            // Check if should stop
            int requested_floor = queue_get_next(fsm.current_floor, fsm.direction);

            if(fsm.current_floor == requested_floor)
            {
                queue_remove_request(fsm.current_floor, fsm.direction);
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
        
        case EVENT_STOP_BUTTON_PRESSED:
            fsm_transition(&state_emergency_stop);
        break;

        case EVENT_OBSTRUCTION_ACTIVE:
            timer_restart();
        break;

        case EVENT_TIMER_TIMEOUT:
            // Check if items in queue
            if(queue_get_next(fsm.current_floor, fsm.direction) != QUEUE_EMPTY)
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

            if(elevator_on_floor())
            {
                fsm_transition(&state_emergency_stop_floor);
            }
            else
            {
                fsm_transition(&state_emergency_stop_nowhere);
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
            queue_clear();
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
            queue_clear();
        break;

        default:
        break;
    }
}
