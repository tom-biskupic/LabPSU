from PowerSupplyChannelConnection import PowerSupplyChannelConnection
from ChannelSettings import ChannelSettings
from ChannelState import ChannelState
import PowerInputControl

def dump_state(state):
    print("Set Voltage %f" % state.get_set_voltage())
    print("Set Current %f" % state.get_set_current())
    print("Output Voltage %f" % state.get_output_voltage())
    print("Output Current %f" % state.get_output_current())
    print("In CC Mode %s" % ( "True" if state.in_cc_mode() else "False" ))
    print("Enabled %s" % ( "True" if state.is_enabled() else "False" ))

PowerInputControl.turnOnSupply()
connection = PowerSupplyChannelConnection(1,"/dev/ttyACM0")
settings = ChannelSettings(1)

connection.connect()
connection.init_from_settings(settings)

state = ChannelState()
connection.update_channel_state(state)
dump_state(state)
PowerInputControl.turnOffSupply()
