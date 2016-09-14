/* 
* VoltageADCCommand.h
*
* Created: 3/12/2016 12:17:40 AM
* Author: tom
*/


#ifndef __VOLTAGEADCCOMMAND_H__
#define __VOLTAGEADCCOMMAND_H__

#include "Command.h"

/*
The voltage ADC Command is used to read the current count from
the voltage ADC converter. This is used during PSU calibration.
*/
class VoltageADCCommand : public Command
{
public:
    VoltageADCCommand();
    
    virtual void handleSetCommand( const char *params, LabPSU *labPSU );
    
    virtual void handleGetCommand(LabPSU *labPSU) const;
        
};
#endif //__VOLTAGEADCCOMMAND_H__
