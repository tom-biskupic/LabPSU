import unittest
import serial

import sys
from unittest.mock import patch
from unittest.mock import Mock, MagicMock, call
from PowerSupplyChannelConnection import PowerSupplyChannelConnection
from ChannelState import ChannelState
from ChannelCommand import SetVoltageCommand, SetCurrentCommand, EnableCommand

class PowerSupplyChannelConnectionTest(unittest.TestCase):

    TEST_CHANNEL_NUMBER=1
    TEST_FILE='/dev/xyx'

    def setUp(self):
        serial_patcher = patch("serial.Serial")
        self.mock_serial = Mock()
        self.mock_serial_package = serial_patcher.start()
        self.addCleanup(serial_patcher.stop)
        self.mock_serial_package.return_value = self.mock_serial

        linearizer_patcher = patch('PowerSupplyChannelConnection.Linearizer')
        self.mock_voltage_adc_linearizer = MagicMock()
        self.mock_voltage_dac_linearizer = MagicMock()
        self.mock_current_adc_linearizer = MagicMock()
        self.mock_current_dac_linearizer = MagicMock()
        self.mock_linearizer_class_mock = linearizer_patcher.start()
        self.addCleanup(linearizer_patcher.stop)
        self.mock_linearizer_class_mock.side_effect = [   
            self.mock_voltage_dac_linearizer,
            self.mock_voltage_adc_linearizer,
            self.mock_current_dac_linearizer,
            self.mock_current_adc_linearizer ]

    @patch("serial.Serial")
    def test_connect(self,mock_serial_package):

        mock_serial = Mock()
        mock_serial_package.return_value = mock_serial
        fixture = PowerSupplyChannelConnection(self.TEST_CHANNEL_NUMBER,self.TEST_FILE)
        self.assertFalse(fixture.is_connected())
        fixture.connect()
        self.assertTrue(fixture.is_connected())
        mock_serial_package.assert_called_with(
                port=self.TEST_FILE,
                baudrate=115200,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS )
        self.assertTrue(fixture.is_connected())

    def test_connect_fails(self):
        self.mock_serial_package.side_effect = serial.SerialException()
        fixture = PowerSupplyChannelConnection(self.TEST_CHANNEL_NUMBER,self.TEST_FILE)
        fixture.connect()
        self.assertFalse(fixture.is_connected())

    def test_call_set_command(self):
        fixture = self._setupFixture()
        fixture.call_set_command('XX','YY')
        self._verify_set_command_mocks('XX','YY')

    def test_call_get_command(self):
        fixture = self._setupFixture()
        self._setup_get_mocks('XX','12345 ')
        self.assertEqual('12345',fixture.call_get_command('XX'))
        self._verify_get_mocks('XX')

    def test_init_from_settings_no_cal(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        self.assertEqual(fixture.get_set_voltage(),12.345)
        self.assertEqual(fixture.get_set_current(),1.234)

        # Voltage DAC constructor
        self.assertEqual(self.mock_linearizer_class_mock.mock_calls[0],call([(0,0.0), (0xffff,30.0)]))
        #Voltage ADC Constrctor
        self.assertEqual(self.mock_linearizer_class_mock.mock_calls[1],call([(0,0.0), (0xffff,30.0)]))
        #Current DAC Constrctor
        max_count=int(4.8/5.0*0xffff)
        self.assertEqual(self.mock_linearizer_class_mock.mock_calls[2],call([(0,0.0), (max_count,4.8)]))
        #Current ADC Constrctor
        self.assertEqual(self.mock_linearizer_class_mock.mock_calls[3],call([(0,0.0), (max_count,4.8)]))

    def _init_fixture(self,fixture):
        mock_channel_settings = Mock()
        mock_channel_settings.get_voltage_cal.return_value = []
        mock_channel_settings.get_current_cal.return_value = []
        mock_channel_settings.get_voltage.return_value = 12.345
        mock_channel_settings.get_current.return_value = 1.234
        fixture.init_from_settings(mock_channel_settings)

    def test_init_from_settings(self):
        fixture = self._setupFixture()
        max_count=int(4.8/5.0*0xffff)
        voltage_cal = [(0,0.0,0), (0xfffe,29.9,0xfffb)]
        current_cal = [(0,0.1,0), (max_count-5,4.7,max_count-1)]
        mock_channel_settings = Mock()
        mock_channel_settings.get_voltage_cal.return_value = voltage_cal
        mock_channel_settings.get_current_cal.return_value = current_cal
        mock_channel_settings.get_voltage.return_value = 12.345
        mock_channel_settings.get_current.return_value = 1.234
        fixture.init_from_settings(mock_channel_settings)
        self.assertEqual(fixture.get_set_voltage(),12.345)
        self.assertEqual(fixture.get_set_current(),1.234)

        # Voltage DAC constructor
        self.assertEqual(
            self.mock_linearizer_class_mock.mock_calls[0],
            call([(0,0.0), (0xfffe,29.9)]))
        #Voltage ADC Constrctor
        self.assertEqual(
            self.mock_linearizer_class_mock.mock_calls[1],
            call([(0,0.0), (0xfffb,29.9)]))
        #Current DAC Constrctor
        self.assertEqual(
            self.mock_linearizer_class_mock.mock_calls[2],
            call([(0,0.1), (max_count-5,4.7)]))
        #Current ADC Constrctor
        self.assertEqual(
            self.mock_linearizer_class_mock.mock_calls[3],
            call([(0,0.1), (max_count-1,4.7)]))

    def test_set_set_voltage_not_enabled(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        fixture.set_set_voltage(10.0)
        self.assertEqual(10.0,fixture.get_set_voltage())

    def test_set_set_voltage_enabled(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        fixture.set_is_enabled(True)
        self.mock_voltage_dac_linearizer.reset_mock()
        self.mock_serial.reset_mock()

        self.mock_voltage_dac_linearizer.value_to_code.return_value=12345
        fixture.set_set_voltage(10.0)
        self.assertEqual(10.0,fixture.get_set_voltage())
        self.mock_voltage_dac_linearizer.value_to_code.assert_called_with(10.0)
        self._verify_set_command_mocks(fixture.VDAC_COMMAND,("%x" % 12345))

    def test_set_set_current_not_enabled(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        fixture.set_set_current(3.0)
        self.assertEqual(3.0,fixture.get_set_current())

    def test_set_set_current_enabled(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        fixture.set_is_enabled(True)
        self.mock_current_dac_linearizer.reset_mock()
        self.mock_serial.reset_mock()

        self.mock_current_dac_linearizer.value_to_code.return_value=12345
        fixture.set_set_current(4.0)
        self.assertEqual(4.0,fixture.get_set_current())
        self.mock_current_dac_linearizer.value_to_code.assert_called_with(4.0)
        self._verify_set_command_mocks(fixture.IDAC_COMMAND,("%x" % 12345))

    def test_get_outputvoltage(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        self._setup_get_mocks(fixture.VADC_COMMAND,'abcd123')
        self.mock_voltage_adc_linearizer.code_to_value.return_value=12.0
        self.assertEqual(12.0,fixture.get_output_voltage())
        self.mock_voltage_adc_linearizer.code_to_value.assert_called_with(0xabcd123)
        self._verify_get_mocks(fixture.VADC_COMMAND)

    def test_get_outputcurrent(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        self._setup_get_mocks(fixture.IADC_COMMAND,'abcd123')
        self.mock_current_adc_linearizer.code_to_value.return_value=2.0
        self.assertEqual(2.0,fixture.get_output_current())
        self.mock_current_adc_linearizer.code_to_value.assert_called_with(0xabcd123)
        self._verify_get_mocks(fixture.IADC_COMMAND)

    def test_get_cc_mode(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        fixture.set_is_enabled(True)
        self.mock_serial.reset_mock()

        self._setup_get_mocks(fixture.CCMODE_COMMAND,'True')
        self.assertEqual(True,fixture.get_ccmode())
        self._verify_get_mocks(fixture.CCMODE_COMMAND)

    # def test_update_channel_state(self):
    #     fixture = self._setupFixture()
    #     side_effects = [
    #         b'',
    #         (fixture.VSET_COMMAND+"=12.5").encode('utf-8'),
    #         b'',
    #         (fixture.ISET_COMMAND+'=2.5').encode('utf-8'),
    #         b'',
    #         (fixture.VOUT_COMMAND+'=12.4').encode('utf-8'),
    #         b'',
    #         (fixture.IOUT_COMMAND+'=2.4').encode('utf-8'),
    #         b'',
    #         (fixture.CCMODE_COMMAND+'=False').encode('utf-8'),
    #         b'',
    #         (fixture.ENABLE_COMMAND+'=True').encode('utf-8') ]
    #     self.mock_serial.readline.side_effect = side_effects

    #     channelState = ChannelState()
    #     fixture.update_channel_state(channelState)
    #     self.assertEqual(12.5,channelState.get_set_voltage())
    #     self.assertEqual(2.5,channelState.get_set_current())
    #     self.assertEqual(12.4,channelState.get_output_voltage())
    #     self.assertEqual(2.4,channelState.get_output_current())
    #     self.assertTrue(channelState.is_enabled())
    #     self.assertFalse(channelState.in_cc_mode())

    def test_set_voltage_command(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        fixture.set_is_enabled(True)
        self.mock_voltage_dac_linearizer.reset_mock()
        self.mock_serial.reset_mock()

        self.mock_voltage_dac_linearizer.value_to_code.return_value=12345
        fixture.execute_command(SetVoltageCommand(10.0))
        self.assertEqual(10.0,fixture.get_set_voltage())
        self.mock_voltage_dac_linearizer.value_to_code.assert_called_with(10.0)
        self._verify_set_command_mocks(fixture.VDAC_COMMAND,("%x" % 12345))

    def test_set_current_command(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        fixture.set_is_enabled(True)
        self.mock_current_dac_linearizer.reset_mock()
        self.mock_serial.reset_mock()

        self.mock_current_dac_linearizer.value_to_code.return_value=12345
        fixture.execute_command(SetCurrentCommand(4.0))
        self.assertEqual(4.0,fixture.get_set_current())
        self.mock_current_dac_linearizer.value_to_code.assert_called_with(4.0)
        self._verify_set_command_mocks(fixture.IDAC_COMMAND,("%x" % 12345))

    def test_enable_command(self):
        fixture = self._setupFixture()
        self._init_fixture(fixture)
        self.mock_serial.reset_mock()
        fixture.execute_command(EnableCommand(True))
        self.assertTrue(fixture.get_is_enabled())

    def test_get_command_bool(self):
        fixture = self._setupFixture()
        self._setup_get_mocks('XX','true')
        self.assertEqual(True,fixture.call_get_command_bool('XX'))
        self._verify_get_mocks('XX')

    def test_voltage_set_dac(self):
        fixture = self._setupFixture()
        voltage_cal_control = fixture.get_voltage_cal_controls(None)
        voltage_cal_control.set_dac(123)
        self._verify_set_command_mocks(fixture.VDAC_COMMAND,("%x" % 123))

    def test_voltage_get_adc(self):
        fixture = self._setupFixture()
        self._setup_get_mocks(fixture.VADC_COMMAND,("%x" % 123))
        voltage_cal_control = fixture.get_voltage_cal_controls(None)
        result = voltage_cal_control.get_adc()
        self.assertEqual(123,result)

    def test_current_set_dac(self):
        fixture = self._setupFixture()
        current_cal_control = fixture.get_current_cal_controls(None)
        current_cal_control.set_dac(123)
        self._verify_set_command_mocks(fixture.IDAC_COMMAND,("%x" % 123))

    def test_current_get_adc(self):
        fixture = self._setupFixture()
        self._setup_get_mocks(fixture.IADC_COMMAND,("%x" % 123))
        current_cal_control = fixture.get_current_cal_controls(None)
        result = current_cal_control.get_adc()
        self.assertEqual(123,result)

    def _setup_get_mocks(self,test_var,test_value):
        self.mock_serial.readline.return_value = (test_var+"="+test_value).encode('utf-8')

    def _verify_get_mocks(self,test_var):
        self.mock_serial.write.assert_called_with((test_var+'=?\n').encode('utf-8'))
        self.assertEqual(2,self.mock_serial.readline.call_count)

    def _verify_set_command_mocks(self,test_var,test_value):
        self.mock_serial.write.assert_called_with((test_var+"="+test_value+"\n").encode('utf-8'))
        self.assertEqual(2,self.mock_serial.readline.call_count)

    def _setupFixture(self):
        fixture = PowerSupplyChannelConnection(self.TEST_CHANNEL_NUMBER,self.TEST_FILE)
        fixture.connect()
        return fixture

if __name__ == '__main__':
    unittest.main()