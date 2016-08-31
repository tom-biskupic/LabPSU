#!/usr/bin/env python

from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.uix.boxlayout import BoxLayout
from kivy.core.window import Window
from kivy.clock import Clock
import time

from NumericalValuePopup import NumericalValuePopup
from PowerSupplyChannel import PowerSupplyChannel
from FanController import FanController

import PowerInputControl

class ChannelControl(BoxLayout):

    set_voltage=ObjectProperty(30.0)
    set_current=ObjectProperty(3.0)
    output_voltage=ObjectProperty(30.0)
    output_current=ObjectProperty(3.0)
    temperature=ObjectProperty(25.0)

    enabled = ObjectProperty(False)
    in_current_limit = ObjectProperty(False)

    def enable_clicked(self):
        if self.channel.is_connected():
            self.channel.enable(not self.enabled);

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

    def bind_to_psu(self,channel,channel_number,fan_controller):
        self.channel = channel
        self.channel_number = channel_number
        self.fan_controller = fan_controller

    def update_from_channel(self,dt):
        if self.channel.is_connected():
            try:
                print("Updating channel %s" % self.channel_number)
                set_voltage = self.channel.get_set_voltage()
                if self.set_voltage != set_voltage:
                    self.set_voltate = set_voltage
                self.set_voltage = self.channel.get_set_voltage()
                self.set_current = self.channel.get_set_current()
                self.output_current = self.channel.get_output_current()
                self.output_voltage = self.channel.get_output_voltage()
                self.in_current_limit = self.channel.in_cc_mode()
                self.enabled = self.channel.is_enabled()
                print("Updating channel %s completed" % self.channel_number)
            except Exception:
                print("Exception updating channel %s" % self.channel_number)
                self.channel.close()
                self.enabled=False
        else:
            self.channel.connect()
        return True

    def update_temperature(self,dt):
        if self.channel.is_connected():
            print("Updating channel %s temp" % self.channel_number)
            self.temperature = self.channel.get_temperature()
            self.fan_controller.set_temp(self.temperature,self.channel_number)
            print("Updating channel %s temp completed" % self.channel_number)
        return True

    def start_updates(self):
	Clock.schedule_interval(self.update_from_channel,0.25)
        Clock.schedule_interval(self.update_temperature,10)

    def stop_updates(self):
	print("Waiting for unschedule channel %s" % self.channel_number)
	Clock.unschedule(self.update_from_channel)
        Clock.unschedule(self.update_temperature)
        print("Unschedule completed channel %s" % self.channel_number)


class ControlWindow(BoxLayout):
    channel1 = ObjectProperty(None)
    channel2 = ObjectProperty(None)
    channel3 = ObjectProperty(None)


    def bind_channels(self,fan_controller):
        self.channel1.bind_to_psu( PowerSupplyChannel("/dev/ttyACM0","28-0415b237afff"),1,fan_controller)
        self.channel1.start_updates()
        self.channel2.bind_to_psu( PowerSupplyChannel("/dev/ttyACM1","28-0415b24bc8ff"),2,fan_controller)
        self.channel2.start_updates()
        self.channel3.bind_to_psu( PowerSupplyChannel("/dev/ttyACM2","28-0315b24341ff"),3,fan_controller)
        self.channel3.start_updates()

class ButtonSlice(BoxLayout):
    pass

class LabPowerSupplyCtrlApp(App):

    def build(self):
        control_window = ControlWindow()
        self.fan_controller = FanController()
        control_window.bind_channels(self.fan_controller)
        Window.full_screen = 1
#        Window.size = (800,600)
        return control_window

if __name__ == '__main__':
     try:
        PowerInputControl.turnOnSupply()
        LabPowerSupplyCtrlApp().run()
     finally:
        PowerInputControl.turnOffSupply()

