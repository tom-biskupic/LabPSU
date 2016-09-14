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

VoltageSetCommand::VoltageSetCommand() : Command(VOLTAGE_SET_COMMAND_NAME)
{
    
}

void VoltageSetCommand::handleSetCommand( const char *params, LabPSU *labPSU )
{
    float volts;
    if ( parseFloatParam(volts,params) )
    {
        if ( withinRange(MIN_VOLTAGE_RANGE,MAX_VOLTAGE_RANGE,volts))
        {
            labPSU->setOutputVoltageLimit(volts);
            printFloat(labPSU->getOutputVoltageLimit());	
        }
    }
}

void VoltageSetCommand::handleGetCommand(LabPSU *labPSU) const
{
    printFloat(labPSU->getOutputVoltageLimit());
}

