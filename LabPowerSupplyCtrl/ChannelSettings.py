
import configparser
import os

class ChannelSettings():
    SECTION_NAME="ChannelSettings"
    CAL_SECTION_NAME="ChannelCalibrationSettings"
    VOLTAGE_CAL_TYPE="Voltage"
    CURRENT_CAL_TYPE="Current"

    def __init__(self,channelNumber):
        self.channelNumber = channelNumber
        self.config = configparser.ConfigParser()
        config_file = self._make_file_name(channelNumber)
        config_exists = os.path.isfile(config_file)
        if config_exists:
            self.config.read(config_file)
        if not self.config.has_section(self.SECTION_NAME):
            self.config[self.SECTION_NAME] = {}

    def _make_file_name(self,channelNumber):
        return os.path.expanduser("~/.labpschannel"+str(channelNumber)+".cfg")

    def _save(self):
        with open(self._make_file_name(self.channelNumber),'w') as config_file:
            self.config.write(config_file)

    def set_voltage(self,voltage):
        self.config[self.SECTION_NAME]["Voltage"] = str(voltage)
        self._save()

    def get_voltage(self):
        voltage = 10.0
        try:
            voltage = float(self._get_value("Voltage"))
        except configparser.NoOptionError:
            pass

        if not (0.0 < voltage <= 30.0 ):
            voltage = 10.0

        return voltage

    def set_current(self,current):
        self.config[self.SECTION_NAME]["Current"] = str(current)
        self._save()

    def get_current(self):
        current = 1.0
        try:
            current = float(self._get_value("Current"))
        except configparser.NoOptionError:
            pass

        if not (0.0 < current <= 4.8):
            current = 1.0

        return current

    def set_voltage_cal(self,cal_table):
        self._set_cal_table(self.VOLTAGE_CAL_TYPE,cal_table)
        self._save()

    def get_voltage_cal(self):
        return self._get_cal_table(self.VOLTAGE_CAL_TYPE)

    def set_current_cal(self,cal_table):
        self._set_cal_table(self.CURRENT_CAL_TYPE,cal_table)
        self._save()

    def get_current_cal(self):
        return self._get_cal_table(self.CURRENT_CAL_TYPE)

    def _get_value(self,key):
        if key in self.config[self.SECTION_NAME]:
            return self.config[self.SECTION_NAME][key]
        else:
            return "0.0"

    def _set_cal_table(self,cal_type,cal_table):
        section = self.CAL_SECTION_NAME+cal_type
        if not self.config.has_section(section):
            self.config[section] = {}

        self.config[section]["NumPoints"] = str(len(cal_table))

        for point_num in range(0,len(cal_table)):
            self.config[section][("calpoint_%d" % point_num)] = self._cal_point_to_string(cal_table[point_num])

        self._save()

    def _cal_point_to_string(self,cal_point):
        return "%0x,%f,%0x" % (cal_point[0], cal_point[1], cal_point[2])

    def _get_cal_table(self,cal_type):
        section = self.CAL_SECTION_NAME+cal_type
        if not self.config.has_section(section):
            return []

        num_points = int(self.config[section]["NumPoints"])

        cal_table = []

        for point_num in range(0,num_points):
            cal_table.append(self._string_to_cal_point(self.config[section][("calpoint_%d" % point_num)]))

        return cal_table

    def _string_to_cal_point(self,cal_point_string):
        vals = cal_point_string.split(',')
        return (int(vals[0],16), float(vals[1]), int(vals[2],16) )