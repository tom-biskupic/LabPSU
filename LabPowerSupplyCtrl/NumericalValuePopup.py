__author__ = 'tom'

from kivy.uix.modalview import ModalView
from kivy.properties import ObjectProperty


class NumericalValuePopup(ModalView):

    value = ObjectProperty(None)
    editBox=ObjectProperty()
    current_mode=ObjectProperty(False)

    def set_value(self,value):
        self.value = value
        self.editBox.text = "%.3f" % value

    def set_current_mode(self,value):
        self.current_mode=value

    def get_value(self):
        return self.value

    def mv_pressed(self):
        self.value = float(self.editBox.text)/1000.00
        self.dismiss()

    def v_pressed(self):
        self.value = float(self.editBox.text)
        self.dismiss()

    def cancel_pressed(self):
        self.dismiss()

    def key_pressed(self,value):
        self.editBox.text = self.editBox.text + value
        pass

