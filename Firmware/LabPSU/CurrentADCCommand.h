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
    CurrentADCCommand(LabPSU *psu);
    
    virtual void handleSetCommand( const char *params );
    
    virtual void handleGetCommand() const;
};

#endif //__CURRENT_ADC_COMMAND_H__

