# ********************************************************************************
# 
# Provides project specific functions to communicate over USB with the embedded device.
#
# Created on Sun Nov 24 2024
#
# Daniel Figueira <daniel.castro.figueira@gmail.com>
# 
# ********************************************************************************
import os
import time

import test_utils.usb_utils as usb

######################## SETTINGS #########################

DEFAULT_DATA_RATE = int(os.getenv("DATA_RATE")) if os.getenv("DATA_RATE") else 100

USB_CONNECTION_WAIT_PERIOD  = 1.0 # seconds
USB_COMMAND_INTERVAL        = 1.0 # seconds

######################## CONSTANTS ########################

# Command types
COMMAND_SET_DATA_RATE = 0
COMMAND_SET_READ_RATE = 1
COMMAND_SET_SEND_RATE = 2
COMMAND_START_PATTERN = 3

# Simulation patterns
PATTERN_CONST = 0
PATTERN_INCREASING = 1
PATTERN_DECREASING = 2
PATTERN_RANDOM = 3

###################### PUBLIC FUNCTIONS ####################

data_rate_modified = False
read_rate_modified = False
send_rate_modified = False

def init():
    ''' Initialize the USB connection '''
    # Make the read timeout twice the sample period 
    usb.init(usb.DEFAULT_PORT, 2 / DEFAULT_DATA_RATE)
    time.sleep(USB_CONNECTION_WAIT_PERIOD)
    set_data_rate(DEFAULT_DATA_RATE, False)
    set_read_rate(DEFAULT_DATA_RATE, False)
    set_send_rate(DEFAULT_DATA_RATE, False)

def restore_data_rates():
    ''' Set the default data/read/send rates '''
    global data_rate_modified
    global read_rate_modified
    global send_rate_modified
    try:
        if data_rate_modified:
            set_data_rate(DEFAULT_DATA_RATE, False)
        if read_rate_modified:
            set_read_rate(DEFAULT_DATA_RATE, False)
        if send_rate_modified:
            set_send_rate(DEFAULT_DATA_RATE, False)
    except Exception as e:
        print(f"Error: Failed to restore default data rates: {e}")

def clear_buffers():
    ''' Clear the input and output buffers '''
    usb.clear_input()
    usb.clear_output()

def read_data():
    ''' Read all data samples available '''
    samples = []
    while True:
        data = usb.read()
        if not data:
            break
        data = data.decode().strip().split('\n')
        samples += [float(x) for x in data]

    return samples
    

def set_data_rate(data_rate, restore=True):
    ''' Set the rate at which the simulated data is produced '''
    global data_rate_modified
    usb.send(f"{COMMAND_SET_DATA_RATE} {data_rate}".encode())
    time.sleep(USB_COMMAND_INTERVAL)
    data_rate_modified = restore

def set_read_rate(read_rate, restore=True):
    ''' Set the rate at which the simulated data is read '''
    global read_rate_modified
    usb.send(f"{COMMAND_SET_READ_RATE} {read_rate}".encode())
    time.sleep(USB_COMMAND_INTERVAL)
    read_rate_modified = restore

def set_send_rate(send_rate, restore=True):
    ''' Set the rate at which the simulated data is sent '''
    global send_rate_modified
    usb.send(f"{COMMAND_SET_SEND_RATE} {send_rate}".encode())
    time.sleep(USB_COMMAND_INTERVAL)
    send_rate_modified = restore

def simulate_const_pattern(value, n_samples):
    ''' Start a 'const' pattern simulation '''
    usb.send(f"{COMMAND_START_PATTERN} {PATTERN_CONST} {value} {n_samples}".encode())
    time.sleep(USB_COMMAND_INTERVAL)
    return read_data()

def simulate_increasing_pattern(start_value, increment, max_value):
    ''' Start a 'increasing' pattern simulation '''
    usb.send(f"{COMMAND_START_PATTERN} {PATTERN_INCREASING} {start_value} {increment} {max_value}".encode())
    time.sleep(USB_COMMAND_INTERVAL)
    return read_data()

def simulate_decreasing_pattern(start_value, decrement, min_value):
    ''' Start a 'decreasing' pattern simulation '''
    usb.send(f"{COMMAND_START_PATTERN} {PATTERN_DECREASING} {start_value} {decrement} {min_value}".encode())
    time.sleep(USB_COMMAND_INTERVAL)
    return read_data()

def simulate_random_pattern(min_value, max_value, n_samples):
    ''' Start a 'random' pattern simulation '''
    usb.send(f"{COMMAND_START_PATTERN} {PATTERN_RANDOM} {min_value} {max_value} {n_samples}".encode())
    time.sleep(USB_COMMAND_INTERVAL)
    return read_data()