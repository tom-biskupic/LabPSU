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

CurrentADCCommand::CurrentADCCommand(LabPSU *psu) : Command(CURRENT_ADC_COMMAND_NAME,psu)
{
    
}

void CurrentADCCommand::handleSetCommand( const char *params )
{

}

void CurrentADCCommand::handleGetCommand() const
{
    printHex(m_psu->getOutputCurrentCount());
}

