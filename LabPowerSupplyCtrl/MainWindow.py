from kivy.app import App
from kivy.uix.widget import Widget
from kivy.properties import ObjectProperty
from kivy.uix.boxlayout import BoxLayout
from kivy.core.window import Window

class ChannelControl(BoxLayout):
    enabled=False
    def output_enabled(self):
        return self.enabled

    def enable_clicked(self):
        self.enabled = not self.enabled
        print(format(self.enabled))
    pass

class ControlWindow(BoxLayout):
    channel1 = ObjectProperty(None)
    channel2 = ObjectProperty(None)
    channel3 = ObjectProperty(None)
    pass


class LabPowerSupplyCtrlApp(App):
    def build(self):
        control_window = ControlWindow()
        Window.full_screen = 1
        Window.size = (800,600)
        return control_window


if __name__ == '__main__':
    LabPowerSupplyCtrlApp().run()