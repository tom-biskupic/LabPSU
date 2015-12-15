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

CurrentGetCommand::CurrentGetCommand(LabPSU *psu) : Command(CURRENT_GET_COMMAND_NAME,psu)
{
	
}

void CurrentGetCommand::handleSetCommand( const char *params )
{
}

void CurrentGetCommand::handleGetCommand() const
{
	printFloat(m_psu->getOutputCurrent());
}

