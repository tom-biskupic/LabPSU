/*
* VoltageDACCommand.cpp
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/

#include "VoltageDACCommand.h"
#include "stdio.h"
namespace
{
    const char *VOLTAGE_DAC_COMMAND_NAME = "VDAC";
    const unsigned int MIN_RANGE = 0;
    const unsigned int MAX_RANGE = 0xffff;
}

VoltageDACCommand::VoltageDACCommand(LabPSU *psu) : Command(VOLTAGE_DAC_COMMAND_NAME,psu)
{
    
}

void VoltageDACCommand::handleSetCommand( const char *params )
{
    unsigned int count;
    if ( parseHexParam(count,params) )
    {
        if ( withinRange(MIN_RANGE,MAX_RANGE,count))
        {
            m_psu->setVoltageDACCount(count);
            printFloat(m_psu->getOutputVoltageLimit());
        }
    }
}

void VoltageDACCommand::handleGetCommand() const
{
    printFloat(m_psu->getOutputVoltageLimit());
}

