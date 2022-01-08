
import serial
import threading
import sys
import os
import traceback

if sys.version_info[0] < 3:
    import Queue
else:
    import queue

from Temp import TempSensor
from LockedThing import LockedThing
from ChannelSettings import ChannelSettings
from ChannelState import ChannelState
from ChannelCommand import SetVoltageCommand, SetCurrentCommand, EnableCommand
from PowerSupplyChannelConnection import PowerSupplyChannelConnection

class PowerSupplyChannel(threading.Thread):
    POLL_PERIOD=0.1
    def __init__(self,channelNumber,file_name,temp_sensor_id):
        self.exit_event = threading.Event()
        self.connection = PowerSupplyChannelConnection(channelNumber,file_name)
        self.channel_state = ChannelState()
        self.temp_sensor = TempSensor(temp_sensor_id)

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

    def is_connected(self):
        return self.connection.is_connected()

    def run(self):
        while not self.exit_event.wait(self.POLL_PERIOD):
            self._check_paused()
            #
            # If we are not currently connected then try and connect
            #
            if not self.connection.is_connected():
                self.connection.connect()
                if self.connection.is_connected():
                    self._check_settings_loaded()

            # If we connected or if we already were connected then do
            # the update
            if self.connection.is_connected():
                try:
                    self._process_commands()
                    self._update_from_channel()
                except Exception as e:
                    # Something failed - just disconnect and re-connect
                    # on the next round
                    self.connection.close()
                    traceback.print_exc(file=sys.stdout)
                    self.loaded_settings = False
#                    exc_type, exc_obj, exc_tb = sys.exc_info()
#                    fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
#                    print("Caught error in loop - disconnecting", exc_type, fname, exc_tb.tb_lineno)
#                    print("Caught error in loop - disconnecting", str(e), sys.exc_info()[0])
        print("Channel exiting")

    def _check_settings_loaded(self):
        if not self.loaded_settings:
            self.connection.init_from_settings(self.channel_settings)
            self.loaded_settings = True

    def _check_paused(self):
        self.pause_lock.acquire()
        while self.paused:
            self.pause_lock.wait()
        self.pause_lock.release()

    def _process_commands(self):
        while not self.command_queue.empty():
            command = self.command_queue.get()
            # If we are not connected we just swallow any outstanding commands
            # This is so we don't end up with a back-log if the user does stuff
            # while not connected
            if self.connection.is_connected():
                self.connection.execute_command(command)

    def _update_from_channel(self):
        self.connection.update_channel_state(self.channel_state)

        # Throttle temperature updates to one in 5
        if self.update_count == 5:
            self.temp_val.set_value(self.temp_sensor.read_temp())
            self.update_count = 0
        else:
            self.update_count += 1

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

    def get_set_voltage(self):
        return self.channel_state.get_set_voltage()

    def set_set_voltage(self,voltage):
        self.channel_settings.set_voltage(voltage)
        self.command_queue.put(SetVoltageCommand(voltage))

    def get_set_current(self):
        return self.channel_state.get_set_current()

    def set_set_current(self,current):
        self.channel_settings.set_current(current)
        self.command_queue.put(SetCurrentCommand(current))

    def get_output_voltage(self):
        return self.channel_state.get_output_voltage()

    def get_output_current(self):
        return self.channel_state.get_output_current()

    def in_cc_mode(self):
        return self.channel_state.in_cc_mode()

    def enable(self,enabled):
        self.command_queue.put(EnableCommand(enabled))

    def is_enabled(self):
        return self.channel_state.is_enabled()

    #
    # Returns a ChannelCalControl for calibrating and storing
    # the calibration constants for voltage
    #
    def get_voltage_cal_controls(self):
        return self.connection.get_voltage_cal_controls(self.channel_settings)

    #
    # Returns a ChannelCalControl for calibrating and storing
    # the calibration constants for voltage
    #
    def get_current_cal_controls(self):
        return self.connection.get_current_cal_controls(self.channel_settings)

    def get_temperature(self):
         return self.temp_sensor.read_temp()

    def cal_enable(self,enabled):
        #
        # This looks redundant but if enabling cal mode you have to turn off
        # the thread first and if disabling cal mode then the reverse
        #
        if not enabled:
            self.connection.close()
            self.connection.connect()
            self.connection.set_is_enabled(False)
            self.pause(False)
        else:
            self.pause(True)
            self.connection.set_is_enabled(True)

