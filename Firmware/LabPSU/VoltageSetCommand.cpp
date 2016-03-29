/* 
* VoltageSetCommand.cpp
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/

#include "VoltageSetCommand.h"
#include "stdio.h"
namespace
{
    const char *VOLTAGE_SET_COMMAND_NAME = "VSet";
    const float MIN_VOLTAGE_RANGE = 0.0f;
    const float MAX_VOLTAGE_RANGE = 30.0f;
}

VoltageSetCommand::VoltageSetCommand(LabPSU *psu) : Command(VOLTAGE_SET_COMMAND_NAME,psu)
{
    
}

void VoltageSetCommand::handleSetCommand( const char *params )
{
    float volts;
    if ( parseFloatParam(volts,params) )
    {
        if ( withinRange(MIN_VOLTAGE_RANGE,MAX_VOLTAGE_RANGE,volts))
        {
            m_psu->setOutputVoltageLimit(volts);
            printFloat(m_psu->getOutputVoltageLimit());	
        }
    }
}

void VoltageSetCommand::handleGetCommand() const
{
    printFloat(m_psu->getOutputVoltageLimit());
}

