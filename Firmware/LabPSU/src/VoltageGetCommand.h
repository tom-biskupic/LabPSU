/*
* VoltageGetCommand.h
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/


#ifndef __VOLTAGE_GET_COMMAND_H__
#define __VOLTAGE_GET_COMMAND_H__

#include "Command.h"
#include "LabPSU.h"

/*
The voltage get command is used to read back the output volage via an ADC.
*/
class VoltageGetCommand : public Command
{
    public:
    VoltageGetCommand(LabPSU *psu);
    
    virtual void handleSetCommand( const char *params );
    
    virtual void handleGetCommand() const;
};

#endif //__VOLTAGE_GET_COMMAND_H__
