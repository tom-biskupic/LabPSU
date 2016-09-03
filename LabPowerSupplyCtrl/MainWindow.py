#!/usr/bin/env python

from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.uix.boxlayout import BoxLayout
from kivy.core.window import Window
from kivy.clock import Clock
from kivy.config import Config
import time
import signal

from NumericalValuePopup import NumericalValuePopup
from PowerSupplyChannel import PowerSupplyChannel
from FanController import FanController
from CalibratePopup import CalibratePopup

import PowerInputControl

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
        self.start_updates();
        return False

    def set_current_callback(self,instance):
        if self.channel.is_connected() and instance.get_value() is not None:
            self.channel.set_set_current(instance.get_value())
        self.start_updates();
        return False

    def set_voltage_clicked(self):
        self.stop_updates();
        view = NumericalValuePopup()
        #view.set_value(self.set_voltage)
        view.set_current_mode(False)
        view.bind(on_dismiss=self.set_voltage_callback)
        view.open()
        pass

    def set_current_clicked(self):
        self.stop_updates();
        view = NumericalValuePopup()
        #view.set_value(self.set_current)
        view.set_current_mode(True)
        view.bind(on_dismiss=self.set_current_callback)
        view.open()
        pass

    def calibrate_clicked(self):
        view = CalibratePopup()
        view.open()

    def bind_to_psu(self,channel,channel_number,fan_controller):
        self.channel = channel
        self.channel_number = channel_number
        self.fan_controller = fan_controller

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


class ControlWindow(BoxLayout):
    channel1 = ObjectProperty(None)
    channel2 = ObjectProperty(None)
    channel3 = ObjectProperty(None)


    def bind_channels(self,fan_controller):
        self.psuChannel1 = PowerSupplyChannel("/dev/ttyACM0","28-0415b237afff")
        self.psuChannel1.start()
        self.psuChannel2 = PowerSupplyChannel("/dev/ttyACM1","28-0415b24bc8ff")
        self.psuChannel2.start()
        self.psuChannel3 = PowerSupplyChannel("/dev/ttyACM2","28-0315b24341ff")
        self.psuChannel3.start()

        self.channel1.bind_to_psu(self.psuChannel1,1,fan_controller)
        self.channel1.start_updates()
        self.channel2.bind_to_psu(self.psuChannel2,2,fan_controller)
        self.channel2.start_updates()
        self.channel3.bind_to_psu(self.psuChannel3,3,fan_controller)
        self.channel3.start_updates()

    def shutdown(self):
        self.psuChannel1.stop()
        self.psuChannel2.stop()
        self.psuChannel3.stop()

class ButtonSlice(BoxLayout):
    pass

class LabPowerSupplyCtrlApp(App):

    def __init__(self):
        App.__init__(self)
        self.control_window = None

    def build(self):
        self.control_window = ControlWindow()
        self.fan_controller = FanController()
        self.control_window.bind_channels(self.fan_controller)
        Window.full_screen = 1
#        Window.size = (800,600)
        return self.control_window

    def shutdown(self):
       if self.control_window is not None:
           self.control_window.shutdown()


if __name__ == '__main__':
    Config.set('kivy','keyboard_mode','systemandmulti')
    app = LabPowerSupplyCtrlApp()

    def closeStuff(signum,stack):
        app.shutdown()
        PowerInputControl.turnOffSupply()

    try:
        PowerInputControl.turnOnSupply()
        #signal.signal(signal.SIGINT,closeStuff)
        #signal.signal(signal.SIGHUP,closeStuff)
        app.run()
    finally:
        app.shutdown()
        PowerInputControl.turnOffSupply()
