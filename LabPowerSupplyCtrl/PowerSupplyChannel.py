__author__ = 'tom'

class PowerSupplyChannel():
    VSET_COMMAND="Vset"
    ISET_COMMAND="Iset"
    VOUT_COMMAND="Vout"
    IOUT_COMMAND="Iout"
    CCMODE_COMMAND="CCMode"
    ENABLE_COMMAND="Enable"

    usb_device_filename=None
    tty=None

    def __init__(self,file_name):
        self.usb_device_filename = file_name
        self.tty = open(self.usb_device_filename,"r+")

    def get_set_voltage(self):
        return self.call_get_command_float(self.VSET_COMMAND)

    def set_set_voltage(self,voltage):
        self.call_set_command(self.VSET_COMMAND,str(voltage))

    def get_set_current(self):
        return self.call_get_command_float(self.ISET_COMMAND)

    def set_set_current(self,current):
        self.call_set_command(self.ISET_COMMAND,str(current))

    def get_output_voltage(self):
        return self.call_get_command_float(self.VOUT_COMMAND)

    def get_output_current(self):
        return self.call_get_command_float(self.IOUT_COMMAND)

    def in_cc_mode(self):
        return self.call_get_command_bool(self.CCMODE_COMMAND)

    def enable(self,enabled):
        self.call_set_command(self.ENABLE_COMMAND,str(enabled))

    def is_enabled(self):
        return self.call_get_command_bool(self.ENABLE_COMMAND)

    def call_set_command(self,command,value):
        command_string = command+"="+value+"\n"
        self.tty.write(command_string)
        self.tty.readline()

    def call_get_command_float(self,command):
        value = self.call_get_command(command)
        return float(value)

    def call_get_command_bool(self,command):
        value = self.call_get_command(command)
        return value.casefold() == "true"

    def call_get_command(self,command):
        command_string = command+"=?\n"
        self.tty.write(command_string)
        return_string = self.tty.readline()
        return_string = return_string.split("=")[1]
        return_string.rsplit('\n')
        return return_string

