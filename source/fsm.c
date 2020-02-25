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

typedef enum {
    POSITION_ABOVE,
    POSITION_BELOW,
    POSITION_ON
} relative_position_t;

struct fsm_t {
    state_function state;
    fsm_state_t state_name;
    int current_floor;
    direction_t direction;
    relative_position_t position;
};

// FSM functions
static void fsm_transition(state_function new_state);
static bool elevator_on_floor();
static void elevator_go_down();
static void elevator_go_up();
static void clear_all_order_lights();

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

int fsm_get_floor()
{
    return fsm.current_floor;
}


// FSM functions
static void fsm_transition(state_function new_state)
{
    fsm_dispatch(EVENT_EXIT);

    printf("State transition from : %s\n", STATE_STRING[fsm_get_state()]);

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

static void elevator_go_down()
{
    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
    fsm.direction = DIRECTION_DOWN;
}

static void elevator_go_up()
{
    hardware_command_movement(HARDWARE_MOVEMENT_UP);
    fsm.direction = DIRECTION_UP;
}

static void clear_all_order_lights()
{
    HardwareOrder order_types[3] = {
        HARDWARE_ORDER_UP,
        HARDWARE_ORDER_INSIDE,
        HARDWARE_ORDER_DOWN
    };

    for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
        for(int i = 0; i < 3; i++){
            HardwareOrder type = order_types[i];
            hardware_command_order_light(f, type, 0);
        }
    }
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
            elevator_go_down();
        break;

        // Implicit falltrough intentional,
        // handled the same way
        case EVENT_FLOOR_1:
        case EVENT_FLOOR_2:
        case EVENT_FLOOR_3:
        case EVENT_FLOOR_4:
            fsm.current_floor = (int)event;

            fsm.position = POSITION_ON;

            fsm_transition(&state_idle);
        break;

        case EVENT_EXIT:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            queue_clear();
            clear_all_order_lights();
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
    int requested_floor;
    direction_t requested_direction;
    queue_get_next(fsm.current_floor, fsm.direction, &requested_floor, &requested_direction);
    
    switch(event)
    {
        case EVENT_ENTRY:
            fsm.state_name = STATE_MOVING;
            
            // Determine direction to move
            if(requested_floor > fsm.current_floor)
            {
                elevator_go_up();

                if(fsm.position == POSITION_ON)
                {
                    fsm.position = POSITION_ABOVE;
                } 
            }
            else if(requested_floor < fsm.current_floor)
            {
                elevator_go_down();

                if(fsm.position == POSITION_ON)
                {
                    fsm.position = POSITION_BELOW;
                }
            }
            else
            {
                if(elevator_on_floor())
                {
                    // Elevator on correct floor

                    // Do nothing, FSM will receive EVENT_FLOOR_X shortly, then open door
                }
                else
                {
                    // Elevator is somewhere inbetween floors, 
                    // due to previous emergency stop
                    
                    if(fsm.position == POSITION_ABOVE)
                    {
                        elevator_go_down();
                    }
                    else
                    {
                        elevator_go_up();
                    }
                }
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

            if(fsm.current_floor == requested_floor)
            {
                // Clear order lights and queue request
                if(requested_direction == DIRECTION_UP)
                {
                    hardware_command_order_light(requested_floor, HARDWARE_ORDER_UP, false);
                    queue_remove_request(fsm.current_floor, DIRECTION_UP);
                }
                else
                {
                    hardware_command_order_light(requested_floor, HARDWARE_ORDER_DOWN, false);
                    queue_remove_request(fsm.current_floor, DIRECTION_DOWN);
                }

                // Always clear internal order
                hardware_command_order_light(requested_floor, HARDWARE_ORDER_INSIDE, false);

                fsm.position = POSITION_ON;

                fsm_transition(&state_door_open);
            }
            else
            {
                if(fsm.direction == DIRECTION_UP)
                {
                    fsm.position = POSITION_ABOVE;
                }
                else
                {
                    fsm.position = POSITION_BELOW;
                }
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
    int requested_floor;
    direction_t requested_direction;
    queue_get_next(fsm.current_floor, fsm.direction, &requested_floor, &requested_direction);

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
            if(requested_floor != QUEUE_EMPTY)
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
            hardware_command_stop_light(true);

            queue_clear();
            clear_all_order_lights();

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
            hardware_command_stop_light(false);
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
            hardware_command_stop_light(false);
        break;

        default:
        break;
    }
}
