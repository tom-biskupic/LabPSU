/*
* CurrentADCCommand.h
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/

#ifndef __CURRENT_ADC_COMMAND_H__
#define __CURRENT_ADC_COMMAND_H__

#include "Command.h"
#include "LabPSU.h"

/*
The Current aDC command is used to directly read the count
on the current ADC. This is primarily used for calibration
*/
class CurrentADCCommand : public Command
{
public:
    CurrentADCCommand();
    
    virtual void handleSetCommand( const char *params, LabPSU *labPSU );
    
    virtual void handleGetCommand(LabPSU *labPSU) const;
};

#endif //__CURRENT_ADC_COMMAND_H__

