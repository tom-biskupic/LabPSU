
import time

class TempSensor():

    def __init__(self,sensor_id):
        self.sensor_id = sensor_id

    def read_temp_file(self):
        file_name = "/sys/bus/w1/devices/"+self.sensor_id+"/w1_slave"
        file = open(file_name,"r")
        lines = file.readlines()
        file.close()
        return lines

    def data_read(self,lines):
        yes_no = lines[0].split(" ")[11].strip()
        return (yes_no == "YES")

    def read_temp(self):
       got_read = False
       while(not got_read):
           file_contents = self.read_temp_file()
           got_read = self.data_read(file_contents)
           if not got_read:
               print("Not got read!")

       temp_expr = file_contents[1].split(" ")[9]
       temp_string = temp_expr.split("=")[1]
       return float(temp_string.strip()) / 1000.0
      


