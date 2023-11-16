#!/usr/bin/env -P /usr/bin:/usr/local/bin python3 -B
# coding: utf-8

#
#  main.py
#  EtatVersTexte version 1.0
#  Created by Ingenuity i/o on 2023/11/16
#

import sys
import ingescape as igs


# inputs
def input_callback(iop_type, name, value_type, value, my_data):
    if name == "estOuvert":
        if value:
            igs.output_set_string('titre', "Portail ouvert")
    elif name == "estFerme":
        if value:
            igs.output_set_string('titre', "Portail fermé")


if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("usage: python3 main.py agent_name network_device port")
        devices = igs.net_devices_list()
        print("Please restart with one of these devices as network_device argument:")
        for device in devices:
            print(f" {device}")
        exit(0)

    igs.agent_set_name(sys.argv[1])
    igs.definition_set_version("1.0")
    igs.log_set_console(True)
    igs.log_set_file(True, None)
    igs.set_command_line(sys.executable + " " + " ".join(sys.argv))

    igs.input_create("estOuvert", igs.BOOL_T, None)
    igs.input_create("estFerme", igs.BOOL_T, None)

    igs.output_create("titre", igs.STRING_T, None)

    igs.observe_input("estOuvert", input_callback, None)
    igs.observe_input("estFerme", input_callback, None)

    igs.start_with_device(sys.argv[2], int(sys.argv[3]))

    input()

    igs.stop()
