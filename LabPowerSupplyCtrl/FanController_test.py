import unittest

import sys
from unittest.mock import patch
from unittest.mock import Mock, MagicMock
from FanController import FanController

if "linux" in sys.platform:
    import RPi.GPIO as GPIO

class TestFanController(unittest.TestCase):

    @patch('RPi.GPIO.PWM')
    @patch('RPi.GPIO.setup')
    def test_setup(self,setup_mock,pwm_func_mock):
        mock_pwm = Mock()
        pwm_func_mock.return_value = mock_pwm
        fixture = FanController()
        setup_mock.assert_called_with(12,GPIO.OUT)
        pwm_func_mock.assert_called_with(12,100)
        mock_pwm.start.assert_called_with(0)

    @patch('RPi.GPIO.PWM')
    @patch('RPi.GPIO.setup')
    def test_set_temp_same(self,setup_mock,pwm_func_mock):
        mock_pwm = Mock()
        pwm_func_mock.return_value = mock_pwm
        fixture = FanController()
        fixture.set_temp(15.0,1)
        mock_pwm.ChangeDutyCycle.assert_not_called()

    @patch('RPi.GPIO.PWM')
    @patch('RPi.GPIO.setup')
    def test_set_temp_higher1(self,setup_mock,pwm_func_mock):
        mock_pwm = Mock()
        pwm_func_mock.return_value = mock_pwm
        fixture = FanController()
        fixture.set_temp(31.0,1)
        expected_duty = 50 + int((31 - 30.0) * 10)
        mock_pwm.ChangeDutyCycle.assert_called_with(expected_duty)

    @patch('RPi.GPIO.PWM')
    @patch('RPi.GPIO.setup')
    def test_set_temp_higher2(self,setup_mock,pwm_func_mock):
        mock_pwm = Mock()
        pwm_func_mock.return_value = mock_pwm
        fixture = FanController()
        fixture.set_temp(33.0,1)
        expected_duty = 50 + int((33 - 30.0) * 10)
        mock_pwm.ChangeDutyCycle.assert_called_with(expected_duty)

    @patch('RPi.GPIO.PWM')
    @patch('RPi.GPIO.setup')
    def test_set_temp_max(self,setup_mock,pwm_func_mock):
        mock_pwm = Mock()
        pwm_func_mock.return_value = mock_pwm
        fixture = FanController()
        fixture.set_temp(36.0,1)
        mock_pwm.ChangeDutyCycle.assert_called_with(100.0)


if __name__ == '__main__':
    unittest.main()
