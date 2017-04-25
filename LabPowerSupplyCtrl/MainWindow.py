#!/usr/bin/env python

import sys
from kivy.app import App
from kivy.properties import ObjectProperty
from kivy.uix.boxlayout import BoxLayout
from kivy.core.window import Window

from PowerSupplyChannel import PowerSupplyChannel
from FanController import FanController
import PowerInputControl
from ChannelControl import ChannelControl

class ControlWindow(BoxLayout):
    channel1 = ObjectProperty(None)
    channel2 = ObjectProperty(None)
    channel3 = ObjectProperty(None)

    def bind_channels(self,fan_controller):
        try:
            print("Inside bind_channels")
            self.psuChannel1 = PowerSupplyChannel(
                1,"/dev/ttyACM0","28-0415b237afff")
            print("Starting channel1")
            self.psuChannel1.start()
            self.psuChannel2 = PowerSupplyChannel(
                2,"/dev/ttyACM1","28-0415b24bc8ff")
            print("Starting channel2")
            self.psuChannel2.start()
            self.psuChannel3 = PowerSupplyChannel(
                3,"/dev/ttyACM2","28-0315b24341ff")
            print("Starting channel3")
            self.psuChannel3.start()

            self.channel1.bind_to_psu(self.psuChannel1,1,fan_controller,self)
            self.channel2.bind_to_psu(self.psuChannel2,2,fan_controller,self)
            self.channel3.bind_to_psu(self.psuChannel3,3,fan_controller,self)

            self.start_updates()
        except:
            print("Unexpected error:", sys.exc_info()[0])
            raise

    def start_updates(self):
      self.channel1.start_updates()
      self.channel2.start_updates()
      self.channel3.start_updates()

    def stop_updates(self):
      self.channel1.stop_updates()
      self.channel2.stop_updates()
      self.channel3.stop_updates()

    def shutdown(self):
        if self.psuChannel1 is not None:
            self.psuChannel1.stop()
        if self.psuChannel2 is not None:
            self.psuChannel2.stop()
        if self.psuChannel3 is not None:
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
        return self.control_window

    def shutdown(self):
       if self.control_window is not None:
           self.control_window.shutdown()


if __name__ == '__main__':
    app = LabPowerSupplyCtrlApp()

    def closeStuff(signum,stack):
        app.shutdown()
        PowerInputControl.turnOffSupply()

    try:
        PowerInputControl.turnOnSupply()
        #signal.signal(signal.SIGINT,closeStuff)
        #signal.signal(signal.SIGHUP,closeStuff)
        app.run()
    except:
        print("Unexpected error:", sys.exc_info()[0])
        raise
    finally:
        app.shutdown()
        PowerInputControl.turnOffSupply()
