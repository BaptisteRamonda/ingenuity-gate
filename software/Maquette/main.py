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

# global variables
port = 'COM5'
baudrate = 115200
ser = None

lock = None


# inputs
def input_callback(iop_type, name, value_type, value, my_data):
    with lock:
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        ser.write(str(value).encode())


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("usage: python3 main.py agent_name network_device port")
        devices = igs.net_devices_list()
        print("Please restart with one of these devices as network_device argument:")
        for device in devices:
            print(f" {device}")
        exit(0)

    # start serial
    ser = serial.Serial(port, baudrate)
    time.sleep(2)

    # init mutex
    lock = Lock()

    igs.agent_set_name(sys.argv[1])
    igs.definition_set_version("1.0")
    igs.log_set_console(True)
    igs.log_set_file(True, None)
    igs.set_command_line(sys.executable + " " + " ".join(sys.argv))

    igs.input_create("commandePortail", igs.INTEGER_T, None)

    igs.output_create("estOuvert", igs.BOOL_T, None)
    igs.output_create("estFerme", igs.BOOL_T, None)
    igs.output_create("estAppele", igs.BOOL_T, None)
    igs.output_create("distanceParcourue", igs.DOUBLE_T, None)

    igs.observe_input("commandePortail", input_callback, None)

    igs.start_with_device(sys.argv[2], int(sys.argv[3]))

    while True:
        with lock:
            line = ser.readline()
            try:
                decoded_line = line.decode()
                datas = decoded_line.strip().split(',')
                if len(datas) == 4:
                    igs.output_set_double('distanceParcourue', float(datas[0]))
                    igs.output_set_bool('estOuvert', bool(int(datas[1])))
                    igs.output_set_bool('estFerme', bool(int(datas[2])))
                    igs.output_set_bool('estAppele', bool(int(datas[3])))
            except Exception:
                pass
    input()

    igs.stop()
