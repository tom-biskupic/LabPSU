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
    CalibrationCommand(const char *commandName);
    
    virtual void handleSetCommand( const char *params,LabPSU *labPSU );
    
    virtual void handleGetCommand(LabPSU *labPSU) const;
	
protected:

	//
	//	Must be overridden by the specific Voltage/Current/ADC/DAC command
	//
	virtual Linearizer& getLinearizer(LabPSU *labPSU) const=0;
	
	//
	//	Must be overridden by the specific voltage/current/ADC/DAC command
	//
	virtual void saveLinearizer(LabPSU *labPSU) const=0;
};

class VoltageADCCalibrationCommand : public CalibrationCommand
{
public:
	VoltageADCCalibrationCommand();
	
protected:

	virtual Linearizer& getLinearizer(LabPSU *labPSU) const;
	
	virtual void saveLinearizer(LabPSU *labPSU) const;
};

class VoltageDACCalibrationCommand : public CalibrationCommand
{
public:
	VoltageDACCalibrationCommand();
	
protected:

	virtual Linearizer& getLinearizer(LabPSU *labPSU) const;
	
	virtual void saveLinearizer(LabPSU *labPSU) const;
};

class CurrentADCCalibrationCommand : public CalibrationCommand
{
public:
	CurrentADCCalibrationCommand();
	
protected:

	virtual Linearizer& getLinearizer(LabPSU *labPSU) const;
	
	virtual void saveLinearizer(LabPSU *labPSU) const;
};

class CurrentDACCalibrationCommand : public CalibrationCommand
{
public:
	CurrentDACCalibrationCommand();
	
protected:

	virtual Linearizer& getLinearizer(LabPSU *labPSU) const;
	
	virtual void saveLinearizer(LabPSU *labPSU) const;
};

#endif //__CALIBRATIONCOMMAND_H__
