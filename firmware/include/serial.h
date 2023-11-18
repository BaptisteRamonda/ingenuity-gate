#ifndef SERIAL__H
#define SERIAL__H

#include <Arduino.h>
#include "gate.h"

// Definitions for command types
#define OPEN_CMD 0
#define CLOSE_CMD 1
#define NO_CMD 2

// Input data type for serial communication
typedef char Data_in_s;

// Output data type for serial communication
typedef Gate_status_s Data_out_s;

// Function declarations for serial communication
void serial_setup(unsigned long baudrate);   // Initialize serial communication
Data_in_s serial_read_data(void);             // Read data from the serial interface
void serial_write_data(Data_out_s* data);    // Write data to the serial interface

#endif
