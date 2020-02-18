/**
 * @file
 * @brief Module for taking elevator orders and creating a queue.
 *
 * */


#ifndef ELEVATOR_CALL_H
#define ELEVATOR_CALL_H
#include <stdbool.h>
#include <limits.h>

typedef struct FloorRequests {
    int floor;
    bool direction_up;
} FloorRequest;

/**
 * @brief Places an order for floor @p floor with direction @p direction_up 
 * in @p elevator_queue.
 *
 * @param floor Floor we place an order for.
 * @param direction_up Tell the queue system wether we are going up or down.
 * @param current_floor Tells the queue system where the elevator currently is.
 * @param current_direction_up Tells the queue system which direction the elevator currently is going.
 * 
 * @return @p void function that changes the internal elevator_queue based on priority from direction and floor.
 */
void add_request(int floor, bool direction_up, int current_floor, bool current_direction_up);

/**
 * @brief Removes an order for floor @p floor with direction @p direction_up 
 * in @p elevator_queue.
 *
 * @param floor Floor we want to remove order from. 
 * @param direction_up Tell the queue system wether we are going up or down.
 */
void remove_request(int floor, bool direction_up);

#endif
