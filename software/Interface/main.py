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
    return wasCalled


def setWasCalled(value):
    wasCalled = value


# inputs
def input_callback(iop_type, name, value_type, value, my_data):
    if not getWasCalled():
        igs.service_call("Whiteboard", "addShape", ("rectangle", 0., 0., 100., 1500., "black", "black", 3.), "")
        igs.service_call("Whiteboard", "addShape", ("rectangle", 1040., 0., 100., 1500., "black", "black", 3.), "")
        print(iop_type, name, value_type, value, my_data)
        setWasCalled(True)


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("usage: python3 main.py agent_name network_device port")
        devices = igs.net_devices_list()
        print("Please restart with one of these devices as network_device argument:")
        for device in devices:
            print(f" {device}")
        exit(0)

    wasCalled = False

    igs.agent_set_name(sys.argv[1])
    igs.definition_set_version("1.0")
    igs.log_set_console(True)
    igs.log_set_file(True, None)
    igs.set_command_line(sys.executable + " " + " ".join(sys.argv))

    igs.input_create("estOuvert", igs.BOOL_T, None)
    igs.input_create("estFerme", igs.BOOL_T, None)
    igs.input_create("estAppele", igs.BOOL_T, None)
    igs.input_create("distanceParcourue", igs.DOUBLE_T, None)

    igs.observe_input("estOuvert", input_callback, None)
    igs.observe_input("estFerme", input_callback, None)
    igs.observe_input("estAppele", input_callback, None)
    igs.observe_input("distanceParcourue", input_callback, None)

    igs.start_with_device(sys.argv[2], int(sys.argv[3]))

    input()

    igs.stop()
