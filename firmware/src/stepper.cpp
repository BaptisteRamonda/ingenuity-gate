#include "stepper.h"

// Global variable to store the portal length
float portal_length = 0;

// Function to set up the stepper motor pins
void stepper_setup()
{
    // Set pin modes for direction, step, and enable pins
    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);

    // Disable the stepper motor by default
    digitalWrite(EN_PIN, HIGH);
}

// Function to control the stepper motor movement
void stepper_write(unsigned dir)
{
    // Activate the stepper motor by lowering the enable pin
    digitalWrite(EN_PIN, LOW);

    // Set the direction of the stepper motor
    digitalWrite(DIR_PIN, dir);

    // Write a step to the stepper motor
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(100);

    // Deactivate the stepper motor by raising the enable pin
    digitalWrite(EN_PIN, HIGH);

    // Update the portal length based on the stepper motor movement
    portal_length += (dir * 2.0f - 1.0f) * 1.0f / STEPS_PER_MM / MICRO_STEPS;
}

// Function to get the current portal length
float stepper_get_portal_length()
{
    return portal_length;
}

// Function to set a new portal length
void stepper_set_portal_length(float new_portal_length)
{
    portal_length = new_portal_length;
}
