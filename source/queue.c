#include "queue.h"

#include <stdbool.h>

#define QUEUE_LENGTH 4

static bool m_queue_up[QUEUE_LENGTH] = { false };
static bool m_queue_down[QUEUE_LENGTH] = { false }; 

void queue_add_request(int floor, direction_t requested_direction)
{
    if(requested_direction == DIRECTION_UP)
    {
        m_queue_up[floor] = true; 
    }
    else
    {
        m_queue_down[floor] = true; 
    }
}

void queue_remove_request(int floor, direction_t requested_direction)
{
    if(requested_direction == DIRECTION_UP)
    {
        m_queue_up[floor] = false; 
    }
    else
    {
        m_queue_down[floor] = false; 
    }
}

void queue_get_next(int current_floor, direction_t moving_direction, int *next_floor, direction_t *next_direction)
{
    if(moving_direction == DIRECTION_UP)
    {
        for(int i = current_floor; i < QUEUE_LENGTH; i++)
        {
            if(m_queue_up[i])
            {
                *next_floor = i;
                *next_direction = DIRECTION_UP;
                return;
            }
        }
        for(int i = QUEUE_LENGTH - 1; i > -1; i--)
        {
            if(m_queue_down[i])
            {
                *next_floor = i;
                *next_direction = DIRECTION_DOWN;
                return;
            } 
        }
        for(int i = 0; i < current_floor; i++)
        {
            if(m_queue_up[i])
            {
                *next_floor = i;
                *next_direction = DIRECTION_UP;
                return;
            } 
        } 
    }
    else
    {
        for(int i = current_floor; i > -1; i--)
        {
            if(m_queue_down[i])
            {
                *next_floor = i;
                *next_direction = DIRECTION_DOWN;
                return;
            } 
        } 
        for(int i = 0; i < QUEUE_LENGTH; i++)
        {
            if(m_queue_up[i])
            {
                *next_floor = i;
                *next_direction = DIRECTION_UP;
                return;
            } 
        } 
        for(int i = QUEUE_LENGTH - 1; i > current_floor; i--)
        {
            if(m_queue_down[i])
            {
                *next_floor = i;
                *next_direction = DIRECTION_DOWN;
                return;
            } 
        } 
    }

    *next_floor = QUEUE_EMPTY;
    return;
}

void queue_clear()
{
    for(int i = 0; i < QUEUE_LENGTH; i++)
    {
        m_queue_up[i] = false;
        m_queue_down[i] = false; 
    } 
}