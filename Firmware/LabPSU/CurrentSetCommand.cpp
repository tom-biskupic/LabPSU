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

CurrentSetCommand::CurrentSetCommand(LabPSU *psu) : Command(CURRENT_SET_COMMAND_NAME,psu)
{
}

void CurrentSetCommand::handleSetCommand( const char *params )
{
    float amps;
    if ( parseFloatParam(amps,params) )
    {
        if ( withinRange(MIN_CURRENT_RANGE,m_psu->getMaxCurrentLimit(),amps))
        {
            m_psu->setCurrentLimit(amps);
            printFloat(m_psu->getCurrentLimit());
        }
    }
}

void CurrentSetCommand::handleGetCommand() const
{
    printFloat(m_psu->getCurrentLimit());
}


