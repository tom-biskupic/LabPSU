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

VoltageADCCommand::VoltageADCCommand(LabPSU *psu) : Command(VOLTAGE_ADC_COMMAND_NAME,psu)
{
    
}

void VoltageADCCommand::handleSetCommand( const char *params )
{
}

void VoltageADCCommand::handleGetCommand() const
{
    printHex(m_psu->getOutputVoltageCount());
}

