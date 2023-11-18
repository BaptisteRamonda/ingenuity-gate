#ifndef GATE__H
#define GATE__H

#include <Arduino.h>
#include "stepper.h"

#define OPEN_SW_PIN 8
#define CLOSE_SW_PIN 9
#define LED_PIN 6
#define USER_SW_PIN 11

typedef struct 
{
    float portal_length;
    char open_sw_state;
    char close_sw_state;
    char user_sw_state;
} Gate_status_s;

void gate_setup();
Gate_status_s gate_get_status();
char gate_open();
char gate_close();
void gate_light_on();
void gate_light_off();

#endif