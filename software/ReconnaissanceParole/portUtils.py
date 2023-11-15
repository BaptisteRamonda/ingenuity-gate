import time

import serial.tools.list_ports


def printPortsList():
    ports = serial.tools.list_ports.comports()
    print([port.device for port in ports])


def testPortail(command):
    port = 'COM5'
    baudrate = 115200
    ser = serial.Serial(port, baudrate)
    time.sleep(2)

    ser.reset_input_buffer()
    ser.reset_output_buffer()

    ser.write('pre-writing'.encode('ascii'))
    ser.write((str(command)).encode('ascii'))

    ser.close()


if __name__ == '__main__':
    # printPortsList()

    testPortail(0)
