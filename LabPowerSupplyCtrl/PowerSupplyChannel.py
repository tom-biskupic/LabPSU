
import serial
import threading
import sys

if sys.version_info[0] < 3:
    import Queue
else:
    import queue

from Temp import TempSensor
from LockedThing import LockedThing
from ChannelSettings import ChannelSettings

__author__ = 'tom'


class PowerSupplyChannel(threading.Thread):
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

    def __init__(self,channelNumber,file_name,temp_sensor_id):
        self.exit_event = threading.Event()
        self.usb_device_filename=file_name
        self.serialPort=None
        self.temp_sensor = TempSensor(temp_sensor_id)
        self.set_voltage_val = LockedThing(0.0)
        self.set_current_val = LockedThing(0.0)
        self.output_voltage_val = LockedThing(0.0)
        self.output_current_val = LockedThing(0.0)
        self.in_cc_mode_val = LockedThing(0.0)
        self.enabled_val = LockedThing(False)
        self.temp_val = LockedThing(0.0)
	self.pause_lock = threading.Condition()
        self.paused = False
        print("Channel about to load settings")
        self.channel_settings = ChannelSettings(channelNumber)
        self.loaded_settings = False

        if sys.version_info[0] < 3:
            self.command_queue = Queue.Queue()
        else:
            self.command_queue = queue.Queue()

        self.update_count = 0
        threading.Thread.__init__(self)

    def run(self):
        while not self.exit_event.wait(self.POLL_PERIOD):
            self.pause_lock.acquire()
            while self.paused:
                self.pause_lock.wait()
            self.pause_lock.release()
            #
            # If we are not currently connected then try and connect
            #
            if self.serialPort is None:
                self._connect()

            # If we connected or if we already were connected then do
            # the update
            if self.serialPort is not None:
                try:
                    if not self.loaded_settings:
                        self._load_settings()

                    self._process_commands()
                    self._update_from_channel()
                except Exception:
                    # Something failed - just disconnect and re-connect
                    # on the next round
                    self.serialPort = None
                    print("Caught error in loop - disconnecting", sys.exc_info()[0])
        print("Channel exiting")

    def _process_commands(self):
        while not self.command_queue.empty():
            command = self.command_queue.get()
            # If we are not connected we just swallow any outstanding commands
            # This is so we don't end up with a back-log if the user does stuff
            # while not connected
            if self.is_connected():
                self.call_set_command(command[0],command[1])

    def _update_from_channel(self):
        self.set_voltage_val.set_value(round(
            float(self.call_get_command_float(self.VSET_COMMAND)),
            self.VOLTAGE_DECIMALS))
        self.set_current_val.set_value(round(
            float(self.call_get_command_float(self.ISET_COMMAND)),
            self.CURRENT_DECIMALS))
        self.output_voltage_val.set_value(round(
            float(self.call_get_command_float(self.VOUT_COMMAND)),
            self.VOLTAGE_DECIMALS))
        self.output_current_val.set_value(round(
            float(self.call_get_command_float(self.IOUT_COMMAND)),
            self.CURRENT_DECIMALS))
        self.in_cc_mode_val.set_value(self.call_get_command_bool(self.CCMODE_COMMAND))
        self.enabled_val.set_value(self.call_get_command_bool(self.ENABLE_COMMAND))

        # Throttle temperature updates to one in 5
        if self.update_count == 5:
            self.temp_val.set_value(self.temp_sensor.read_temp())
            self.update_count = 0
        else:
            self.update_count += 1

    def _load_settings(self):
        self.loaded_settings = True
        self.set_set_voltage(self.channel_settings.get_voltage())
        self.set_set_current(self.channel_settings.get_current())

    def stop(self):
        self.pause_lock.acquire()
        self.paused = False
        self.pause_lock.notify()
        self.pause_lock.release()
        self.exit_event.set()
        self.join()

    def pause(self,paused):
        self.pause_lock.acquire()
        self.paused = paused
        self.pause_lock.notify()
        self.pause_lock.release()

    # Don't call this - the thread will manage the connection
    def _connect(self):
        try:
            self.serialPort = serial.Serial(
                port=self.usb_device_filename,
                baudrate=115200,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS )
        except Exception :
            print("Connect to %s failed" % self.usb_device_filename)
            self.serialPort = None

    def _close(self):
        self.serialPort = None

    def is_connected(self):
        return self.serialPort is not None;

    def get_set_voltage(self):
        return self.set_voltage_val.get_value()

    def set_set_voltage(self,voltage):
        self.channel_settings.set_voltage(voltage)
        self.command_queue.put((self.VSET_COMMAND,str(voltage)))

    def get_set_current(self):
        return self.set_current_val.get_value()

    def set_set_current(self,current):
        self.channel_settings.set_current(current)
        self.command_queue.put((self.ISET_COMMAND,str(current)))

    def get_output_voltage(self):
        return self.output_voltage_val.get_value()

    def get_output_current(self):
        return self.output_current_val.get_value()

    def in_cc_mode(self):
        return self.in_cc_mode_val.get_value()

    def enable(self,enabled):
        self.command_queue.put((self.ENABLE_COMMAND,str(enabled)))

    def set_voltage_dac(self,count):
        self.call_set_command(self.VDAC_COMMAND,("%x" % count))

    def get_voltage_adc(self):
        return int(self.call_get_command(self.VADC_COMMAND),16)

    def set_current_dac(self,count):
        self.call_set_command(self.IDAC_COMMAND,("%x" % count))

    def get_current_adc(self):
        return int(self.call_get_command(self.IADC_COMMAND),16)

    def is_enabled(self):
        return self.enabled_val.get_value()

    def get_temperature(self):
        return self.temp_sensor.read_temp()

    def set_voltage_dac_cal_points(self,numPoints):
        param = ("numpoints,%d" % numPoints)
        print("Setting %s=%s",(self.VOLTS_DAC_CAL_COMMAND,param))
        self.call_set_command(self.VOLTS_DAC_CAL_COMMAND,param)

    def set_voltage_dac_cal(self,index,code,measurement):
        param = ("%d,%x,%f" % (index,code,measurement))
        print("Setting %s=%s",(self.VOLTS_DAC_CAL_COMMAND,param))
        self.call_set_command(self.VOLTS_DAC_CAL_COMMAND,param)

    def save_voltage_dac_cal(self):
        print("Setting %s=%s",(self.VOLTS_DAC_CAL_COMMAND,"save"))
        self.call_set_command(self.VOLTS_DAC_CAL_COMMAND,"save")

    def set_voltage_adc_cal_points(self,numPoints):
        self.call_set_command(self.VOLTS_ADC_CAL_COMMAND,("numpoints,%d" % numPoints))

    def set_voltage_adc_cal(self,index,code,measurement):
        self.call_set_command(self.VOLTS_ADC_CAL_COMMAND,("%d,%x,%f" % (index,code,measurement)))

    def save_voltage_adc_cal(self):
        self.call_set_command(self.VOLTS_ADC_CAL_COMMAND,"save")

    def set_current_dac_cal_points(self,numPoints):
        self.call_set_command(self.CURRENT_DAC_CAL_COMMAND,("numpoints,%d" % numPoints))

    def set_current_dac_cal(self,index,code,measurement):
        self.call_set_command(self.CURRENT_DAC_CAL_COMMAND,("%d,%x,%f" % (index,code,measurement)))

    def save_current_dac_cal(self):
        self.call_set_command(self.CURRENT_DAC_CAL_COMMAND,"save")

    def set_current_adc_cal_points(self,numPoints):
        self.call_set_command(self.CURRENT_ADC_CAL_COMMAND,("numpoints,%d" % numPoints))

    def set_current_adc_cal(self,index,code,measurement):
        self.call_set_command(self.CURRENT_ADC_CAL_COMMAND,("%d,%x,%f" % (index,code,measurement)))

    def save_current_adc_cal(self):
        self.call_set_command(self.CURRENT_ADC_CAL_COMMAND,"save")

    def cal_enable(self,enabled):
        self.call_set_command(self.ENABLE_COMMAND,str(enabled))

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

