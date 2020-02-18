#include "elevator_call.h"

int queue_size = 0;
FloorRequest* elevator_queue[0];

void add_request(int floor, bool direction_up, int current_floor, bool current_direction_up){
    bool order_in_queue = false;
    for (int i = 0; i < queue_size; i++) {
        if (elevator_queue[i]->floor == floor && elevator_queue[i]->direction_up == direction_up) {
            order_in_queue = true;
        }
    }
    if (order_in_queue == false) {
        int i = 0;
        while (i < queue_size){
            if (elevator_queue[i]->direction_up != direction_up) {
                i++;
                continue;
            }
            if (direction_up == true && elevator_queue[i]->floor < floor){
                i++;
            }
            else if (direction_up == false && elevator_queue[i]->floor > floor) {
                i++;
            }
        }
        //TODO insert request here
    }
}