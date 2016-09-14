__author__ = 'tom'

from kivy.uix.popup import Popup
from kivy.properties import ObjectProperty
from DMM import DMM
from Calibrator import Calibrator
import traceback

__author__ = 'tom'

class CalibratePopup(Popup):
    dmm_ip = ObjectProperty("")
    dmm_error = ObjectProperty("")

    def __init__(self,channel_control):
        Popup.__init__(self)
        self.dmm_ip.text = "K34461A"
        self.channel_control = channel_control
        self.calibration_type = None

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
            self.dmm_ip_value = self.dmm_ip.text
            self.calibration_type = Calibrator.VOLTAGE
            self.dismiss()

    def get_dmm_ip(self):
        return self.dmm_ip_value

    def get_calibration_type(self):
        return self.calibration_type

    def calibrate_current(self):
        pass
