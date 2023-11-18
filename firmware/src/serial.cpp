#include "serial.h"

void serial_setup(unsigned long baudrate)
{
    Serial.begin(baudrate);

}

Data_in_s serial_read_data(void)
{
    Data_in_s data;

    data = NO_CMD;

    if(Serial.available() > 0)
    {
        char cmd = Serial.read();
        if(cmd == '0')
        {
            data = OPEN_CMD;
        }
        else if(cmd == '1')
        {
            data = CLOSE_CMD;
        }
    }

    return data;
}

void serial_write_data(Data_out_s* data)
{
    char portal_length_str[10];

    dtostrf(data->portal_length, 1, 2, portal_length_str);

    Serial.print(portal_length_str);
    Serial.print(",");
    Serial.print(data->open_sw_state);
    Serial.print(",");
    Serial.print(data->close_sw_state);
    Serial.print(",");
    Serial.print(data->user_sw_state);
    Serial.println();
}