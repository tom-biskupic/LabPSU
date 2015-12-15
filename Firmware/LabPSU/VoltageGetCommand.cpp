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

VoltageGetCommand::VoltageGetCommand(LabPSU *psu) : Command(VOLTAGE_GET_COMMAND_NAME,psu)
{
	
}

void VoltageGetCommand::handleSetCommand( const char *params )
{
}

void VoltageGetCommand::handleGetCommand() const
{
	printFloat(m_psu->getOutputVoltage());
}

