#include "gate.h"

// Function to initialize the gate setup
void gate_setup()
{
    // Set the pin modes for different components
    pinMode(OPEN_SW_PIN, INPUT);
    pinMode(CLOSE_SW_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(USER_SW_PIN, INPUT);

    // Initialize the stepper motor setup
    stepper_setup();
}

// Function to get the current status of the gate
Gate_status_s gate_get_status()
{
    // Create a structure to hold the gate status
    Gate_status_s gate_status;

    // Read the state of the close switch and convert it to a character
    gate_status.close_sw_state = '0' + !digitalRead(CLOSE_SW_PIN);

    // Read the state of the open switch and convert it to a character
    gate_status.open_sw_state = '0' + !digitalRead(OPEN_SW_PIN);

    // Read the state of the user switch
    gate_status.user_sw_state = '0' + digitalRead(USER_SW_PIN);

    // Get the portal length from the stepper motor
    gate_status.portal_length = stepper_get_portal_length();

    // Return the gate status
    return gate_status;
}

// Function to open the gate
char gate_open()
{
    // Create a structure to hold the gate status
    Gate_status_s gate_status;

    // Activate the stepper motor to open the gate
    stepper_write(1);

    // Get the current gate status
    gate_status = gate_get_status();

    // Return the state of the open switch after the gate movement
    return gate_status.open_sw_state;
}

// Function to close the gate
char gate_close()
{
    // Create a structure to hold the gate status
    Gate_status_s gate_status;

    // Activate the stepper motor to close the gate
    stepper_write(0);

    // Get the current gate status
    gate_status = gate_get_status();

    // Return the state of the close switch after the gate movement
    return gate_status.close_sw_state;
}

// Function to turn on the gate light
void gate_light_on()
{
    // Set the LED pin to HIGH to turn on the light
    digitalWrite(LED_PIN, HIGH);
}

// Function to turn off the gate light
void gate_light_off()
{
    // Set the LED pin to LOW to turn off the light
    digitalWrite(LED_PIN, LOW);
}