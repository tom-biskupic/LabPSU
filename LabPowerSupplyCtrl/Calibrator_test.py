import unittest
import serial

import sys
from unittest.mock import patch
from unittest.mock import Mock, MagicMock, call
from Calibrator import Calibrator

class CalibratorTest(unittest.TestCase):

    TEST_IP = "1.2.3.4"

    @patch('DMM.DMM')
    def test_voltage_cal(self,mock_dmm_class):
        mock_channel = Mock()
        mock_callback = Mock()
        mock_cal_control = Mock()
        mock_dmm = Mock()
        mock_dmm_class.return_value = mock_dmm
        mock_dmm.read_voltage.return_value = 10.0

        mock_channel.get_voltage_cal_controls.return_value = mock_cal_control
        mock_cal_control.get_adc.return_value = 12

        fixture = Calibrator(mock_channel, Calibrator.VOLTAGE, self.TEST_IP, mock_callback,0.01,0.01)
        fixture.run()
        self.assertEqual(mock_cal_control.set_dac.call_count,Calibrator.STEPS)
        self.check_per_step_calls(mock_cal_control,mock_callback)

    @patch('DMM.DMM')
    def test_current_cal(self,mock_dmm_class):
        mock_channel = Mock()
        mock_callback = Mock()
        mock_cal_control = Mock()
        mock_dmm = Mock()
        mock_dmm_class.return_value = mock_dmm
        mock_dmm.read_current.return_value = 10.0

        mock_channel.get_current_cal_controls.return_value = mock_cal_control
        mock_cal_control.get_adc.return_value = 12

        fixture = Calibrator(mock_channel, Calibrator.CURRENT, self.TEST_IP, mock_callback,0.01,0.01)
        fixture.run()
        max_count = int((4.6/5.0)*0xffff)
        self.check_per_step_calls(mock_cal_control,mock_callback,max_count)

    def check_per_step_calls(self,mock_cal_control,mock_callback,max_count=0xffff):
        self.assertEqual(mock_cal_control.set_dac.call_count,Calibrator.STEPS)
        set_cal_args, set_cal_kwargs = mock_cal_control.set_cal_points.call_args
        for i in range(0,Calibrator.STEPS):
            next_count = int(max_count/(Calibrator.STEPS-1)*i)
            self.assertEqual(mock_cal_control.set_dac.mock_calls[i],call(next_count))
            self.assertEqual(mock_callback.status.mock_calls[i],call("0x%04X=%03.3f, ADC=0x%04X" % (next_count, 10.0, 12)))
            self.assertEqual(mock_callback.progress.mock_calls[i],call(int(float(i) / float(Calibrator.STEPS) * 100.0)))
            self.assertEqual(set_cal_args[0][i],(next_count, 10.0, 12))

if __name__ == '__main__':
    unittest.main()
