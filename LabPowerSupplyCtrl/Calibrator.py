import DMM
import threading
import time

__author__ = 'tom'


class Calibrator(threading.Thread):
    VOLTAGE = "Voltage"
    CURRENT = "Current"
    STEPS = 65

    def make_step_table(self):
        if self.voltage_or_current == self.VOLTAGE:
            max_step = 0xffff
        else:
            max_step = int(4.6 / 5.0 * 0xffff)

        step_size = max_step / (self.STEPS-1)

        #
        # Steps +1 so we get the max value
        #
        for i in range(self.STEPS):
            self.steps.append(int((i) * step_size))

    def __init__(self, power_supply_channel, voltage_or_current, dmm_ip, callback,loop_wait=1.0,dmm_wait=2.0):
        self.power_supply_channel = power_supply_channel
        self.voltage_or_current = voltage_or_current
        self.dmm = DMM.DMM()
        self.dmm.connect(dmm_ip)
        self.callback = callback
        self.step_num = 0
        self.steps = []
        self.dmm_wait = dmm_wait
        self.loop_wait = loop_wait

        if voltage_or_current == self.VOLTAGE:
            self.cal_control = power_supply_channel.get_voltage_cal_controls()
        else:
            self.cal_control = power_supply_channel.get_current_cal_controls()

        self.make_step_table()
        self.exit_event = threading.Event()
        threading.Thread.__init__(self)
        self.values = []

    def stop(self):
        self.exit_event.set()
        self.join()

    def run(self):
        self.power_supply_channel.cal_enable(True)

        while not self.exit_event.wait(self.loop_wait) and not self.is_done():
            self.do_next()

        if self.is_done():
            self.cal_control.set_cal_points(self.values)
            self.callback.complete(self.values)

        self.power_supply_channel.cal_enable(False)

    def is_done(self):
        return self.step_num >= self.STEPS

    def do_next(self):
        next_count = self.steps[self.step_num]
        # self.callback.status("0x%4X = " % self.next_count)
        self.cal_control.set_dac(next_count)
        time.sleep(self.dmm_wait)
        if self.voltage_or_current == self.VOLTAGE:
            dmm_reading = self.dmm.read_voltage()
        else:
            dmm_reading = self.dmm.read_current()

        adc_reading = self.cal_control.get_adc()

        self.callback.status("0x%04X=%03.3f, ADC=0x%04X" % (next_count, dmm_reading, adc_reading))
        self.values.append((next_count, dmm_reading, adc_reading))
        self.callback.progress(int(float(self.step_num) / float(self.STEPS) * 100.0))
        self.step_num += 1
