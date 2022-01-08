
from LockedThing import LockedThing

class ChannelState:

    def __init__(self):
        self.set_voltage_val = LockedThing(0.0)
        self.set_current_val = LockedThing(0.0)
        self.output_voltage_val = LockedThing(0.0)
        self.output_current_val = LockedThing(0.0)
        self.in_cc_mode_val = LockedThing(0.0)
        self.enabled_val = LockedThing(False)

    def get_set_voltage(self):
        return self.set_voltage_val.get_value()

    def set_set_voltage(self,value):
        self.set_voltage_val.set_value(value)

    def get_set_current(self):
        return self.set_current_val.get_value()

    def set_set_current(self,value):
        self.set_current_val.set_value(value)

    def get_output_voltage(self):
        return self.output_voltage_val.get_value()

    def set_output_voltage(self,value):
        self.output_voltage_val.set_value(value)

    def get_output_current(self):
        return self.output_current_val.get_value()

    def set_output_current(self,value):
        self.output_current_val.set_value(value)

    def in_cc_mode(self):
        return self.in_cc_mode_val.get_value()

    def set_in_cc_mode(self,value):
        self.in_cc_mode_val.set_value(value)

    def is_enabled(self):
        return self.enabled_val.get_value()

    def set_is_enabled(self,value):
        return self.enabled_val.set_value(value)
