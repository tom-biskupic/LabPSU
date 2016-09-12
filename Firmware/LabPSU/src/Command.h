/* 
* Command.h
*
* Created: 20/07/2015 12:52:04 AM
* Author: tom
*/

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "LabPSU.h"

/*
Defines the interface to a command that can be executed
by the command interpreter.
*/
class Command
{
public:
    Command(const char *commaneName, LabPSU *psu);
    
    /*
    Returns true if the command name provided is handled
    by this command
    */
    virtual bool handlesThis( const char *command ) const;

    /*
    Implements a set command (i.e. one that changes the state
    and which can have a parameter).
    */
    virtual void handleSetCommand( const char *params );
    
    /*
    Handles a get command.
    */	
    virtual void handleGetCommand() const;
    
protected:

    /*
    Parses the string as a float and set the float with this value.
    If there was an error parsing the parameter the function returns false
    and an error is printed to stdout
    */
    bool parseFloatParam( float& value, const char *valueAsString) const;
    
    /*
    Parses the string as a hexadecimal value and sets the int with this value.
    If there was an error parsing the parameter the function returns false
    and an error is printed to stdout
    */    
    bool parseHexParam( unsigned int& value, const char *valueAsString) const;

    /*
    Parses the string as a boolean and sets the value accordingly.
    If the string contains the text 'true' in any mixture of case
    the value is true and it is false otherwise.
    */
    void parseBool(bool& value, const char *valueAsString) const;
    
    /*
    Returns true if the value is within the min..max range (inclusive).
    Will print an error if not
    */
    bool withinRange(float min,float max,float value) const;
        
    /*
    Returns true if the value is within the min..max range (inclusive).
    Will print an error if not
    */
    bool withinRange(unsigned int min,unsigned int max,unsigned int value) const;
    
    /*
    Prints the command name=value where the value is the value
    passed in converted to a float
    */
    void printFloat(float value) const;

    /*
    Prints the command name=value where the value is the value
    passed in converted to a boolean
    */
    void printBool(bool value) const;
    
    /*
    Prints the command name=value where the value passed is converted
    to a 4 digit hexadecimal.
    */
    void printHex( const uint16_t value ) const;

    LabPSU		*m_psu;
    const char	*m_commandName;
};

#endif //__COMMAND_H__
