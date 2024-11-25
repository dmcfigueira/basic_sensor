# ********************************************************************************
# 
# Python script used to test basic communication over USB with the embedded device.
# 
# Created on Thu Nov 21 2024
# 
# Daniel Figueira <daniel.castro.figueira@gmail.com>
#
# ********************************************************************************

import sys
import threading
import time
import os

import tests.test_utils.usb_utils as usb

######################### USB RX #########################

def wait_for_usb_connection():
    print(f"Waiting for USB a connection...")
    while True:
        try:
            usb.close()
            time.sleep(1)
            usb.init(sys.argv[1] if len(sys.argv) > 1 else None)
            break
        except Exception as e:
            if not "Errno 2" in str(e):
                print(f"USB connection failed: {e}")
    print("Connected!")

def usb_rx_loop():
    while True:
        try:
            # Wait for a USB connection
            wait_for_usb_connection()    

            while True:
                # Read and print any data sent over USB
                data = usb.read().strip()
                if data:
                    print(f"> {data.decode('utf-8')}")

        except Exception as e:
            if not "device disconnected" in str(e):
                print(f"Unhandled exception on 'usb_rx' loop: {e}\n")
        
########################## MAIN ##########################

def init():
    # Spawn a separate thread to read data sent over USB
    # (marking it as a 'daemon' will ensure it gets killed when the main thread exits)
    usb_rx_thread = threading.Thread(target=usb_rx_loop)
    usb_rx_thread.daemon = True
    usb_rx_thread.start()

def main():
    print("Application started. Type 'exit', 'quit' or 'q' to exit.")

    # Initialize the app
    init()

    # Start the app main loop
    while (True):
        try:
            # Wait for a command from the user
            usr_cmd = sys.stdin.readline().strip()

            # Terminate the app if the user chooses to quit
            if usr_cmd == "exit" or usr_cmd == "quit" or usr_cmd == "q":
                os._exit(0)

            # Otherwise send the command over USB
            usb.send(usr_cmd.encode('utf-8'))

        except Exception as e:
            print(f"Unhandled exception on 'main' loop: {e}\n")
            time.sleep(1)

if __name__ == "__main__":
    main()
