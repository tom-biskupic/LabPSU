__author__ = 'tom'

import socket

class DMM():
    def connect(self,address):
        self.s = socket.socket()
        self.s.connect((address,5025))
        self.s.send(bytes("*IDN?\n",'UTF-8'))
        print(self.s.recv(300).decode("UTF-8"))

    def close(self):
        self.s.close()
        self.s = None

    def read_voltage(self):
        self.s.send(bytes("MEASURE:VOLTAGE:DC?\r\n",'UTF-8'))
        return float(self.s.recv(300).decode("UTF-8").rstrip())

    def read_current(self):
        self.s.send(bytes("SENSE:CURRENT:DC:TERMINALS 10\r\n",'UTF-8'));
        self.s.send(bytes("read?\r\n",'UTF-8'));
        return float(self.s.recv(300).decode("UTF-8").rstrip());

