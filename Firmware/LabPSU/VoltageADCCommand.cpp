/*
* VoltageADCCommand.cpp
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/

#include "VoltageADCCommand.h"
#include "stdio.h"
namespace
{
    const char *VOLTAGE_ADC_COMMAND_NAME = "VADC";
}

VoltageADCCommand::VoltageADCCommand() : Command(VOLTAGE_ADC_COMMAND_NAME)
{
    
}

void VoltageADCCommand::handleSetCommand( const char *params,LabPSU *labPSU )
{
}

void VoltageADCCommand::handleGetCommand(LabPSU *labPSU) const
{
    printHex(labPSU->getOutputVoltageCount());
}

