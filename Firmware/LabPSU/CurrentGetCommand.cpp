/*
* CurrentGetCommand.cpp
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/

#include "CurrentGetCommand.h"
#include "stdio.h"

namespace
{
    const char *CURRENT_GET_COMMAND_NAME = "Iout";
}

CurrentGetCommand::CurrentGetCommand() : Command(CURRENT_GET_COMMAND_NAME)
{
    
}

void CurrentGetCommand::handleSetCommand( const char *params, LabPSU *labPSU )
{
}

void CurrentGetCommand::handleGetCommand(LabPSU *labPSU) const
{
    printFloat(labPSU->getOutputCurrent());
}

