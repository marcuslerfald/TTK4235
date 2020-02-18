#include "queue.h"

#include <stdbool.h>

#define QUEUE_LENGTH 4

static bool queue_up[QUEUE_LENGTH] = { false };
static bool queue_down[QUEUE_LENGTH] = { false }; 

void queue_add_request(int floor, direction_t requested_direction)
{
    if(requested_direction == DIRECTION_UP)
    {
        queue_up[floor] = true; 
    }
    else
    {
        queue_down[floor] = true; 
    }
}

void queue_remove_request(int floor, direction_t requested_direction)
{
    if(requested_direction == DIRECTION_UP)
    {
        queue_up[floor] = false; 
    }
    else
    {
        queue_down[floor] = false; 
    }
}

int queue_get_next(int current_floor, direction_t moving_direction)
{
    if(moving_direction == DIRECTION_UP)
    {
        for(int i = current_floor; i < QUEUE_LENGTH; i++)
        {
            if(queue_up[i])
            {
                return i;
            }
        }
        for(int i = QUEUE_LENGTH - 1; i > -1; i--)
        {
            if(queue_down[i])
            {
                return i;
            } 
        }
        for(int i = 0; i < current_floor; i++)
        {
            if(queue_up[i])
            {
                return i;
            } 
        } 
    }
    else
    {
        for(int i = current_floor; i > -1; i--)
        {
            if(queue_down[i])
            {
                return i;
            } 
        } 
        for(int i = 0; i < QUEUE_LENGTH; i++)
        {
            if(queue_up[i])
            {
                return i;
            } 
        } 
        for(int i = QUEUE_LENGTH - 1; i > current_floor; i--)
        {
            if(queue_down[i])
            {
                return i;
            } 
        } 
    }

    return QUEUE_EMPTY;
    
}

void queue_clear()
{
    for(int i = 0; i < QUEUE_LENGTH; i++)
    {
        queue_up[i] = false;
        queue_down[i] = false; 
    } 
}