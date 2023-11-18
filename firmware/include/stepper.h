#ifndef STEPPER__H
#define STEPPER__H

#include <Arduino.h>

// Constants for stepper motor configuration
#define STEPS_PER_MM 2
#define MICRO_STEPS 8

// Pin definitions for stepper motor control
#define DIR_PIN 4
#define STEP_PIN 3
#define EN_PIN 2

// Function declarations for stepper motor control
void stepper_setup();                           // Initialize stepper motor pins
void stepper_write(unsigned dir);               // Control stepper motor movement
float stepper_get_portal_length();              // Get the current portal length
void stepper_set_portal_length(float new_portal_length);  // Set a new portal length

#endif
