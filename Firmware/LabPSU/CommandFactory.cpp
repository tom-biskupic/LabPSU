/* 
* CommandFactory.cpp
*
* Created: 21/07/2015 11:17:00 PM
* Author: tom
*/


#include "CommandFactory.h"
#include "string.h"
#include "stdio.h"

CommandFactory::CommandFactory( LabPSU *psu ) 
	:	m_voltageSetCommand(psu), 
		m_currentSetCommand(psu), 
		m_voltageGetCommand(psu),
		m_currentGetCommand(psu),
        m_calVoltageADCZeroCommand(psu)
{
	memset(m_commandArray,'\0',sizeof(m_commandArray)/sizeof(m_commandArray[0]));
	addCommand(&m_voltageSetCommand);
    addCommand(&m_currentSetCommand);
	addCommand(&m_voltageGetCommand);
	addCommand(&m_currentGetCommand);
    addCommand(&m_calVoltageADCZeroCommand);
}

Command *CommandFactory::getCommand( const char *commandName ) const
{
	for(int i=0;i<MAX_COMMANDS;i++)
	{
		if ( m_commandArray[i] == NULL )
		{
			return NULL;
		}
		
		if ( m_commandArray[i]->handlesThis(commandName))
		{
			return m_commandArray[i];
		}
	}	
	
	return NULL;
}

void CommandFactory::addCommand( Command *command )
{
	for(int i=0;i<MAX_COMMANDS;i++)
	{
		if ( m_commandArray[i] == NULL )
		{
			m_commandArray[i] = command;
			return;
		}
	}
	
	printf("Failed to initialize command array - array too small");
}
