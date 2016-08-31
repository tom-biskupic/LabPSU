

import sys
import time

if "linux" in sys.platform:
    import RPi.GPIO as GPIO

class FanController():

    def __init__(self):
        self.current_duty=0
        self.temps = [0,0,0]
        if "linux" in sys.platform:
            self.setupIOs()

    def setupIOs(self):
        GPIO.setup(12,GPIO.OUT)
        self.fan_pwm = GPIO.PWM(12,100)
        self.fan_pwm.start(0)

    def set_temp(self,temp,channel):
        self.temps[channel-1] = temp
        self.update_fan()

    def find_max_temp(self):
        max_temp = 0.0
        for channel_temp in self.temps:
            if channel_temp > max_temp:
                max_temp = channel_temp
        return max_temp
 
    def update_fan(self):
        self.set_fan(self.find_max_temp())

    def set_fan(self,temp):
        duty=0
        if temp > 30.0:
            duty = 50 + int((temp - 30.0) * 10)
            if duty > 100:
                duty=100
        if duty != self.current_duty:
            self.fan_pwm.ChangeDutyCycle(duty)
            self.current_duty=duty

