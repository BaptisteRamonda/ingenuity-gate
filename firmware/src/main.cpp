#include <Arduino.h>
#include "serial.h"
#include "gate.h"
#include "stepper.h"

#define BAUDRATE 115200

void setup() 
{
    // init serial
    serial_setup(BAUDRATE);

    // init gate
    gate_setup();
}

void loop() 
{
    // vars
    Data_in_s new_cmd;
    Data_out_s data_out;
    Gate_status_s gate_satus;
    static Data_in_s current_cmd = NO_CMD;

    // try to find a command and update the current command (no blocking function)
    new_cmd = serial_read_data();
    if(new_cmd == OPEN_CMD || new_cmd == CLOSE_CMD)
    {
        gate_light_on();
        current_cmd = new_cmd;
    }

    // if there is a command, execute it (no blocking function)
    if(current_cmd == OPEN_CMD)
    {
        if(gate_open() == '1')
        {
            current_cmd = NO_CMD;
            gate_light_off();
        }
    }
    else if(current_cmd == CLOSE_CMD)
    {
        if(gate_close() == '1')
        {
            stepper_set_portal_length(0.0f);
            gate_light_off();
            current_cmd = NO_CMD;
        }
    }


    // send gate state
    gate_satus = gate_get_status();
    serial_write_data(&gate_satus);
}