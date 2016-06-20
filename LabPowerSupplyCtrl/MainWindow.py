
from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.uix.boxlayout import BoxLayout
from kivy.core.window import Window
from kivy.clock import Clock
import RPi.GPIO as GPIO
import time

from NumericalValuePopup import NumericalValuePopup
from PowerSupplyChannel import PowerSupplyChannel

class ChannelControl(BoxLayout):

    set_voltage=ObjectProperty(30.0)
    set_current=ObjectProperty(3.0)
    output_voltage=ObjectProperty(30.0)
    output_current=ObjectProperty(3.0)

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
        view.set_value(self.set_voltage)
        view.set_current_mode(False)
        view.bind(on_dismiss=self.set_voltage_callback)
        view.open()
        pass

    def set_current_clicked(self):
        self.stop_updates();
        view = NumericalValuePopup()
        view.set_value(self.set_current)
        view.set_current_mode(True)
        view.bind(on_dismiss=self.set_current_callback)
        view.open()
        pass

    def bind_to_psu(self,channel):
        self.channel = channel

    def update_from_channel(self,dt):
        if self.channel.is_connected():
            self.set_voltage = self.channel.get_set_voltage()
            self.set_current = self.channel.get_set_current()
            self.output_current = self.channel.get_output_current()
            self.output_voltage = self.channel.get_output_voltage()
            self.in_current_limit = self.channel.in_cc_mode()
            self.enabled = self.channel.is_enabled()
        else:
            self.channel.connect()

    def start_updates(self):
        Clock.schedule_interval(self.update_from_channel,0.5)

    def stop_updates(self):
        Clock.unschedule(self.update_from_channel)


class ControlWindow(BoxLayout):
    channel1 = ObjectProperty(None)
    channel2 = ObjectProperty(None)
    channel3 = ObjectProperty(None)


    def bind_channels(self):
        # self.channel1.bind_to_psu( PowerSupplyChannel("/dev/cu.usbmodem1421"))
        self.channel1.bind_to_psu( PowerSupplyChannel("/dev/ttyACM0"))
        self.channel1.start_updates()
        self.channel2.bind_to_psu( PowerSupplyChannel("/dev/ttyACM1"))
	self.channel2.start_updates()
        #self.channel1.bind_to_psu( PowerSupplyChannel("/dev/tty.usbmodem1431"))
	pass

class ButtonSlice(BoxLayout):
    pass

class LabPowerSupplyCtrlApp(App):

    def build(self):
        control_window = ControlWindow()
        control_window.bind_channels()
        Window.full_screen = 1
        #Window.size = (800,600)
        return control_window


def setupIOs():
    print "Setting GPIO 23 and 24 to outputs"
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(23, GPIO.OUT)
    GPIO.setup(24, GPIO.OUT)

def turnOnSupply():
   print "Enabling 23"
   GPIO.output(23,True);
   time.sleep(1)
   print "Enabling 24"
   GPIO.output(24,True);
   time.sleep(1)
   pass


def turnOffSupply():
   print "Enabling 23"
   GPIO.output(23,False);
   GPIO.output(24,False);

if __name__ == '__main__':
     try:
        setupIOs()
        turnOnSupply()
        LabPowerSupplyCtrlApp().run()
     finally:
        turnOffSupply()
        GPIO.cleanup()

