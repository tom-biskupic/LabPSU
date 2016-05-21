/*
* CurrentDACCommand.h
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/

#ifndef __CURRENT_DAC_COMMAND_H__
#define __CURRENT_DAC_COMMAND_H__

#include "Command.h"
#include "LabPSU.h"

/*
The Current DAC command is used to directly control the count
on the output current DAC. This is primarily used for calibration
*/
class CurrentDACCommand : public Command
{
    public:
    CurrentDACCommand(LabPSU *psu);
    
    virtual void handleSetCommand( const char *params );
    
    virtual void handleGetCommand() const;
};

#endif //__CURRENT_DAC_COMMAND_H__

