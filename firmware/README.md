# Gate Control System

This Arduino project controls a gate system with the ability to open, close, and monitor its status. The system utilizes a stepper motor for precise gate movement, serial communication for command input/output, and various sensors and indicators for state monitoring.

## Components

### Gate Control (`gate.h`)

- `gate_setup()`: Initialize gate components such as switches, LEDs, and the stepper motor.
- `gate_get_status()`: Retrieve the current status of the gate, including portal length and switch states.
- `gate_open()`: Open the gate and return the state of the open switch after the movement.
- `gate_close()`: Close the gate, set the portal length to zero, and return the state of the close switch after the movement.
- `gate_light_on()`: Turn on the gate light.
- `gate_light_off()`: Turn off the gate light.

### Serial Communication (`serial.h`)

- `serial_setup(unsigned long baudrate)`: Initialize serial communication with a specified baud rate.
- `serial_read_data()`: Read data from the serial interface and interpret it as a command.
- `serial_write_data(Data_out_s* data)`: Write gate status data to the serial interface.

### Stepper Motor Control (`stepper.h`)

- `stepper_setup()`: Initialize stepper motor pins.
- `stepper_write(unsigned dir)`: Control the stepper motor movement based on the specified direction.
- `stepper_get_portal_length()`: Get the current portal length.
- `stepper_set_portal_length(float new_portal_length)`: Set a new portal length.

## Pin Configuration

### Gate Control

- `OPEN_SW_PIN`: Pin for the open switch.
- `CLOSE_SW_PIN`: Pin for the close switch.
- `LED_PIN`: Pin for the gate light.
- `USER_SW_PIN`: Pin for the user switch.

### Stepper Motor

- `DIR_PIN`: Pin for the stepper motor direction.
- `STEP_PIN`: Pin for the stepper motor step.
- `EN_PIN`: Pin to enable/disable the stepper motor.

## Constants

### Stepper Motor

- `STEPS_PER_MM`: Number of steps per millimeter.
- `MICRO_STEPS`: Microsteps for smoother motor movement.

## Usage

1. Connect the specified components to the Arduino board according to the provided pin configurations.
2. Upload the provided code to the Arduino.
3. Monitor and control the gate using the specified serial commands.
