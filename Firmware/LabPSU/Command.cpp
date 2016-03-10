/* 
* Command.cpp
*
* Created: 20/07/2015 12:52:04 AM
* Author: tom
*/

#include "Command.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

namespace
{
	const char *TRUE_NAME = "true";
	const char *FALSE_NAME = "false";
}

Command::Command(const char *commandName, LabPSU *psu) : m_psu(psu), m_commandName(commandName)
{
	
}

bool Command::handlesThis( const char *command ) const
{
	return strcasecmp(m_commandName,command) == 0;
}

void Command::handleSetCommand( const char *params )
{
    //
    //  Because AVR GCC doesn't handle pure-virtual functions for some reason...
    //
}    
	
void Command::handleGetCommand() const
{
    //
    //  Because AVR GCC doesn't handle pure-virtual functions for some reason...
    //
}

bool Command::parseFloatParam( float& value, const char *valueAsString) const
{
	if (  sscanf(valueAsString,"%f",&value) != 1 )
	{
        printf("Invalid param : %s\r\n",valueAsString);    
		return false;
	}
	//value = atof(valueAsString);
    
	return true;
}

bool Command::parseHexParam( unsigned int& value, const char *valueAsString) const
{
    if (  sscanf(valueAsString,"%x",&value) != 1 )
    {
        printf("Invalid param : %s\r\n",valueAsString);
        return false;
    }
    
    return true;
}

void Command::parseBool(bool& value, const char *valueAsString) const
{
	value = ( strcasecmp(valueAsString,TRUE_NAME) == 0 );
}

bool Command::withinRange(unsigned int min,unsigned int max,unsigned int value) const
{
    if ( value >= min && value <= max )
    {
        return true;
    }
    
    printf("%s : Value %d is outside of range %d to %d\r\n",m_commandName,value,min,max);
    return false;
}

bool Command::withinRange(float min,float max,float value) const
{
	if ( value >= min && value <= max )
	{
		return true;
	}
	
	printf("%s : Value %f is outside of range %f to %f\r\n",m_commandName,(double)value,(double)min,(double)max);
	return false;
}

void Command::printFloat(float value) const
{
	printf("%s=%f\r\n",m_commandName,(double)value);
}

void Command::printBool(bool value) const
{
	printf("%s=%s\r\n",m_commandName,value ? TRUE_NAME : FALSE_NAME);
}
