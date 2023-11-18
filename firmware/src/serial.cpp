#include "serial.h"

// Function to set up serial communication with a specified baud rate
void serial_setup(unsigned long baudrate)
{
    // Begin serial communication with the specified baud rate
    Serial.begin(baudrate);
}

// Function to read data from the serial interface and interpret it as a command
Data_in_s serial_read_data(void)
{
    // Initialize data variable with NO_CMD (no command)
    Data_in_s data = NO_CMD;

    // Check if there is data available in the serial buffer
    if(Serial.available() > 0)
    {
        // Read the first character from the serial buffer
        char cmd = Serial.read();

        // Interpret the received character and set the appropriate command
        if(cmd == '0')
        {
            data = OPEN_CMD;
        }
        else if(cmd == '1')
        {
            data = CLOSE_CMD;
        }
    }

    // Return the interpreted command
    return data;
}

// Function to write data to the serial interface
void serial_write_data(Data_out_s* data)
{
    // Create a character array to store the portal length as a string
    char portal_length_str[10];

    // Convert the portal length to a string with 1 digit before and 2 digits after the decimal point
    dtostrf(data->portal_length, 1, 2, portal_length_str);

    // Send the formatted data over serial communication
    Serial.print(portal_length_str);
    Serial.print(",");
    Serial.print(data->open_sw_state);
    Serial.print(",");
    Serial.print(data->close_sw_state);
    Serial.print(",");
    Serial.print(data->user_sw_state);
    Serial.println();  // Print a newline character to indicate the end of the data
}
