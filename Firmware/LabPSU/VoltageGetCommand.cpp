/*
* VoltageGetCommand.cpp
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/

#include "VoltageGetCommand.h"
#include "stdio.h"

namespace
{
    const char *VOLTAGE_GET_COMMAND_NAME = "Vout";
}

VoltageGetCommand::VoltageGetCommand() : Command(VOLTAGE_GET_COMMAND_NAME)
{
    
}

void VoltageGetCommand::handleSetCommand( const char *params, LabPSU *labPSU )
{
}

void VoltageGetCommand::handleGetCommand(LabPSU *labPSU) const
{
    printFloat(labPSU->getOutputVoltage());
}

