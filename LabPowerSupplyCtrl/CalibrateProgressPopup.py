
from kivy.uix.popup import Popup
from kivy.properties import ObjectProperty
from DMM import DMM
from Calibrator import Calibrator

__author__ = 'tom'


class CalibrateProgressPopup(Popup):
    progress_value = ObjectProperty(0)
    current_status = ObjectProperty("")

    def __init__(self,power_supply_channel,dmm_ip,voltage_or_current):
        Popup.__init__(self)
        self.calibrator = Calibrator(power_supply_channel,voltage_or_current,dmm_ip,self)

    def open(self):
        Popup.open(self)
        self.calibrator.start()

    def status(self,status_string):
        self.current_status = status_string

    def progress(self,percent):
        print("Progress = %d" % int(percent))
        self.progress_value = int(percent)

    def complete(self,values):
        self.dismiss()

    def cancel(self):
        self.calibrator.stop()
        self.dismiss()
        pass
