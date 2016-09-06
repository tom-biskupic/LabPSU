from DMM import DMM
import threading
import time

__author__ = 'tom'


class Calibrator(threading.Thread):
    VOLTAGE="Voltage"
    CURRENT="Current"
    STEPS=32
    WAIT_PERIOD=1.0

    def __init__(self,power_supply_channel,voltage_or_current,dmm_ip,callback):
        self.power_supply_channel = power_supply_channel
        self.voltage_or_current = voltage_or_current
        self.dmm = DMM()
        self.dmm.connect(dmm_ip)
        self.callback = callback

        if voltage_or_current == self.VOLTAGE:
            self.max = 0xffff;
        else:
            self.max = int(4.8/5.0 * 0xffff)

        self.step = self.max / self.STEPS
        self.next = 0;
        self.exit_event = threading.Event()
        threading.Thread.__init__(self)
        self.values = []

    def stop(self):
        self.exit_event.set()
        self.join()

    def run(self):
        while not self.exit_event.wait(self.WAIT_PERIOD) and not self.is_done():
            self.do_next()
        self.callback.complete(self.values)

    def is_done(self):
        self.next < self.max

    def do_next(self):
        self.callback.status("0x%4X = " % self.next)
        if self.voltage_or_current == self.VOLTAGE:
            #self.power_supply_channel.set_voltage_dac(self.next);
            time.sleep(2.0)
            dmm_reading = self.dmm.read_voltage()
            #adc_reading = self.power_supply_channel.get_voltage_adc()
            adc_reading = 0.0
        else:
            #self.power_supply_channel.set_current_dac(self.next);
            time.sleep(2.0)
            dmm_reading = self.dmm.read_current()
            #adc_reading = self.power_supply_channel.get_current_adc()
            adc_reading = 0.0

        self.callback.status("0x%4X = %3.3f, ADC=0x%4X" % (self.next,dmm_reading,adc_reading))
        self.values.append((self.next,dmm_reading,adc_reading))
        self.next += self.step
        self.callback.progress( int((self.next/self.max) * 100.0))