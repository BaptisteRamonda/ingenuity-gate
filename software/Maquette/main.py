#!/usr/bin/env -P /usr/bin:/usr/local/bin python3 -B
# coding: utf-8

#
#  main.py
#  Maquette version 1.0
#  Created by Ingenuity i/o on 2023/11/15
#

from threading import Lock
import sys
import ingescape as igs
import serial
import time

# Global variables
port = 'COM5'
baudrate = 115200
ser = None  # Serial port object

lock = None  # Thread lock for synchronization


def input_callback(iop_type, name, value_type, value, my_data):
    """
    Callback function for handling changes in the 'commandePortail' input.

    Parameters:
    - iop_type (int): Type of input or output (e.g., igs.INPUT_T, igs.OUTPUT_T).
    - name (str): Name of the input or output.
    - value_type (int): Data type of the value (e.g., igs.INTEGER_T, igs.BOOL_T).
    - value: The new value of the input or output.
    - my_data: Additional data (unused in this example).

    This function is called when the 'commandePortail' input changes, and it sends the new value to the serial port.
    """
    with lock:
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        ser.write(str(value).encode())


if __name__ == "__main__":
    # Check if command line arguments are provided
    if len(sys.argv) < 4:
        print("usage: python3 main.py agent_name network_device port")
        devices = igs.net_devices_list()
        print("Please restart with one of these devices as network_device argument:")
        for device in devices:
            print(f" {device}")
        exit(0)

    # Initialize the serial port
    ser = serial.Serial(port, baudrate)
    time.sleep(2)  # Allow time for the serial connection to stabilize

    # Initialize the thread lock
    lock = Lock()

    # Configure the INGESCape agent
    igs.agent_set_name(sys.argv[1])
    igs.definition_set_version("1.0")
    igs.log_set_console(True)
    igs.log_set_file(True, None)
    igs.set_command_line(sys.executable + " " + " ".join(sys.argv))

    # Define input and output variables
    igs.input_create("commandePortail", igs.INTEGER_T, None)
    igs.output_create("estOuvert", igs.BOOL_T, None)
    igs.output_create("estFerme", igs.BOOL_T, None)
    igs.output_create("estAppele", igs.BOOL_T, None)
    igs.output_create("distanceParcourue", igs.DOUBLE_T, None)

    # Set up the callback function for the 'commandePortail' input
    igs.observe_input("commandePortail", input_callback, None)

    # Start the INGESCape agent with the specified network device and port
    igs.start_with_device(sys.argv[2], int(sys.argv[3]))

    # Main loop for reading data from the serial port and updating outputs
    while True:
        with lock:
            line = ser.readline()
            try:
                decoded_line = line.decode()
                datas = decoded_line.strip().split(',')
                if len(datas) == 4:
                    # Update the INGESCape outputs based on the received data
                    igs.output_set_double('distanceParcourue', float(datas[0]))
                    igs.output_set_bool('estOuvert', bool(int(datas[1])))
                    igs.output_set_bool('estFerme', bool(int(datas[2])))
                    igs.output_set_bool('estAppele', bool(int(datas[3])))

            except Exception:
                pass