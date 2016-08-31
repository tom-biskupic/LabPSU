#!/usr/bin/env python

from subprocess import call
import RPi.GPIO as gpio
import os
import time
 
# Define a function to keep script running
def loop():
    while True:
        time.sleep(5)
 
# Define a function to run when an interrupt is called
def shutdown(pin):
    os.system("sudo shutdown -h now")
 
gpio.setmode(gpio.BCM)
gpio.setup(17, gpio.IN)
gpio.add_event_detect(17, gpio.FALLING, callback=shutdown, bouncetime=200)
 
loop() # Run the loop function to keep script running

