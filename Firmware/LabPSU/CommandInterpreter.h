/* 
* CommandInterpreter.h
*
* Created: 19/07/2015 1:29:08 AM
* Author: tom
*/

#ifndef __COMMANDINTERPRETER_H__
#define __COMMANDINTERPRETER_H__

#include "stddef.h"
#include "Command.h"
#include "CommandFactory.h"
#include "LabPSU.h"

/*
The command interpreter class reads commands from the serial line and then
executes the command by either modifying the state of the PowerSupply or
returning a parameter from the supply.

The command interpreter returns a status for each command. Invalid commands
cause an error to be returned.

The command syntax is pretty simple

param=value

Where the the parameter is a string specifying what device parameter
we are interested in.

The value is can either be a boolean value, (true/false case is not important)
or can be a numerical decimal value. If the value is a question mark (?) then
the command will return the current value of that parameter. Some parameters
are read-only so they can't be set.

The commands currently supported are
V - Sets the output voltage or returns the current voltage read by the ADC
Ilimit - Sets the current limit or returns the current limit value
I - You can't set the current but this will return the current produced by the device

*/
class CommandInterpreter
{
public:
    CommandInterpreter(LabPSU *psu);
    
    void processNextCommand();
    
private:

    /*
    Reads a line into the buffer. For some reason fgets didn't work
    so I am doing this instead.
    */
    void readLine( char *buffer, size_t bufferLen ) const;
    
    bool parseLine( char *line, char *command, char *param ) const;	
    
    CommandFactory  m_commandFactory;
    LabPSU          *m_psu;
};

#endif //__COMMANDINTERPRETER_H__
