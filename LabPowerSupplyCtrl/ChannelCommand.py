
class ChannelCommand:
    def __init__(self,value):
        self.command_value = value

    def get_value(self):
        return self.command_value

class SetVoltageCommand(ChannelCommand):
    def __init__(self,value):
        super().__init__(value)

class SetCurrentCommand(ChannelCommand):
    def __init__(self,value):
        super().__init__(value)

class EnableCommand(ChannelCommand):
    def __init__(self,value):
        super().__init__(value)

