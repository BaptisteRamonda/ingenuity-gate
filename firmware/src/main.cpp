#include <Arduino.h>
#include "serial.h"
#include "gate.h"
#include "stepper.h"

// Set the baud rate for serial communication
#define BAUDRATE 115200

// Setup function, runs once at the beginning
void setup() 
{
    // Initialize serial communication
    serial_setup(BAUDRATE);

    // Initialize gate components
    gate_setup();
}

// Main loop function, runs repeatedly
void loop() 
{
    // Variables
    Data_in_s new_cmd;
    Data_out_s data_out;
    Gate_status_s gate_status;
    static Data_in_s current_cmd = NO_CMD;

    // Try to find a command and update the current command (non-blocking function)
    new_cmd = serial_read_data();
    if(new_cmd == OPEN_CMD || new_cmd == CLOSE_CMD)
    {
        // Turn on the gate light if there is a valid command
        gate_light_on();
        current_cmd = new_cmd;
    }

    // If there is a command, execute it (non-blocking function)
    if(current_cmd == OPEN_CMD)
    {
        // Check if the gate is fully open
        if(gate_open() == '1')
        {
            // Reset the current command and turn off the gate light
            current_cmd = NO_CMD;
            gate_light_off();
        }
    }
    else if(current_cmd == CLOSE_CMD)
    {
        // Check if the gate is fully closed
        if(gate_close() == '1')
        {
            // Set the portal length to 0, turn off the gate light, and reset the current command
            stepper_set_portal_length(0.0f);
            gate_light_off();
            current_cmd = NO_CMD;
        }
    }

    // Send gate state over serial communication
    gate_status = gate_get_status();
    serial_write_data(&gate_status);
}
