#ifndef GATE__H
#define GATE__H

#include <Arduino.h>
#include "stepper.h"

// Pin definitions for various components
#define OPEN_SW_PIN 8
#define CLOSE_SW_PIN 9
#define LED_PIN 6
#define USER_SW_PIN 11

// Structure to hold the status of the gate
typedef struct 
{
    float portal_length;  // Current length of the gate portal
    char open_sw_state;   // State of the open switch ('0' or '1')
    char close_sw_state;  // State of the close switch ('0' or '1')
    char user_sw_state;   // State of the user switch ('0' or '1')
} Gate_status_s;

// Function declarations for gate control
void gate_setup();          // Initialize gate components
Gate_status_s gate_get_status();  // Get the current status of the gate
char gate_open();           // Open the gate
char gate_close();          // Close the gate
void gate_light_on();       // Turn on the gate light
void gate_light_off();      // Turn off the gate light

#endif
