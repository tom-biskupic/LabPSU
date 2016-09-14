/* 
* CommandInterpreter.cpp
*
* Created: 19/07/2015 1:29:08 AM
* Author: tom
*/

#include "CommandInterpreter.h"
#include "VoltageSetCommand.h"

#include "string.h"
#include "stdio.h"

namespace
{
    const int LINE_SIZE=50;
    const int PARAM_SIZE=40;
    const int COMMAND_SIZE=20;
    
    const char EQUALS = '=';
    
    const char *INVALID_SYNTAX = "Invalid syntax. Expected command=param\r\n";
    const char *UNKNOWN_COMMAND_ERROR = "Invalid command\r\n";
}

CommandInterpreter::CommandInterpreter(LabPSU *psu) : m_commandFactory(), m_psu(psu)
{
    
}

void CommandInterpreter::processNextCommand()
{
    char nextLine[LINE_SIZE];
    
    readLine(nextLine,LINE_SIZE);
       
    if ( nextLine[0] != '\0' )
    {                    
        //
        //	Split the line into command and param
        //
        printf("%s\r\n",nextLine);
        
        char commandName[COMMAND_SIZE];
        char param[PARAM_SIZE];
        
        if ( parseLine(nextLine,commandName,param) )
        {
            Command *commandHandler = m_commandFactory.getCommand(commandName);
            
            if ( commandHandler == NULL )
            {
                printf(UNKNOWN_COMMAND_ERROR);
                return;
            }
            
            if ( param[0] == '?' )
            {
                commandHandler->handleGetCommand(m_psu);
            }
            else
            {
                commandHandler->handleSetCommand(param,m_psu);
            }
        }
    }
}

bool CommandInterpreter::parseLine( char *line, char *command, char *param ) const
{
    char *equalsPos = strchr(line,EQUALS);
        
    if ( equalsPos == NULL )
    {
        printf(INVALID_SYNTAX);
        return false;
    }
        
    int commandLen = (int)(equalsPos-line);
        
    if ( commandLen == 0 || commandLen > COMMAND_SIZE )
    {
        printf(INVALID_SYNTAX);
        return false;
    }
        
    int paramLen = strlen(line)-commandLen-1;

    if ( paramLen == 0 || paramLen > PARAM_SIZE )
    {
        printf(INVALID_SYNTAX);
        return false;
    }
        
    memcpy(command,line,commandLen);
    command[commandLen]='\0';
    memcpy(param,equalsPos+1,paramLen);
    param[paramLen] = '\0';
    return true;
}

void CommandInterpreter::readLine( char *buffer, size_t bufferLen ) const
{
    bool foundEnd=false;
    unsigned int nextPos = 0;
    
    while(!foundEnd && nextPos < bufferLen )
    {
        char c = getchar();
        
        if ( c == '\r' || c == '\n' )
        {
            foundEnd = true;
        }
        else if ( c != '\0' )
        {
            buffer[nextPos++] = c;
        }
    }
    
    buffer[nextPos]='\0';
}
