from DMM import DMM
import threading
import time

__author__ = 'tom'


class Calibrator(threading.Thread):
    VOLTAGE="Voltage"
    CURRENT="Current"
    STEPS=16
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
            self.max = int(4.6/5.0 * 0xffff)

        self.step = self.max / self.STEPS
        self.step_num=0
        self.exit_event = threading.Event()
        threading.Thread.__init__(self)
        self.values = []

    def stop(self):
        self.exit_event.set()
        self.join()

    def run(self):
        self.power_supply_channel.cal_enable(True)

        while not self.exit_event.wait(self.WAIT_PERIOD) and not self.is_done():
            self.do_next()

        #
        # Write the cal table
        #
        for i in range(self.STEPS):
            if self.voltage_or_current == self.VOLTAGE:
                self.power_supply_channel.set_voltage_dac_cal_points(self.STEPS)
                self.power_supply_channel.set_voltage_dac_cal(i,self.values[i][0],self.values[i][1])
            else:
                self.power_supply_channel.set_current_dac_cal_points(self.STEPS)
                self.power_supply_channel.set_current_dac_cal(i,self.values[i][0],self.values[i][1])

        if self.voltage_or_current == self.VOLTAGE:
            self.power_supply_channel.save_voltage_dac_cal()
        else:
            self.power_supply_channel.save_current_dac_cal()

        for i in range(self.STEPS):
            if self.voltage_or_current == self.VOLTAGE:
                self.power_supply_channel.set_voltage_adc_cal_points(self.STEPS)
                self.power_supply_channel.set_voltage_adc_cal(i,self.values[i][2],self.values[i][1])
            else:
                self.power_supply_channel.set_current_adc_cal_points(self.STEPS)
                self.power_supply_channel.set_current_adc_cal(i,self.values[i][2],self.values[i][1])

        if self.voltage_or_current == self.VOLTAGE:
            self.power_supply_channel.save_voltage_adc_cal()
        else:
            self.power_supply_channel.save_current_adc_cal()

        self.callback.complete(self.values)
        self.power_supply_channel.cal_enable(False)

    def is_done(self):
        return self.step_num >= self.STEPS

    def do_next(self):
        next_count = self.step_num * self.step
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

        self.callback.status("0x%4X = %3.3f, ADC=0x%4X" % (next_count,dmm_reading,adc_reading))
        self.values.append((next_count,dmm_reading,adc_reading))
        self.callback.progress( int(float(self.step_num)/float(self.STEPS) * 100.0))
        self.step_num += 1
