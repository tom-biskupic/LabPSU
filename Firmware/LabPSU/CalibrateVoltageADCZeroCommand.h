/* 
* CalibrateADCCommand.h
*
* Created: 9/22/2015 11:52:01 PM
* Author: tom
*/

#ifndef __CALIBRATE_VOLTAGE_ADC_COMMAND_H__
#define __CALIBRATE_VOLTAGE_ADC_COMMAND_H__

#include "Command.h"
#include "LabPSU.h"

/*
The CalibrateADCCommand is used to request that the ADC performs
a zero or full scale calibration and to read-back the calibration values
from the device
*/
class CalibrateVoltageADCZeroCommand : public Command
{
public:
    CalibrateVoltageADCZeroCommand(LabPSU *psu);
    
    virtual void handleSetCommand( const char *params );
    
    virtual void handleGetCommand() const;
};

#endif //__CALIBRATE_VOLTAGE_ADC_COMMAND_H__
