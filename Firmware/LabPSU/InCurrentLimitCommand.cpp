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

InCurrentLimitCommand::InCurrentLimitCommand(LabPSU *psu) : Command(IN_CURRENT_LIMIT_COMMAND_NAME,psu)
{
}

void InCurrentLimitCommand::handleSetCommand( const char *params )
{
}

void InCurrentLimitCommand::handleGetCommand() const
{
    printBool(m_psu->isInCurrentLimit());
}
