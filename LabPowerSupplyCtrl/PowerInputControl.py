
import sys

if "linux" in sys.platform:
    import RPi.GPIO as GPIO

def setupIOs():
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(23, GPIO.OUT)
    GPIO.setup(24, GPIO.OUT)

def turnOnSupply():
    if "linux" in sys.platform:
        setupIOs()
        GPIO.output(23,True)
        time.sleep(1)
        GPIO.output(24,True)
        time.sleep(1)


def turnOffSupply():
    if "linux" in sys.platform:
        GPIO.output(23,False)
        GPIO.output(24,False)
        GPIO.reset()
