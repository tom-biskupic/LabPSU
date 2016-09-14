/*
* InCurrentLimitCommand.h
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/


#ifndef __IN_CURRENT_LIMIT_COMMAND_H__
#define __IN_CURRENT_LIMIT_COMMAND_H__

#include "Command.h"
#include "LabPSU.h"

/*
A read-only command used to determine if we are in current limit
*/
class InCurrentLimitCommand : public Command
{
public:
    InCurrentLimitCommand();

    virtual void handleSetCommand( const char *params, LabPSU *labPSU );
    virtual void handleGetCommand(LabPSU *labPSU) const;
};

#endif // __IN_CURRENT_LIMIT_COMMAND_H__
