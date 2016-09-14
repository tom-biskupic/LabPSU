/* 
* CurrentSetCommand.cpp
*
* Created: 23/07/2015 12:40:08 AM
* Author: tom
*/

#include "CurrentSetCommand.h"

namespace
{
    const char *CURRENT_SET_COMMAND_NAME = "ISet";
    const float MIN_CURRENT_RANGE = 0.0f;
}

CurrentSetCommand::CurrentSetCommand() : Command(CURRENT_SET_COMMAND_NAME)
{
}

void CurrentSetCommand::handleSetCommand( const char *params,LabPSU *labPSU )
{
    float amps;
    if ( parseFloatParam(amps,params) )
    {
        if ( withinRange(MIN_CURRENT_RANGE,labPSU->getMaxCurrentLimit(),amps))
        {
            labPSU->setCurrentLimit(amps);
            printFloat(labPSU->getCurrentLimit());
        }
    }
}

void CurrentSetCommand::handleGetCommand(LabPSU *labPSU) const
{
    printFloat(labPSU->getCurrentLimit());
}


