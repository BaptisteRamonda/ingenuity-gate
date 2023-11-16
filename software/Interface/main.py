#!/usr/bin/env -P /usr/bin:/usr/local/bin python3 -B
# coding: utf-8

#
#  main.py
#  Interface version 1.0
#  Created by Ingenuity i/o on 2023/11/15
#

import sys
import ingescape as igs

wasCalled = False


def getWasCalled():
    """
    Get the value of the 'wasCalled' flag.

    Returns:
    bool: The current value of the 'wasCalled' flag.
    """
    return wasCalled


def setWasCalled(value):
    """
    Set the value of the 'wasCalled' flag.

    Parameters:
    - value (bool): The new value to set for the 'wasCalled' flag.
    """
    wasCalled = value


# inputs
def input_callback(iop_type, name, value_type, value, my_data):
    """
    Callback function for handling changes in the specified inputs.

    Parameters:
    - iop_type (int): Type of input or output (e.g., igs.INPUT_T, igs.OUTPUT_T).
    - name (str): Name of the input or output.
    - value_type (int): Data type of the value (e.g., igs.BOOL_T, igs.DOUBLE_T).
    - value: The new value of the input or output.
    - my_data: Additional data (unused in this example).

    This function is called when any of the observed inputs change, and it triggers specific actions.
    """
    if not getWasCalled():
        # Perform actions only if 'wasCalled' is False
        igs.service_call("Whiteboard", "addShape", ("rectangle", 0., 0., 100., 1500., "black", "black", 3.), "")
        igs.service_call("Whiteboard", "addShape", ("rectangle", 1040., 0., 100., 1500., "black", "black", 3.), "")
        print(iop_type, name, value_type, value, my_data)
        setWasCalled(True)


if __name__ == "__main__":
    # Check if command line arguments are provided
    if len(sys.argv) < 4:
        print("usage: python3 main.py agent_name network_device port")
        devices = igs.net_devices_list()
        print("Please restart with one of these devices as network_device argument:")
        for device in devices:
            print(f" {device}")
        exit(0)

    wasCalled = False  # Reset the 'wasCalled' flag

    # Configure the INGESCape agent
    igs.agent_set_name(sys.argv[1])
    igs.definition_set_version("1.0")
    igs.log_set_console(True)
    igs.log_set_file(True, None)
    igs.set_command_line(sys.executable + " " + " ".join(sys.argv))

    # Define observed input variables
    igs.input_create("estOuvert", igs.BOOL_T, None)
    igs.input_create("estFerme", igs.BOOL_T, None)
    igs.input_create("estAppele", igs.BOOL_T, None)
    igs.input_create("distanceParcourue", igs.DOUBLE_T, None)

    # Set up the callback function for the observed inputs
    igs.observe_input("estOuvert", input_callback, None)
    igs.observe_input("estFerme", input_callback, None)
    igs.observe_input("estAppele", input_callback, None)
    igs.observe_input("distanceParcourue", input_callback, None)

    # Start the INGESCape agent with the specified network device and port
    igs.start_with_device(sys.argv[2], int(sys.argv[3]))

    input()  # This line seems unreachable and may be a mistake in the code

    # Stop the INGESCape agent
    igs.stop()
