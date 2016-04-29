
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

dmm = DMM()

dmm.connect("192.168.1.45")

psu = PowerSupplyChannel("/dev/cu.usbmodem1421");
psu.connect()
psu.enable("True")

count=0

while count <= 0xffff:
    psu.set_voltage_dac(count)
    voltage = dmm.read_voltage()

    countAsString = "0x%x" % count
    print("        {"+countAsString+",    "+voltage+"},")

    count += 0xffff/32;
    time.sleep(2.0)

psu.set_output_voltage(10.0)

