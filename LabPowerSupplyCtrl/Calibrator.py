from DMM import DMM
import threading
import time

__author__ = 'tom'


class Calibrator(threading.Thread):
    VOLTAGE = "Voltage"
    CURRENT = "Current"
    STEPS = 16
    WAIT_PERIOD = 1.0

    def make_step_table(self):
        if self.voltage_or_current == self.VOLTAGE:
            max_step = 0xffff;
        else:
            max_step = int(4.6 / 5.0 * 0xffff)

        step_size = max_step / (self.STEPS-2)

        #
        # We need more resolution down low so add a half-step at the
        # beginning and then continue evenly after that
        #
        for i in range(self.STEPS):
            if i == 0:
                self.steps.append(0)
            elif i == 1:
                self.steps.append(step_size/4)
            elif i == 2:
                self.steps.append(step_size / 2)
            else:
                self.steps.append((i-2) * step_size)

    def __init__(self, power_supply_channel, voltage_or_current, dmm_ip, callback):
        self.power_supply_channel = power_supply_channel
        self.voltage_or_current = voltage_or_current
        self.dmm = DMM()
        self.dmm.connect(dmm_ip)
        self.callback = callback
        self.step_num = 0
        self.steps = []

        self.make_step_table()
        self.exit_event = threading.Event()
        threading.Thread.__init__(self)
        self.values = []
        print("Calibrator type = ", voltage_or_current);

    def stop(self):
        self.exit_event.set()
        self.join()

    def write_voltage_cal_to_channel(self):
        self.power_supply_channel.set_voltage_dac_cal_points(self.STEPS)

        for i in range(self.STEPS):
            self.power_supply_channel.set_voltage_dac_cal(i, self.values[i][0], self.values[i][1])

        self.power_supply_channel.save_voltage_dac_cal()

        self.power_supply_channel.set_voltage_adc_cal_points(self.STEPS)

        for i in range(self.STEPS):
            self.power_supply_channel.set_voltage_adc_cal(i, self.values[i][2], self.values[i][1])

        self.power_supply_channel.save_voltage_adc_cal()

        self.power_supply_channel.cal_enable(False)

    def write_current_cal_to_channel(self):
        self.power_supply_channel.set_current_dac_cal_points(self.STEPS)

        for i in range(self.STEPS):
            self.power_supply_channel.set_current_dac_cal(i, self.values[i][0], self.values[i][1])

        self.power_supply_channel.save_current_dac_cal()

        self.power_supply_channel.set_current_adc_cal_points(self.STEPS)

        for i in range(self.STEPS):
            self.power_supply_channel.set_current_adc_cal(i, self.values[i][2], self.values[i][1])

        self.power_supply_channel.save_current_adc_cal()

        self.power_supply_channel.cal_enable(False)

    def run(self):
        self.power_supply_channel.cal_enable(True)

        while not self.exit_event.wait(self.WAIT_PERIOD) and not self.is_done():
            self.do_next()

        #
        # Write the cal table
        #
        if self.voltage_or_current == self.VOLTAGE:
            self.write_voltage_cal_to_channel()
        else:
            self.write_current_cal_to_channel()

        self.callback.complete(self.values)

    def is_done(self):
        return self.step_num >= self.STEPS

    def do_next(self):
        next_count = self.steps[self.step_num]
        # self.callback.status("0x%4X = " % self.next_count)
        if self.voltage_or_current == self.VOLTAGE:
            self.power_supply_channel.set_voltage_dac(next_count);
            time.sleep(2.0)
            dmm_reading = self.dmm.read_voltage()
            adc_reading = self.power_supply_channel.get_voltage_adc()
        else:
            self.power_supply_channel.set_current_dac(next_count);
            time.sleep(2.0)
            dmm_reading = self.dmm.read_current()
            adc_reading = self.power_supply_channel.get_current_adc()

        self.callback.status("0x%4X = %3.3f, ADC=0x%4X" % (next_count, dmm_reading, adc_reading))
        self.values.append((next_count, dmm_reading, adc_reading))
        self.callback.progress(int(float(self.step_num) / float(self.STEPS) * 100.0))
        self.step_num += 1
