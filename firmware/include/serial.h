#ifndef SERIAL__H
#define SERIAL__H

#include <Arduino.h>
#include "gate.h"

#define OPEN_CMD 0
#define CLOSE_CMD 1
#define NO_CMD 2

typedef char Data_in_s;

typedef Gate_status_s Data_out_s;

void serial_setup(unsigned long baudrate);
Data_in_s serial_read_data(void);
void serial_write_data(Data_out_s* data);

#endif