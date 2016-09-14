/*
* InCurrentLimitCommand.cpp
*
* Created: 23/07/2015 12:40:08 AM
* Author: tom
*/

#include "InCurrentLimitCommand.h"
#include <string.h>
#include "stdio.h"

namespace
{
    const char *IN_CURRENT_LIMIT_COMMAND_NAME = "CCMode";
}

InCurrentLimitCommand::InCurrentLimitCommand() : Command(IN_CURRENT_LIMIT_COMMAND_NAME)
{
}

void InCurrentLimitCommand::handleSetCommand( const char *params,LabPSU *labPSU )
{
}

void InCurrentLimitCommand::handleGetCommand(LabPSU *labPSU) const
{
    printBool(labPSU->isInCurrentLimit());
}
