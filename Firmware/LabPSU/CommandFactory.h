/* 
* CommandFactory.h
*
* Created: 21/07/2015 11:17:01 PM
* Author: tom
*/

#ifndef __COMMANDFACTORY_H__
#define __COMMANDFACTORY_H__

#include "LabPSU.h"
#include "VoltageSetCommand.h"
#include "CurrentSetCommand.h"
#include "VoltageGetCommand.h"
#include "CurrentGetCommand.h"
#include "CalibrateVoltageADCZeroCommand.h"

class CommandFactory
{
public:
	CommandFactory( LabPSU *psu );

	/*
	Returns the Command handler that will process the command
	with the name provided or NULL if the command is not
	supported.
	*/
	Command *getCommand( const char *commandName ) const;
		
private:

	/*
	Adds a command to the command array (used for searching)
	*/
	void addCommand( Command *command );
		
	//
	//	Maximum number of commands we can support. Update as required
	//
	enum 
	{
		MAX_COMMANDS=10
	};
	
	VoltageSetCommand	m_voltageSetCommand;
    CurrentSetCommand   m_currentSetCommand;
	VoltageGetCommand   m_voltageGetCommand;
	CurrentGetCommand   m_currentGetCommand;
    
    CalibrateVoltageADCZeroCommand m_calVoltageADCZeroCommand;
    
	Command				*m_commandArray[MAX_COMMANDS];
};

#endif //__COMMANDFACTORY_H__
