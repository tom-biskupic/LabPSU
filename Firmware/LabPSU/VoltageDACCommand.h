/*
* VoltageDACCommand.h
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/

#ifndef __VOLTAGE_DAC_COMMAND_H__
#define __VOLTAGE_DAC_COMMAND_H__

#include "Command.h"
#include "LabPSU.h"

/*
The voltage DAC command is used to directly control the count
on the output voltage DAC. This is primarily used for calibration
*/
class VoltageDACCommand : public Command
{
    public:
    VoltageDACCommand(LabPSU *psu);
    
    virtual void handleSetCommand( const char *params );
    
    virtual void handleGetCommand() const;
};

#endif //__VOLTAGE_DAC_COMMAND_H__

