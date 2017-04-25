
import ConfigParser
import os

class ChannelSettings():
    SECTION_NAME="ChannelSettings"

    def __init__(self,channelNumber):
        print("Inside ChannelSettings")
        self.channelNumber = channelNumber
        self.config = ConfigParser.RawConfigParser()
        config_file = self._make_file_name()
        print("Checking file "+config_file+" exists")
        config_exists = os.path.isfile(config_file)
        print("Config file exists = "+str(config_exists))
        if config_exists:
            print("Reading config from file")
            self.config.read(config_file)
        if not self.config.has_section(self.SECTION_NAME):
            print("Adding "+self.SECTION_NAME)
            self.config.add_section(self.SECTION_NAME)
        print("Leaving ChannelSettings __init__")

    def _make_file_name(self):
        return "./Channel"+str(self.channelNumber)+".cfg"

    def _save(self):
        with open(self._make_file_name(),'wb') as config_file:
            self.config.write(config_file)

    def set_voltage(self,voltage):
        self.config.set(self.SECTION_NAME,"Voltage",voltage)
        self._save()

    def get_voltage(self):
        voltage = 10.0 
        try:
            voltage = self.config.getfloat(self.SECTION_NAME,"Voltage")
        except ConfigParser.NoOptionError:
            Pass
    
        if not (0.0 < voltage <= 30.0 ):
            voltage = 10.0

        print("Init settings have voltage = "+str(voltage));
        return voltage

    def set_current(self,current):
        self.config.set(self.SECTION_NAME,"Current",current)
        self._save()

    def get_current(self):
        current = 1.0
        try:
            current = self.config.getfloat(self.SECTION_NAME,"Current")
        except ConfigParser.NoOptionError:
            Pass

        if not (0.0 < current <= 4.8):
            current = 1.0

        print("Init settings have current = "+str(current));
        return current

