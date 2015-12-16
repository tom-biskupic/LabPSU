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

OutputEnableCommand::OutputEnableCommand(LabPSU *psu) : Command(OUTPUT_ENABLE_COMMAND_NAME,psu)
{
}

void OutputEnableCommand::handleSetCommand( const char *params )
{
	bool enable = false;
	parseBool(enable,params);
	
	m_psu->enableOutput(enable);
	printBool(enable);
}

void OutputEnableCommand::handleGetCommand() const
{
	printBool(m_psu->isOutputEnabled());
}
