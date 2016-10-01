from kivy.properties import ObjectProperty
from kivy.clock import Clock
from kivy.config import Config
from kivy.uix.boxlayout import BoxLayout

from CalibratePopup import CalibratePopup
from CalibrateProgressPopup import CalibrateProgressPopup
from Calibrator import Calibrator

from NumericalValuePopup import NumericalValuePopup

__author__ = 'tom'


class ChannelControl(BoxLayout):

    set_voltage=ObjectProperty(30.0)
    set_current=ObjectProperty(3.0)
    output_voltage=ObjectProperty(30.0)
    output_current=ObjectProperty(3.0)
    temperature=ObjectProperty(25.0)

    enabled = ObjectProperty(False)
    in_current_limit = ObjectProperty(False)

    def enable_clicked(self,instance,value):
        if self.channel.is_connected():
            self.channel.enable(value);

    def set_voltage_callback(self,instance):
        if self.channel.is_connected() and instance.get_value() is not None:
            self.channel.set_set_voltage(instance.get_value())
        self.control_window.start_updates();
        return False

    def set_current_callback(self,instance):
        if self.channel.is_connected() and instance.get_value() is not None:
            self.channel.set_set_current(instance.get_value())
        self.control_window.start_updates();
        return False

    def set_voltage_clicked(self):
        self.control_window.stop_updates();
        view = NumericalValuePopup()
        #view.set_value(self.set_voltage)
        view.set_current_mode(False)
        view.bind(on_dismiss=self.set_voltage_callback)
        view.open()
        pass

    def set_current_clicked(self):
        self.control_window.stop_updates();
        view = NumericalValuePopup()
        #view.set_value(self.set_current)
        view.set_current_mode(True)
        view.bind(on_dismiss=self.set_current_callback)
        view.open()
        pass

    def calibrate_clicked(self):
        view = CalibratePopup(self)
        view.bind(on_dismiss=self.calibrate_chosen)
        view.open()

    def calibrate_chosen(self,instance):
        if instance.get_calibration_type() is not None:
            self.control_window.stop_updates()
            self.channel.pause(True)
            view = CalibrateProgressPopup(self.channel,instance.get_dmm_ip(),instance.get_calibration_type())
            view.bind(on_dismiss=self.calibrate_complete)
            view.open()

    def calibrate_complete(self,instance):
        self.channel.pause(False)
        self.control_window.start_updates();

    def bind_to_psu(self,channel,channel_number,fan_controller,control_window):
        self.channel = channel
        self.channel_number = channel_number
        self.fan_controller = fan_controller
        self.control_window = control_window

    def update_from_channel(self,dt):
        if self.channel.is_connected():
            set_voltage = self.channel.get_set_voltage()
            if self.set_voltage != set_voltage:
                self.set_voltate = set_voltage
            self.set_voltage = self.channel.get_set_voltage()
            self.set_current = self.channel.get_set_current()
            self.output_current = self.channel.get_output_current()
            self.output_voltage = self.channel.get_output_voltage()
            self.in_current_limit = self.channel.in_cc_mode()
            self.enabled = self.channel.is_enabled()

        return True

    def update_temperature(self,dt):
        if self.channel.is_connected():
            self.temperature = self.channel.get_temperature()
            self.fan_controller.set_temp(self.temperature,self.channel_number)
        return True

    def start_updates(self):
        Clock.schedule_interval(self.update_from_channel,0.25)
        Clock.schedule_interval(self.update_temperature,10)

    def stop_updates(self):
        Clock.unschedule(self.update_from_channel)
        Clock.unschedule(self.update_temperature)
