/* 
* CalibrationCommand.h
*
* Created: 11/09/2016 11:12:58 AM
* Author: tom
*/

#ifndef __CALIBRATIONCOMMAND_H__
#define __CALIBRATIONCOMMAND_H__

#include "Command.h"

/*
The CalibrationCommand is used to set the calibration constants for the
voltage/current DAC/ADCs.
*/
class CalibrationCommand : public Command
{
public:
    CalibrationCommand(const char *commandName,LabPSU *psu);
    
    virtual void handleSetCommand( const char *params );
    
    virtual void handleGetCommand() const;
	
protected:

	//
	//	Must be overridden by the specific Voltage/Current/ADC/DAC command
	//
	virtual Linearizer& getLinearizer() const=0;
	
	//
	//	Must be overridden by the specific voltage/current/ADC/DAC command
	//
	virtual void saveLinearizer() const=0;
};

class VoltageADCCalibrationCommand : public CalibrationCommand
{
public:
	VoltageADCCalibrationCommand(LabPSU *psu);
	
protected:

	virtual Linearizer& getLinearizer() const;
	
	virtual void saveLinearizer() const;
};

class VoltageDACCalibrationCommand : public CalibrationCommand
{
public:
	VoltageDACCalibrationCommand(LabPSU *psu);
	
protected:

	virtual Linearizer& getLinearizer() const;
	
	virtual void saveLinearizer() const;
};

class CurrentADCCalibrationCommand : public CalibrationCommand
{
public:
	CurrentADCCalibrationCommand(LabPSU *psu);
	
protected:

	virtual Linearizer& getLinearizer() const;
	
	virtual void saveLinearizer() const;
};

class CurrentDACCalibrationCommand : public CalibrationCommand
{
public:
	CurrentDACCalibrationCommand(LabPSU *psu);
	
protected:

	virtual Linearizer& getLinearizer() const;
	
	virtual void saveLinearizer() const;
};

#endif //__CALIBRATIONCOMMAND_H__
