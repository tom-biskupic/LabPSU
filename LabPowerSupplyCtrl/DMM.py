__author__ = 'tom'

import socket
import sys

class DMM():
    def connect(self,address):
        self.s = socket.socket()
        self.s.connect((address,5025))
        self.send("*IDN?\n")
        print(self.s.recv(300).decode("UTF-8"))

    def send(self,str):
        if "linux" in sys.platform:
            self.s.send(str)
        else:
            self.s.send(bytes(str),'UTF-8')
 
    def close(self):
        self.s.close()
        self.s = None

    def read_voltage(self):
        self.send("MEASURE:VOLTAGE:DC?\r\n")
        return float(self.s.recv(300).decode("UTF-8").rstrip())

    def read_current(self):
        self.send("SENSE:CURRENT:DC:TERMINALS 10\r\n");
        self.s.send(bytes("read?\r\n",'UTF-8'));
        return float(self.s.recv(300).decode("UTF-8").rstrip());

