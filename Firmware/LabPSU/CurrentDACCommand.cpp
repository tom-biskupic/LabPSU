/*
* CurrentDACCommand.cpp
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/

#include "CurrentDACCommand.h"
#include "stdio.h"
namespace
{
    const char *CURRENT_DAC_COMMAND_NAME = "IDAC";
    const unsigned int MIN_RANGE = 0;
    const unsigned int MAX_RANGE = 0xffff;
}

CurrentDACCommand::CurrentDACCommand() : Command(CURRENT_DAC_COMMAND_NAME)
{
    
}

void CurrentDACCommand::handleSetCommand( const char *params, LabPSU *labPSU )
{
    unsigned int count;
    if ( parseHexParam(count,params) )
    {
        if ( withinRange(MIN_RANGE,MAX_RANGE,count))
        {
            labPSU->setCurrentDACCount(count);
            printFloat(labPSU->getCurrentLimit());
        }
    }
}

void CurrentDACCommand::handleGetCommand(LabPSU *labPSU) const
{
    printFloat(labPSU->getCurrentLimit());
}

