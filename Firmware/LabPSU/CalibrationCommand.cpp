/* 
* CalibrationCommand.cpp
*
* Created: 11/09/2016 11:12:58 AM
* Author: tom
*/

#include "CalibrationCommand.h"
#include "LineariserStorage.h"
#include <stdio.h>
#include <string.h>

namespace
{
	const char SAVE_COMMAND[] = "save";
	const char VOLTAGE_ADC_CAL_COMMAND[] = "VADCCal";
	const char VOLTAGE_DAC_CAL_COMMAND[] = "VDACCal";
	const char CURRENT_ADC_CAL_COMMAND[] = "IADCCal";
	const char CURRENT_DAC_CAL_COMMAND[] = "IDACCal";
}

CalibrationCommand::CalibrationCommand(const char *commandName,LabPSU *psu) : Command(commandName,psu)
{
	
}

void CalibrationCommand::handleSetCommand( const char *params )
{
	if ( strncasecmp(params,SAVE_COMMAND,sizeof(SAVE_COMMAND))==0)
	{
		saveLinearizer();
		printf("Linearizer saved\r\n");
	}
	else
	{
		int				index;
		unsigned int	code;
		float			value;
		
		if ( sscanf(params,"%d,%X,%f",&index,&code,&value) == 3 )
		{
			Linearizer::Point point;
			point.code = code;
			point.value = value;
			getLinearizer().setPoint(index,point);
		}
		else
		{
			printf("Error in parameter value. Expected index,code,value\r\n");
		}
	}
}

void CalibrationCommand::handleGetCommand() const
{
	Linearizer& linearizer = getLinearizer();
	for(uint16_t i=0;i<linearizer.getNumPoints();i++)
	{
		const Linearizer::Point& nextPoint = linearizer.getPoint(i);
		printf("0x%04X = 0x%5f\r\n",nextPoint.code,(double)nextPoint.value);
	}
}

VoltageADCCalibrationCommand::VoltageADCCalibrationCommand(LabPSU *psu) 
	: CalibrationCommand(VOLTAGE_ADC_CAL_COMMAND,psu)
{
	
}

Linearizer& VoltageADCCalibrationCommand::getLinearizer() const
{
	return m_psu->getVoltageADCLinearizer();
}

void VoltageADCCalibrationCommand::saveLinearizer() const
{
	LineariserStorage::saveLinearizer(
		LineariserStorage::VOLTAGE_ADC,
		m_psu->getVoltageADCLinearizer());
}

VoltageDACCalibrationCommand::VoltageDACCalibrationCommand(LabPSU *psu)
	: CalibrationCommand(VOLTAGE_DAC_CAL_COMMAND,psu)
{
		
}
	
Linearizer& VoltageDACCalibrationCommand::getLinearizer() const
{
	return m_psu->getVoltageDACLinearizer();
}
	
void VoltageDACCalibrationCommand::saveLinearizer() const
{
	LineariserStorage::saveLinearizer(
		LineariserStorage::VOLTAGE_DAC,
		m_psu->getVoltageDACLinearizer());
}

CurrentADCCalibrationCommand::CurrentADCCalibrationCommand(LabPSU *psu)
	: CalibrationCommand(CURRENT_ADC_CAL_COMMAND,psu)
{
		
}

Linearizer& CurrentADCCalibrationCommand::getLinearizer() const
{
	return m_psu->getCurrentADCLinearizer();	
}
	
void CurrentADCCalibrationCommand::saveLinearizer() const
{
	LineariserStorage::saveLinearizer(
		LineariserStorage::CURRENT_ADC,
		m_psu->getCurrentADCLinearizer());
}

CurrentDACCalibrationCommand::CurrentDACCalibrationCommand(LabPSU *psu)
	: CalibrationCommand(CURRENT_DAC_CAL_COMMAND,psu)
{
		
}
	
Linearizer& CurrentDACCalibrationCommand::getLinearizer() const
{
	return m_psu->getCurrentDACLinearizer();
}
	
void CurrentDACCalibrationCommand::saveLinearizer() const
{
	LineariserStorage::saveLinearizer(
		LineariserStorage::CURRENT_DAC,
		m_psu->getCurrentDACLinearizer());
}
