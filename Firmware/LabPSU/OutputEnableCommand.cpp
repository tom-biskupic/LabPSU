/*
* OutputEnableCommand.cpp
*
* Created: 23/07/2015 12:40:08 AM
* Author: tom
*/

#include "OutputEnableCommand.h"
#include <string.h>
#include "stdio.h"

namespace
{
    const char *OUTPUT_ENABLE_COMMAND_NAME = "Enable";
}

OutputEnableCommand::OutputEnableCommand() : Command(OUTPUT_ENABLE_COMMAND_NAME)
{
}

void OutputEnableCommand::handleSetCommand( const char *params,LabPSU *labPSU )
{
    bool enable = false;
    parseBool(enable,params);
    
    labPSU->enableOutput(enable);
    printBool(enable);
}

void OutputEnableCommand::handleGetCommand(LabPSU *labPSU) const
{
    printBool(labPSU->isOutputEnabled());
}
