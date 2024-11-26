# Context
This repository contains the source code for an example sensor project, which:

- Simulates sensor data
- Stores the simulated data into a ring buffer
- Accesses the data using a second thread
- Sends the data to the PC over USB

The solution is implemented in C - the language I feel most confortable with professionaly - and aims to showcase a bit of what I do on a daily basis.

The software is meant to be run on a [nRF5340 DK](https://www.nordicsemi.com/Products/Development-hardware/nRF5340-DK) board using ZephyrRTOS, although support for other boards that can run ZephyrRTOS should also be possible (with some effort).

For development I'll be using VSCode on a Linux PC, and the [nRF Connect SDK](https://www.nordicsemi.com/Products/Development-software/nRF-Connect-SDK).

To interact with the device over USB I'll be using a simple Python test script.

For integration tests - which will be run with the PC connected to the device - I'll be using [PyTest](https://docs.pytest.org/en/stable/).

# Building and flashing the project

Install the 'nRF Connect for VS Code' extension as described [here](https://docs.nordicsemi.com/bundle/ncs-latest/page/nrf/installation/install_ncs.html).

Then:

1) Right click the 'app' folder from VS Code explorer view and select 'nRF Connect: Add Folder as Application'.
2) Go to the the nRF Connect view and add a new build configuration for the 'app' application.
3) For the build settings simply select `nrf5340dk_nrf5340_cpuapp` as the board target.

You should then be able to 'Build', 'Debug' and 'Flash' the project from the 'Actions' list on the nRF Connect view.

### Connecting to the board via USB

To communicate with the board, connect the board to the PC using a USB cable and then run: 

```bash
python3 usb_test_app.py /dev/ttyACM3 # replace with the actual device name
```

Note: If USB port permissions are required, temporarily enable them by running:

```bash
sudo chmod 666 /dev/ttyACM3 # replace with the actual device name
```

### Testing

To run the project tests, connect the board to the PC using a USB cable and then run: 

```bash
pytest tests/                   # to run all tests at once
pytest tests/<test_file>.py     # to run a specific test set only
pytest tests/ -k <substr>       # to filter the tests executed by name
```

# Effort breakdown

Setup (2h):
- Project setup: 1.5h
- PyTest setup:  0.5h

Implementation (15.5):
- USB Comm Device (+troubleshoot):      3.0h
- USB Comm PC python script:            1.5h
- USB Commands (+troubleshoot):         2.0h
- Sensor simulation (+troubleshoot):    4.0h
- Ring buffer implementation:           1.0h
- Sensor thread (+troubleshoot):        3.0h
- Data thread:                          1.0h

Testing (4h):
- Sensor simulation tests:  2.0h
- Ring buffer tests:        0.5h
- Bugfixing:                1.5h

Documentation (1h):
- README.md: 1h

Total: 22.5h