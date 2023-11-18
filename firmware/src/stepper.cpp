#include "stepper.h"

float portal_length = 0;

void stepper_setup()
{
    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);

    digitalWrite(EN_PIN, HIGH);
}

void stepper_write(unsigned dir)
{
    // activate motor
    digitalWrite(EN_PIN, LOW);
    
    // choose direction
    digitalWrite(DIR_PIN, dir);

    // write the step
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(100);

    // de activate motor
    digitalWrite(EN_PIN, HIGH);

    // update portal_length
    portal_length += (dir * 2.0f - 1.0f) * 1.0f / STEPS_PER_MM / MICRO_STEPS;
}

float stepper_get_portal_length()
{
    return portal_length;
}

void stepper_set_portal_length(float new_portal_length)
{
    portal_length = new_portal_length;
}