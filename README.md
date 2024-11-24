# Context
This repository contains the source code for an example sensor project, which basically:

- Simulates sensor data
- Stores the simulated data into a ring buffer
- Accesses the data using a second thread
- Sends the data to the PC over USB

The solution is implemented in C - the language I feel most confortable with professionaly - and aims to showcase a bit of what I do on a daily basis.

The software is meant to be run on a [nRF5340 DK](https://www.nordicsemi.com/Products/Development-hardware/nRF5340-DK) board using ZephyrRTOS, although support for other boards that can run ZephyrRTOS should also be possible (with some effort).

For development I'll be using VSCode on a Linux PC, and the [nRF Connect SDK](https://www.nordicsemi.com/Products/Development-software/nRF-Connect-SDK).

To interact with the device over USB I'll be using some basic Python code.

For integration tests - which will be run with the PC connected to the device - I'll be using [PyTest](https://docs.pytest.org/en/stable/).

# Usage



# Effort breakdown

- Repository setup (ssh keys, .gitignore, README.md): 1h
- ZephyrRTOS setup (CMake files, main.c, first build): 0.5h
- USB Comm Device side (usb_comm.h + troubleshooting): 3h
- USB Comm PC side (usb_comm.py): 1.5h
- Sensor simulation functions (sim_sensor.h + troubleshooting): 4h
- USB Commands (command_parser.h + troubleshooting): 2h
- PyTest setup: 0.5h
- Sensor simulation tests: 2h
- Bugfixing: 1.5h
- Ring buffer implementation: 1h
- Sensor thread: 0.5h
- Data thread: 

- Total: 14.5h 

# Other remarks
