
__author__ = 'tom'

import socket
import time

from PowerSupplyChannel import PowerSupplyChannel

class DMM():
    def connect(self,address):
        self.s = socket.socket()
        self.s.connect((address,5025))
        self.s.send("*IDN?\n")
        print(self.s.recv(300).decode("UTF-8"))
        pass

    def read_voltage(self):
        self.s.send("MEASURE:VOLTAGE:DC?\r\n")
        return self.s.recv(300).decode("UTF-8").rstrip()

    def read_current(self):
        self.s.send("SENSE:CURRENT:DC:TERMINALS 10\r\n");
        self.s.send("read?\r\n");
        return self.s.recv(300).decode("UTF-8").rstrip();

dmm = DMM()

dmm.connect("192.168.1.220")

psu = PowerSupplyChannel("/dev/ttyACM0");
psu.connect()

def voltage_dac_calibrate():
    psu.enable("True")
    count=0
    while count <= 0xffff:
        psu.set_voltage_dac(count)
        voltage = dmm.read_voltage()

        countAsString = "0x%x" % count
        print("        {"+countAsString+",    "+voltage+"},")

        count += 0xffff/32;
        time.sleep(2.0)

    psu.set_set_voltage(10.0)
    psu.enable("False")

def voltage_adc_calibrate():
    psu.enable("True")
    count=0
    while count <= 0xffff:
        psu.set_voltage_dac(count)
        voltage = dmm.read_voltage()
        adcCount = psu.get_voltage_adc();

        countAsString = "0x%x" % adcCount
        print("        {"+countAsString+",    "+voltage+"},")

        count += 0xffff/32;
        time.sleep(2.0)

    psu.set_set_voltage(10.0)
    psu.enable("False")

def current_dac_calibrate():
    psu.set_set_voltage(5.5)
    count=0
    max = int(float(0xffff)/4.8 * 4.5)
    while count <= max:
        psu.set_current_dac(count)
        psu.enable("True")
        time.sleep(4.0)
        current = dmm.read_current()
        psu.enable("False")
        countAsString = "0x%x" % count
        print("        {"+countAsString+",    "+current+"},")

	# Let it rest between reads to let things cool down

        time.sleep(4.0)
        count += 0xffff/32;

    psu.set_set_voltage(10.0)
    psu.enable("False")

def current_adc_calibrate():
    psu.set_set_voltage(5.5)
    count=0
    max = int(float(0xffff)/4.8 * 4.5)
    while count <= max:
        psu.set_current_dac(count)
        psu.enable("True")
        time.sleep(4.0)
        current = dmm.read_current()
        readCount = psu.get_current_adc()

        psu.enable("False")
        countAsString = "0x%x" % readCount
        print("        {"+countAsString+",    "+current+"},")
	
        time.sleep(4.0)
        count += 0xffff/32;

#voltage_adc_calibrate()
#voltage_dac_calibrate()
current_dac_calibrate()
#current_adc_calibrate()

