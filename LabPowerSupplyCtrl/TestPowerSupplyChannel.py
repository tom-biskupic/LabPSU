__author__ = 'tom'


import unittest
import builtins

from unittest.mock import patch
from unittest.mock import mock_open, Mock, MagicMock

from PowerSupplyChannelAPI import PowerSupplyChannelAPI


class TestPowerSupplyChannel(unittest.TestCase):

    def make_fixture(self):
        with unittest.mock.patch('builtins.open') as m:
            channel = PowerSupplyChannelAPI("test")
            m.assert_called_once_with("test","r+")
            return channel

    def test_get_set_voltage(self):
        fixture = self.make_fixture()
        self.setup_get_expectaton(fixture,"Vset","30.123")
        self.assertEqual(fixture.get_set_voltage(),30.123)
        self.verify_get_called(fixture,"Vset")

    def test_set_set_voltage(self):
        fixture = self.make_fixture()
        fixture.set_set_voltage(10.0)
        self.setup_set_command_expectations(fixture,"Vset","10.0")

    def test_set_set_current(self):
        fixture = self.make_fixture()
        fixture.set_set_current(1.0)
        self.setup_set_command_expectations(fixture,"Iset","1.0")

    def test_get_set_current(self):
        fixture = self.make_fixture()
        self.setup_get_expectaton(fixture,"Iset","2.123")
        self.assertEqual(fixture.get_set_current(),2.123)
        self.verify_get_called(fixture,"Iset")

    def test_get_output_voltage(self):
        fixture = self.make_fixture()
        self.setup_get_expectaton(fixture,"Vout","21.123")
        self.assertEqual(fixture.get_output_voltage(),21.123)
        self.verify_get_called(fixture,"Vout")

    def test_get_output_current(self):
        fixture = self.make_fixture()
        self.setup_get_expectaton(fixture,"Iout","2.223")
        self.assertEqual(fixture.get_output_current(),2.223)
        self.verify_get_called(fixture,"Iout")

    def test_enable(self):
        fixture = self.make_fixture()
        fixture.enable(True)
        self.setup_set_command_expectations(fixture,"Enable","True")

    def test_is_enabled(self):
        fixture = self.make_fixture()
        self.setup_get_expectaton(fixture,"Enabled","False")
        self.assertEqual(fixture.is_enabled(),False)
        self.verify_get_called(fixture,"Enable")

    def setup_set_command_expectations(self,fixture,command_name,value):
        fixture.tty.write.assert_called_once_with("%s=%s\n" % ( command_name,value) )
        fixture.tty.readline.assert_called_once();

    def setup_get_expectaton(self,fixture,command,value):
        fixture.tty.readline.return_value = "%s=%s\n" % (command,value)

    def verify_get_called(self,fixture,command):
        fixture.tty.write.assert_called_once_with("%s=?\n" % command)

if __name__ == '__main__':
    unittest.main()
