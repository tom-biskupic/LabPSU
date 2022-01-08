#
# This is an interface for accessing the calibration interfaces 
# for each channel. Different instances of this class are created
# for voltage and current on each channel
#
class ChannelCalControls:
    def set_dac(self,value):
        pass

    def get_adc(self):
        pass

    #
    #   This expects an array of triples
    #   where each triple is 
    #   (adc_count,measurement,dac_count)
    #
    def set_cal_points(self,points):
        pass