import threading
import copy
__author__ = 'tom'


class LockedThing():

    def __init__(self,value=None):
        self.value = copy.copy(value)
        self.lock = threading.RLock()

    def set_value(self,value):
        with self.lock:
            self.value = copy.copy(value)

    def get_value(self):
        with self.lock:
            return copy.copy(self.value)
