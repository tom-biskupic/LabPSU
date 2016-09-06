__author__ = 'tom'

from kivy.uix.popup import Popup
from kivy.properties import ObjectProperty
from DMM import DMM
import traceback

__author__ = 'tom'

class CalibratePopup(Popup):
    dmm_ip = ObjectProperty("K34461A")
    dmm_error = ObjectProperty("")

    def __init__(self,channel_control):
        Popup.__init__(self)
        self.channel_control = channel_control

    def check_dmm(self,dmm_address):
        dmm = DMM()
        try:
            dmm.connect(dmm_address)
            dmm.close()
            return True
        except Exception:
            traceback.print_exc()
            return False

    def calibrate_voltage(self):
        if not self.check_dmm(self.dmm_ip.text):
            self.dmm_error = "Cannot contact DMM"
        else:
            self.channel_control.calibrate_voltage(self.dmm_ip.text)
            self.dismiss()

    def calibrate_current(self):
        pass
