/* 
* CommandFactory.cpp
*
* Created: 21/07/2015 11:17:00 PM
* Author: tom
*/


#include "CommandFactory.h"
#include "string.h"
#include "stdio.h"

CommandFactory::CommandFactory() 
    :	m_voltageSetCommand(), 
        m_currentSetCommand(), 
        m_voltageGetCommand(),
        m_currentGetCommand(),
        m_outputEnableCommand(),
        m_inCurrentLimitCommand(),
        m_voltageDACCommand(),
        m_voltageADCCommand(),
        m_currentDACCommand(),
        m_currentADCCommand(),
		m_voltageADCCalCommand(),
		m_voltageDACCalCommand(),
		m_currentADCCalCommand(),
		m_currentDACCalCommand()
{
    for(int i=0;i<MAX_COMMANDS;i++)
    {
        m_commandArray[i] = NULL;
    }
            
    addCommand(&m_voltageSetCommand);
    addCommand(&m_currentSetCommand);
    addCommand(&m_voltageGetCommand);
    addCommand(&m_currentGetCommand);
    addCommand(&m_outputEnableCommand);
    addCommand(&m_inCurrentLimitCommand);
    addCommand(&m_voltageDACCommand);
    addCommand(&m_voltageADCCommand);
    addCommand(&m_currentDACCommand);
    addCommand(&m_currentADCCommand);
	addCommand(&m_voltageADCCalCommand);
	addCommand(&m_voltageDACCalCommand);
	addCommand(&m_currentADCCalCommand);
	addCommand(&m_currentDACCalCommand);
}

Command *CommandFactory::getCommand( const char *commandName ) const
{
    for(int i=0;i<MAX_COMMANDS;i++)
    {
        Command *nextCommand = m_commandArray[i];
        
        if ( nextCommand == NULL )
        {
            return NULL;
        }
        
        if ( nextCommand->handlesThis(commandName))
        {
            return nextCommand;
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
