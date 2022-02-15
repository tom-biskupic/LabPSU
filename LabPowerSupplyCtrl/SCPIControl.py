import socket
import threading
import socketserver

from scpiparser.CommandInterpreter import CommandInterpreter
from scpiparser.CommandHandler import CommandHandler
from scpiparser.QueryHandler import QueryHandler

class MeasureVoltage(QueryHandler):
    def __init__(self,device_context):
        self.device_context = device_context

    def query(self,program_header,call_context):
        return str(self.device_context.get_channel(call_context.get_current_channel()).get_output_voltage())

class MeasureCurrent(QueryHandler):
    def __init__(self,device_context):
        self.device_context = device_context

    def query(self,program_header,call_context):
        return str(self.device_context.get_channel(call_context.get_current_channel()).get_output_current())

class SelectInstrument(CommandHandler):
    def __init__(self,device_context):
        self.device_context = device_context

    def set(self,program_header,data,call_context):
        if data < 0 or data > 2:
            return "Invalid device Number"
        call_context.set_current_channel(int(data))
        return "Ok\n"
class OutputState(CommandHandler):
    def __init__(self,device_context):
        self.device_context = device_context

    def set(self,program_header,data,call_context):
        self.device_context.get_channel(call_context.get_current_channel()).enable(int(data)>0)
        return "Ok\n"

class SetCurrent(CommandHandler):
    def __init__(self,device_context):
        self.device_context = device_context

    def set(self,program_header,data,call_context):
        self.device_context.get_channel(call_context.get_current_channel()).set_set_current(data)
        return "Ok\n"

class SetVoltage(CommandHandler):
    def __init__(self,device_context):
        self.device_context = device_context

    def set(self,program_header,data,call_context):
        self.device_context.get_channel(call_context.get_current_channel()).set_set_voltage(data)
        return "Ok\n"

class DeviceContext:
    def __init__(self,channels,ci):
        self.channels = channels
        ci.register_query_handler("MEASure:VOLTage",MeasureVoltage(self))
        ci.register_query_handler("MEASure:CURRent",MeasureCurrent(self))
        ci.register_command_handler("NSTrument:NSELect",SelectInstrument(self))
        ci.register_command_handler("OUTPut",OutputState(self))
        ci.register_command_handler("SOURce:CURRent",SetCurrent(self))
        ci.register_command_handler("SOURce:VOLTage",SetVoltage(self))

    def get_channel(self,chan_num):
        return self.channels[chan_num]
class SCPIRequestHandler(socketserver.StreamRequestHandler):
    # def __init__(self,request,client_address,server):
    #     super().__init__(request,client_address,server)
    #     self.current_channel = 0

    def handle(self):
        self.current_channel = 0
        for line in self.rfile:
            line = line.rstrip()
            print("<"+str(line,'ascii')+">")
            response = self.server.command_interpreter.process_line(str(line,'ascii'),self)
            self.wfile.write(response.encode())

    def get_current_channel(self):
        return self.current_channel

    def set_current_channel(self,channel):
        self.current_channel = channel


class SCPITCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    MANUFACTURER='Runcble Software'
    MODEL='Triple Channel Power Supply'
    SERIAL='1'    
    FIRMWARE_VERSION='0.1'
    def __init__(self, psu_channels ):
        socketserver.TCPServer.__init__(self, ("",5025), SCPIRequestHandler)
        socketserver.ThreadingMixIn.__init__(self)
        self.command_interpreter = CommandInterpreter(
            manufacturer=self.MANUFACTURER,
            model=self.MODEL,
            serial=self.SERIAL,
            firmware_version=self.FIRMWARE_VERSION)
        self.device_context = DeviceContext(psu_channels,self.command_interpreter)
        self.started = False

    def start(self):
        self.server_thread = threading.Thread(target=self.serve_forever)
        self.server_thread.daemon = True
        self.server_thread.start()
        self.started = True

    def stop(self):
        if self.started:
            self.shutdown()
