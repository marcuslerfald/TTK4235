/**
 * @file queue.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-02-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef QUEUE_H
#define QUEUE_H

typedef enum {
    DIRECTION_UP,
    DIRECTION_DOWN
} direction_t;

/**
 * @brief Adds a request to the queue
 * 
 * @param floor                 Floor to travel to. 0 indexed, "Floor 1" is 0
 * @param requested_direction   Requested direction of travel
 */
void queue_add_request(int floor, direction_t requested_direction);

/**
 * @brief Removes a fulfilled request from the queue
 * 
 * @param floor                 Current floor
 * @param requested_direction   Current direction
 */
void queue_remove_request(int floor, direction_t requested_direction);

/**
 * @brief Retrieves the next element in the queue to travel to
 * 
 * @param current_floor     The current floor the elevator is in
 * @param moving_direction  The current direction of travel for the elevator
 * @return int              The floor to travel to, 0 indexed. -1 if no requests in the queue.
 */
int queue_get_next(int current_floor, direction_t moving_direction);

#endif // QUEUE_H
