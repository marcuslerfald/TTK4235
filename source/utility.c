#include "utility.h"
#include "hardware.h"


bool utility_on_floor()
{
    bool on_floor = false;

    for(int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) 
    {
        if(hardware_read_floor_sensor(floor))
        {
            on_floor = true;
        }
    }

    return on_floor;
}

