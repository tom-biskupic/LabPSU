
import serial

__author__ = 'tom'

class PowerSupplyChannel():
    VSET_COMMAND="Vset"
    ISET_COMMAND="Iset"
    VOUT_COMMAND="Vout"
    IOUT_COMMAND="Iout"
    CCMODE_COMMAND="CCMode"
    ENABLE_COMMAND="Enable"
    VOLTAGE_DECIMALS=3
    CURRENT_DECIMALS=4
    VDAC_COMMAND="VDAC"
    VADC_COMMAND="VADC"

    def __init__(self,file_name):
        self.usb_device_filename=file_name
        self.serialPort=None

    def connect(self):
        try:
            self.serialPort = serial.Serial(
                port=self.usb_device_filename,
                baudrate=115200,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS )
        except Exception :
            print "Connect to %s failed" % self.usb_device_filename
            self.serialPort = None;

    def is_connected(self):
        return self.serialPort is not None;

    def get_set_voltage(self):
        return round(
            float(self.call_get_command_float(self.VSET_COMMAND)),
            self.VOLTAGE_DECIMALS)

    def set_set_voltage(self,voltage):
        self.call_set_command(self.VSET_COMMAND,str(voltage))

    def get_set_current(self):
        return round(
            float(self.call_get_command_float(self.ISET_COMMAND)),
            self.CURRENT_DECIMALS)

    def set_set_current(self,current):
        self.call_set_command(self.ISET_COMMAND,str(current))

    def get_output_voltage(self):
        return round(
            float(self.call_get_command_float(self.VOUT_COMMAND)),
            self.VOLTAGE_DECIMALS)

    def get_output_current(self):
        return round(
            float(self.call_get_command_float(self.IOUT_COMMAND)),
            self.CURRENT_DECIMALS)

    def in_cc_mode(self):
        return self.call_get_command_bool(self.CCMODE_COMMAND)

    def enable(self,enabled):
        self.call_set_command(self.ENABLE_COMMAND,str(enabled))

    def set_voltage_dac(self,count):
        self.call_set_command(self.VDAC_COMMAND,("%x" % count))

    def get_voltage_adc(self):
        return int(self.call_get_command(self.VADC_COMMAND),16)

    def is_enabled(self):
        return self.call_get_command_bool(self.ENABLE_COMMAND)

    def call_set_command(self,command,value):
        command_string = command+"="+value+"\n"
        self.serialPort.write(command_string)
        self.serialPort.readline()
        self.serialPort.readline()

    def call_get_command_float(self,command):
        value = self.call_get_command(command)
        return float(value)

    def call_get_command_bool(self,command):
        value = self.call_get_command(command)
        return value.lower() == "true"

    def call_get_command(self,command):
        command_string = command+"=?\n"
        self.serialPort.write(command_string)
        self.serialPort.readline()
        return_string = self.serialPort.readline()
        return_string = return_string.split("=")[1]
        return_string = return_string.rstrip()
        return return_string

