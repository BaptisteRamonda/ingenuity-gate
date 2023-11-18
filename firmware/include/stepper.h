#ifndef STEPPER__H
#define STEPPER__H

#include <Arduino.h>

#define STEPS_PER_MM 2
#define MICRO_STEPS 8

#define DIR_PIN 4
#define STEP_PIN 3
#define EN_PIN 2

void stepper_setup();
void stepper_write(unsigned dir);
float stepper_get_portal_length();
void stepper_set_portal_length(float new_portal_length);

#endif