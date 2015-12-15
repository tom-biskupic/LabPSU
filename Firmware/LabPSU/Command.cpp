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

Command::Command(const char *commandName, LabPSU *psu) : m_psu(psu), m_commandName(commandName)
{
	
}

bool Command::handlesThis( const char *command ) const
{
	return strncasecmp(m_commandName,command,strlen(m_commandName)) == 0;
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
