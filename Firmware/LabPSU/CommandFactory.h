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
#include "OutputEnableCommand.h"
#include "InCurrentLimitCommand.h"
#include "VoltageDACCommand.h"
#include "VoltageADCCommand.h"
#include "CurrentDACCommand.h"
#include "CurrentADCCommand.h"
#include "CalibrationCommand.h"

class CommandFactory
{
public:
    CommandFactory();

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
        MAX_COMMANDS=16
    };
    
    VoltageSetCommand	m_voltageSetCommand;
    CurrentSetCommand   m_currentSetCommand;
    VoltageGetCommand   m_voltageGetCommand;
    CurrentGetCommand   m_currentGetCommand;
    OutputEnableCommand m_outputEnableCommand;
    InCurrentLimitCommand m_inCurrentLimitCommand;
    VoltageDACCommand   m_voltageDACCommand;
    VoltageADCCommand   m_voltageADCCommand;
    CurrentDACCommand   m_currentDACCommand;
    CurrentADCCommand   m_currentADCCommand;
    VoltageADCCalibrationCommand m_voltageADCCalCommand;
	VoltageDACCalibrationCommand m_voltageDACCalCommand;
	CurrentADCCalibrationCommand m_currentADCCalCommand;
	CurrentDACCalibrationCommand m_currentDACCalCommand;
	
    Command				*m_commandArray[MAX_COMMANDS];
};

#endif //__COMMANDFACTORY_H__
