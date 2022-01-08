
import serial

from ChannelState import ChannelState
from ChannelCommand import SetCurrentCommand, SetVoltageCommand, EnableCommand
from ChannelCalControls import ChannelCalControls
from Linearizer import Linearizer

class CurrentCalControls(ChannelCalControls):
    def __init__(self,connection,channel_settings):
        self.channel_settings = channel_settings
        self.connection = connection

    def set_dac(self,value):
        self.connection._set_current_dac(value)

    def get_adc(self):
        return int(self.connection.call_get_command(self.connection.IADC_COMMAND),16)

    def set_cal_points(self,points):
        self.channel_settings.set_current_cal(points)

class VoltageCalControls(ChannelCalControls):
    def __init__(self,connection,channel_settings):
        self.channel_settings = channel_settings
        self.connection = connection

    def set_dac(self,value):
        self.connection._set_voltage_dac(value)

    def get_adc(self):
        return int(self.connection.call_get_command(self.connection.VADC_COMMAND),16)

    def set_cal_points(self,points):
        self.channel_settings.set_voltage_cal(points)

class PowerSupplyChannelConnection:
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
    IDAC_COMMAND="IDAC"
    IADC_COMMAND="IADC"
    POLL_PERIOD=0.1
    VOLTS_DAC_CAL_COMMAND="VDACCal"
    VOLTS_ADC_CAL_COMMAND="VADCCal"
    CURRENT_DAC_CAL_COMMAND="IDACCal"
    CURRENT_ADC_CAL_COMMAND="IADCCal"

    def __init__(self,channelNumber,file_name):
        self.channel_number = channelNumber
        self.usb_device_filename = file_name
        self.serialPort = None
        self.set_voltage = 0.0
        self.set_voltage_dac_val = 0
        self.set_current = 0.0
        self.set_current_dac_val = 0
        self.enabled = False

    def connect(self):
        try:
            self.serialPort = serial.Serial(
                port=self.usb_device_filename,
                baudrate=115200,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS )
            #print("Connected to "+self.usb_device_filename)
        except Exception :
            print("Connect to %s failed" % self.usb_device_filename)
            self.serialPort = None

    def close(self):
        self.serialPort = None

    def is_connected(self):
        return self.serialPort is not None

    def update_channel_state(self,channel_state):
        channel_state.set_set_voltage(self.get_set_voltage())
        channel_state.set_set_current(self.get_set_current())
        channel_state.set_output_voltage(self.get_output_voltage())
        channel_state.set_output_current(self.get_output_current())
        channel_state.set_in_cc_mode(self.get_ccmode())
        channel_state.set_is_enabled(self.get_is_enabled())

    def execute_command(self,channel_command):
        if isinstance(channel_command,SetVoltageCommand):
            self.set_set_voltage(channel_command.get_value())
        if isinstance(channel_command,SetCurrentCommand):
            self.set_set_current(channel_command.get_value())
        if isinstance(channel_command,EnableCommand):
            self.set_is_enabled(channel_command.get_value())

    def init_from_settings(self,channel_settings):
        self._init_linearizers(channel_settings)
        self.set_set_voltage(channel_settings.get_voltage())
        self.set_set_current(channel_settings.get_current())

    def _init_linearizers(self,channel_settings):
        voltage_cal_table = channel_settings.get_voltage_cal()
        if len(voltage_cal_table) == 0:
            voltage_cal_table = self._make_default_voltage_cal_table()
        (voltage_dac_cal,voltage_adc_cal) = self._make_linearizer_tables(voltage_cal_table)
        self.voltage_dac_linearizer = Linearizer(voltage_dac_cal)
        self.voltage_adc_linearizer = Linearizer(voltage_adc_cal)

        current_cal_table = channel_settings.get_current_cal()
        if len(current_cal_table) == 0:
            current_cal_table = self._make_default_current_cal_table()
        (current_dac_cal,current_adc_cal) = self._make_linearizer_tables(current_cal_table)
        self.current_dac_linearizer = Linearizer(current_dac_cal)
        self.current_adc_linearizer = Linearizer(current_adc_cal)

    def _make_linearizer_tables(self,cal_table):
        (dac_val_list,dmm_list,adc_val_list) = zip(*list(cal_table))
        dmm_dac_cal = list(zip(dac_val_list,dmm_list))
        dmm_adc_cal = list(zip(adc_val_list,dmm_list))
        return (dmm_dac_cal,dmm_adc_cal)

    def _make_default_voltage_cal_table(self):
        #print("Using default voltage cal table for channel "+str(self.channel_number))
        return [(0,0.0,0), (0xffff,30.0,0xffff)]

    def _make_default_current_cal_table(self):
        #print("Using default voltage cal table for channel "+str(self.channel_number))
        max_count = int(4.8/5.0*0xffff)
        return [(0,0.0,0), (max_count,4.8,max_count)]

    def get_current_cal_controls(self,channel_settings):
        return CurrentCalControls(self,channel_settings)

    def get_voltage_cal_controls(self,channel_settings):
        return VoltageCalControls(self,channel_settings)

    def get_set_voltage(self):
        return self.set_voltage

    def set_set_voltage(self,value):
        self.set_voltage_dac_val = self.voltage_dac_linearizer.value_to_code(value)
        #print("Channel %d Setting DAC 0x%0X Value = %f" % (self.channel_number,code,value))
        if self.enabled:
            self._set_voltage_dac(self.set_voltage_dac_val)
        self.set_voltage = value
        #self.call_set_command(self.VSET_COMMAND,str(value))

    def get_set_current(self):
        return self.set_current

    def set_set_current(self,value):
        self.set_current_dac_val = self.current_dac_linearizer.value_to_code(value)
        if self.enabled:
            self._set_current_dac(self.set_current_dac_val)
        self.set_current = value

    def get_output_voltage(self):
        code = int(self.call_get_command(self.VADC_COMMAND),16)
        value = self.voltage_adc_linearizer.code_to_value(code) 
        return value

    def get_output_current(self):
        code = int(self.call_get_command(self.IADC_COMMAND),16)
        value = self.current_adc_linearizer.code_to_value(code)
        return value

    def get_ccmode(self):
        return self.enabled and self.call_get_command_bool(self.CCMODE_COMMAND)

    def get_is_enabled(self):
        return self.enabled

    def set_is_enabled(self,value):
        self.enabled = value
        if self.enabled:
            self._set_voltage_dac(self.set_voltage_dac_val)
            self._set_current_dac(self.set_current_dac_val)
        else:
            print("Setting voltage DAC to zero")
            self._set_voltage_dac(0)
            self._set_current_dac(0)

    def _set_voltage_dac(self,value):
        self.call_set_command(self.VDAC_COMMAND,("%x" % value))

    def _set_current_dac(self,value):
        self.call_set_command(self.IDAC_COMMAND,("%x" % value))

    def call_set_command(self,command,value):
        command_string = command+"="+value+"\n"
        #print('Channel number %d call_set_command, Command=%s'%(self.channel_number,command_string))
        self.serialPort.write(command_string.encode('utf-8'))
        #print('Channel number %d response1=%s'%(self.channel_number,self.serialPort.readline()))
        #print('Channel number %d response2=%s'%(self.channel_number,self.serialPort.readline()))
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
        #print("Channel number %d call_get_command, Command = %s" % (self.channel_number,command_string))
        self.serialPort.write(command_string.encode('utf-8'))
        self.serialPort.readline()
        response = self.serialPort.readline()
        #print("Channel number %d get command response %s" % (self.channel_number,response))
        return_string = response.decode('utf-8')
        return_string = return_string.split("=")[1]
        return_string = return_string.rstrip()
        return return_string
