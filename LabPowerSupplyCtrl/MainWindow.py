from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.uix.boxlayout import BoxLayout
from kivy.core.window import Window

from NumericalValuePopup import NumericalValuePopup


class ChannelControl(BoxLayout):

    set_voltage=ObjectProperty(30.0)
    set_current=ObjectProperty(3.0)

    enabled = ObjectProperty(False)
    in_current_limit = ObjectProperty(False)

    def enable_clicked(self):
        self.enabled = not self.enabled

    def set_voltage_callback(self,instance):
        self.set_voltage = instance.get_value()
        return False

    def set_current_callback(self,instance):
        self.set_current = instance.get_value()
        return False

    def set_voltage_clicked(self):
        view = NumericalValuePopup()
        view.set_value(self.set_voltage)
        view.set_current_mode(False)
        view.bind(on_dismiss=self.set_voltage_callback)
        view.open()
        pass

    def set_current_clicked(self):
        view = NumericalValuePopup()
        view.set_value(self.set_current)
        view.set_current_mode(True)
        view.bind(on_dismiss=self.set_current_callback)
        view.open()
        pass

class ControlWindow(BoxLayout):
    channel1 = ObjectProperty(None)
    channel2 = ObjectProperty(None)
    channel3 = ObjectProperty(None)


class LabPowerSupplyCtrlApp(App):

    def build(self):
        control_window = ControlWindow()
        Window.full_screen = 1
        Window.size = (800,600)
        return control_window


if __name__ == '__main__':
    LabPowerSupplyCtrlApp().run()