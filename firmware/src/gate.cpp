#include "gate.h"

void gate_setup()
{
    pinMode(OPEN_SW_PIN, INPUT);
    pinMode(CLOSE_SW_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(USER_SW_PIN, INPUT);

    stepper_setup();
}

Gate_status_s gate_get_status()
{
    Gate_status_s gate_status;

    gate_status.close_sw_state = '0' + !digitalRead(CLOSE_SW_PIN);
    gate_status.open_sw_state = '0' + !digitalRead(OPEN_SW_PIN);
    gate_status.user_sw_state = '0' + digitalRead(USER_SW_PIN);
    gate_status.portal_length = stepper_get_portal_length();

    return gate_status;
}

char gate_open()
{
    Gate_status_s gate_status;

    stepper_write(1);
    gate_status = gate_get_status();
    return gate_status.open_sw_state;
}

char gate_close()
{
    Gate_status_s gate_status;

    stepper_write(0);
    gate_status = gate_get_status();
    return gate_status.close_sw_state;
}

void gate_light_on()
{
    digitalWrite(LED_PIN, HIGH);
}
void gate_light_off()
{
    digitalWrite(LED_PIN, LOW);
}