# ********************************************************************************
# 
# Provides USB related functions that allow communication with the embedded device.
#
# Created on Thu Nov 21 2024
#
# Daniel Figueira <daniel.castro.figueira@gmail.com>
# 
# ********************************************************************************

import serial
import os

######################## CONSTANTS ########################

DEFAULT_PORT = os.getenv("USB_PORT") if os.getenv("USB_PORT") else "/dev/ttyACM0"
BAUD_RATE = os.getenv("BAUD_RATE") if os.getenv("BAUD_RATE") else 115200
READ_TIMEOUT = 0.01

###################### PUBLIC FUNCTIONS ####################

ser = None

def init(port = DEFAULT_PORT, read_timeout = READ_TIMEOUT):
    global ser
    ser = serial.Serial(port, BAUD_RATE, timeout=read_timeout)

def get_port():
    global ser
    return ser.port if ser else DEFAULT_PORT

def read(size = None):
    global ser

    if size != None:
        # If the number of bytes is specified, read that number of bytes
        data = ser.read(size)
    else:
        # Otherwise read all the bytes available
        new_bytes = ser.read()
        data = new_bytes
        while len(new_bytes) > 0 and new_bytes:
            new_bytes = ser.read()
            data += new_bytes

    return data

def send(data):
    global ser
    ser.write(data)

def flush_input():
    global ser
    ser.reset_input_buffer()

def close():
    global ser
    if ser:
        ser.close()