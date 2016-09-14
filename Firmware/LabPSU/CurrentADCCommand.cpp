/*
* CurrentADCCommand.cpp
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/

#include "CurrentADCCommand.h"
#include "stdio.h"
namespace
{
    const char *CURRENT_ADC_COMMAND_NAME = "IADC";
}

CurrentADCCommand::CurrentADCCommand() : Command(CURRENT_ADC_COMMAND_NAME)
{
    
}

void CurrentADCCommand::handleSetCommand( const char *params,LabPSU *labPSU )
{

}

void CurrentADCCommand::handleGetCommand(LabPSU *labPSU) const
{
    printHex(labPSU->getOutputCurrentCount());
}

