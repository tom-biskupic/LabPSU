/*
* OutputEnableCommand.h
*
* Created: 21/07/2015 1:10:31 AM
* Author: tom
*/


#ifndef __OUTPUT_ENABLE_COMMAND_H__
#define __OUTPUT_ENABLE_COMMAND_H__

#include "Command.h"
#include "LabPSU.h"

/*
The output enable command is used to enable/disable the output.
It effectively sets the output voltage to zero but when the output
is re-enabled it returns to the previous voltage/current limit.

Reading the command returns if the output is currently enabled or
not.
*/
class OutputEnableCommand : public Command
{
public:

	OutputEnableCommand(LabPSU *psu);

	virtual void handleSetCommand( const char *params );
	virtual void handleGetCommand() const;
};

#endif // __OUTPUT_ENABLE_COMMAND_H__
