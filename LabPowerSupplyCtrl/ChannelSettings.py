
import configparser
import os

class ChannelSettings():
    SECTION_NAME="ChannelSettings"

    def __init__(self,channelNumber):
        print("Inside ChannelSettings")
        self.channelNumber = channelNumber
        self.config = configparser.ConfigParser()
        config_file = self._make_file_name()
        print("Checking file "+config_file+" exists")
        config_exists = os.path.isfile(config_file)
        print("Config file exists = "+str(config_exists))
        if config_exists:
            print("Reading config from file")
            self.config.read(config_file)
        if not self.config.has_section(self.SECTION_NAME):
            print("Adding "+self.SECTION_NAME)
            self.config[self.SECTION_NAME] = {}
        print("Leaving ChannelSettings __init__")

    def _make_file_name(self):
        return os.path.expanduser("~/.labpschannel"+str(self.channelNumber)+".cfg")

    def _save(self):
        with open(self._make_file_name(),'w') as config_file:
            self.config.write(config_file)

    def set_voltage(self,voltage):
        self.config[self.SECTION_NAME]["Voltage"] = str(voltage)
        self._save()

    def get_voltage(self):
        voltage = 10.0 
        try:
            voltage = float(self.getValue("Voltage"))
        except configparser.NoOptionError:
            pass
    
        if not (0.0 < voltage <= 30.0 ):
            voltage = 10.0

        print("Init settings have voltage = "+str(voltage));
        return voltage

    def set_current(self,current):
        self.config[self.SECTION_NAME]["Current"] = str(current)
        self._save()

    def get_current(self):
        current = 1.0
        try:
            current = float(self.getValue("Current"))
        except configparser.NoOptionError:
            Pass

        if not (0.0 < current <= 4.8):
            current = 1.0

        print("Init settings have current = "+str(current));
        return current

    def getValue(self,key):
        if key in self.config[self.SECTION_NAME]:
            return self.config[self.SECTION_NAME][key]
        else:
            return "0.0"


