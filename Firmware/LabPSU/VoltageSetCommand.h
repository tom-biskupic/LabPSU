/* 
* VoltageSetCommand.h
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/


#ifndef __VOLTAGESETCOMMAND_H__
#define __VOLTAGESETCOMMAND_H__

#include "Command.h"
#include "LabPSU.h"

/*
The voltage set command is used to control the output voltage setpoint.
*/
class VoltageSetCommand : public Command
{
public:
    VoltageSetCommand();
    
    virtual void handleSetCommand( const char *params, LabPSU *labPSU );
    
    virtual void handleGetCommand(LabPSU *labPSU) const;
};

#endif //__VOLTAGESETCOMMAND_H__
