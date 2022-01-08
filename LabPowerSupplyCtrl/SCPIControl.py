import socket
import threading
import socketserver

from scpiparser.CommandInterpreter import CommandInterpreter
from scpiparser.CommandHandler import CommandHandler
from scpiparser.QueryHandler import QueryHandler

class SCPIRequestHandler(socketserver.StreamRequestHandler):
    def handle(self):
        for line in self.rfile:
            line = line.rstrip()
            print("<"+str(line,'ascii')+">")
            response = ci.process_line(str(line,'ascii'))
            self.wfile.write(response.encode())

class SCPITCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    MANUFACTURER='Runcble Software'
    MODEL='Triple Channel Power Supply'
    SERIAL='1'    
    FIRMWARE_VERSION='0.1'
    def __init__(self, server_address, handler_class=SCPIRequestHandler):
        self.command_interpreter = CommandInterpreter(
            manufacturer=self.MANUFATURER,
            model=self.MODEL,
            self.SERIAL='0',firmware_version='0.1')
        socketserver.TCPServer.__init__(self, server_address, handler_class)

