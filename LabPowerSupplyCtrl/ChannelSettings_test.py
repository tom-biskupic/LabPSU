import unittest
import builtins
import os

from unittest.mock import patch
from unittest.mock import mock_open, Mock, MagicMock
from ChannelSettings import ChannelSettings

class ChannelSettingsTest(unittest.TestCase):
    CHANNEL_NUMBER=69
    TEST_FILE_NAME='TestConfig.cfg'

    @patch("configparser.ConfigParser")
    @patch("os.path")
    def test_init_existing_file(self,os_path_mock,ConfigParserMockClass):
        os_path_mock.expanduser.return_value = self.TEST_FILE_NAME
        os_path_mock.isfile.return_value = True
        mock_config_parser = Mock()
        ConfigParserMockClass.return_value = mock_config_parser
        mock_config_parser.has_section.return_value = True
        fixture = ChannelSettings(self.CHANNEL_NUMBER)
        os_path_mock.expanduser.assert_called_with("~/.labpschannel"+str(self.CHANNEL_NUMBER)+".cfg")
        os_path_mock.isfile.assert_called_with(self.TEST_FILE_NAME)
        mock_config_parser.read.assert_called_with(self.TEST_FILE_NAME)

    @patch("configparser.ConfigParser")
    @patch("os.path")
    def test_init_new_file(self,os_path_mock,ConfigParserMockClass):
        os_path_mock.expanduser.return_value = self.TEST_FILE_NAME
        os_path_mock.isfile.return_value = False
        mock_config_parser = Mock()
        ConfigParserMockClass.return_value = mock_config_parser
        mock_config_parser.has_section.return_value = True
        fixture = ChannelSettings(self.CHANNEL_NUMBER)
        os_path_mock.expanduser.assert_called_with("~/.labpschannel"+str(self.CHANNEL_NUMBER)+".cfg")
        os_path_mock.isfile.assert_called_with(self.TEST_FILE_NAME)
        mock_config_parser.read.assert_not_called()

    #
    # Figures out the config file name and removes the file if it exists
    #
    def _cleanup_config_file(self,channel_number):
        config_filename = os.path.expanduser("~/.labpschannel"+str(self.CHANNEL_NUMBER)+".cfg")
        if os.path.exists(config_filename):
            os.remove(config_filename)

    #
    # This test runs without mocking as I think it is more useful
    #
    def test_voltage(self):
        self._cleanup_config_file(self.CHANNEL_NUMBER)
        fixture = ChannelSettings(self.CHANNEL_NUMBER)

        #
        #    No settings so the default is returned
        #
        self.assertEqual(10.0,fixture.get_voltage())
        fixture.set_voltage(12.345)
        fixture = None
        fixture = ChannelSettings(self.CHANNEL_NUMBER)
        self.assertEqual(12.345,fixture.get_voltage())

    def test_current(self):
        self._cleanup_config_file(self.CHANNEL_NUMBER)
        fixture = ChannelSettings(self.CHANNEL_NUMBER)
        self.assertEqual(1.0,fixture.get_current())
        fixture.set_current(3.45)
        fixture = None
        fixture = ChannelSettings(self.CHANNEL_NUMBER)
        self.assertEqual(3.45,fixture.get_current())

    def test_voltage_cal(self):
        self._cleanup_config_file(self.CHANNEL_NUMBER)
        fixture = ChannelSettings(self.CHANNEL_NUMBER)
        self.assertEqual([],fixture.get_voltage_cal())
        cal_table = [ (0xfefa, 2.0, 0xad ), (0x123 ,3.1, 0xda) ]
        fixture.set_voltage_cal(cal_table)
        fixture = None
        fixture = ChannelSettings(self.CHANNEL_NUMBER)
        self.assertEqual(cal_table,fixture.get_voltage_cal())

if __name__ == '__main__':
    unittest.main()
