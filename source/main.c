#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#include "hardware.h"

#include "fsm.h"
#include "queue.h"
#include "timer.h"

static void sigint_handler(int sig){
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}

int main(){
    fsm_init();
    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    timer_restart();

    fsm_dispatch(EVENT_HW_INIT_DONE);

    while(1){
        /* Read floor sensors */
        for(int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) 
        {
            if(hardware_read_floor_sensor(floor))
            {
                // Update floor indicator
                hardware_command_floor_indicator_on(floor);
                
                // Notify FSM
                fsm_dispatch((fsm_event_t) floor);
            }  
        }
        
        /* Check stop button */
        if(hardware_read_stop_signal())
        {
            fsm_dispatch(EVENT_STOP_BUTTON_PRESSED);

            // continue to prevent from taking orders etc when stopped
            continue;
        }
        else
        {
            fsm_dispatch(EVENT_STOP_BUTTON_RELEASED);
        }

        /* Checking for obstructions */
        if(hardware_read_obstruction_signal())
        {
            fsm_dispatch(EVENT_OBSTRUCTION_ACTIVE);
        }

        /* Polling buttons and adding requests */
        for(int f = 0; f < HARDWARE_NUMBER_OF_FLOORS; f++){
            /* Internal orders */
            if(hardware_read_order(f, HARDWARE_ORDER_INSIDE))
            {
                int delta_floor = f - fsm_get_floor();

                // Determine if up or down request
                if(delta_floor < 0)
                {
                    queue_add_request(f, DIRECTION_DOWN);
                }
                else
                {
                    queue_add_request(f, DIRECTION_UP);
                }

                hardware_command_order_light(f, HARDWARE_ORDER_INSIDE, true);

                fsm_dispatch(EVENT_QUEUE_NOT_EMPTY);
            }

            /* Orders going up */
            if(hardware_read_order(f, HARDWARE_ORDER_UP))
            {
                queue_add_request(f, DIRECTION_UP);

                hardware_command_order_light(f, HARDWARE_ORDER_UP, true);
                
                fsm_dispatch(EVENT_QUEUE_NOT_EMPTY);
            }

            /* Orders going down */
            if(hardware_read_order(f, HARDWARE_ORDER_DOWN))
            {
                queue_add_request(f, DIRECTION_DOWN);

                hardware_command_order_light(f, HARDWARE_ORDER_DOWN, true);
                
                fsm_dispatch(EVENT_QUEUE_NOT_EMPTY);
            }
        }

        /* Check if timer timeout */
        if(timer_check_timeout())
        {
            fsm_dispatch(EVENT_TIMER_TIMEOUT);
        } 
    }

    return 0;
}
