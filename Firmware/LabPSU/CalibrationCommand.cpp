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
    const char NUM_POINTS_COMMAND[] = "numpoints";
	const char VOLTAGE_ADC_CAL_COMMAND[] = "VADCCal";
	const char VOLTAGE_DAC_CAL_COMMAND[] = "VDACCal";
	const char CURRENT_ADC_CAL_COMMAND[] = "IADCCal";
	const char CURRENT_DAC_CAL_COMMAND[] = "IDACCal";
}

CalibrationCommand::CalibrationCommand(const char *commandName) : Command(commandName)
{
	
}

void CalibrationCommand::handleSetCommand( const char *params,LabPSU *labPSU )
{
	if ( strncasecmp(params,SAVE_COMMAND,sizeof(SAVE_COMMAND))==0)
	{
		saveLinearizer(labPSU);
		printf("Linearizer saved\r\n");
	}
	else if ( strncasecmp(params,NUM_POINTS_COMMAND,sizeof(NUM_POINTS_COMMAND)-1)==0 )
    {
        char    *commaPos = strchr(params,',');
        int     numPoints;
        
        if ( commaPos == NULL || sscanf(commaPos+1,"%d",&numPoints) != 1)
        {
            printf("Expected numpoints,<number>\r\n");
            return;
        }
        
        if ( numPoints == 0 || numPoints > Linearizer::MAX_POINTS )
        {
            printf("Number of points must be > 0 and < %d\r\n",Linearizer::MAX_POINTS);
            return;
        }
        
        getLinearizer(labPSU).setNumPoints(numPoints);
        printf("Number of points set to %d\r\n",numPoints);
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
			getLinearizer(labPSU).setPoint(index,point);
            printf("Point %d=0x%04X,%f\r\n",index,point.code,(double)point.value);
		}
		else
		{
			printf("Error in parameter value. Expected index,code,value\r\n");
		}
	}
}

void CalibrationCommand::handleGetCommand(LabPSU *labPSU) const
{
	Linearizer& linearizer = getLinearizer(labPSU);
	for(uint16_t i=0;i<linearizer.getNumPoints();i++)
	{
		const Linearizer::Point& nextPoint = linearizer.getPoint(i);
		printf("0x%04X = 0x%f\r\n",nextPoint.code,(double)nextPoint.value);
	}
}

VoltageADCCalibrationCommand::VoltageADCCalibrationCommand() 
	: CalibrationCommand(VOLTAGE_ADC_CAL_COMMAND)
{
	
}

Linearizer& VoltageADCCalibrationCommand::getLinearizer(LabPSU *labPSU) const
{
	return labPSU->getVoltageADCLinearizer();
}

void VoltageADCCalibrationCommand::saveLinearizer(LabPSU *labPSU) const
{
	LineariserStorage::saveLinearizer(
		LineariserStorage::VOLTAGE_ADC,
		labPSU->getVoltageADCLinearizer());
}

VoltageDACCalibrationCommand::VoltageDACCalibrationCommand()
	: CalibrationCommand(VOLTAGE_DAC_CAL_COMMAND)
{
		
}
	
Linearizer& VoltageDACCalibrationCommand::getLinearizer(LabPSU *labPSU) const
{
	return labPSU->getVoltageDACLinearizer();
}
	
void VoltageDACCalibrationCommand::saveLinearizer(LabPSU *labPSU) const
{
	LineariserStorage::saveLinearizer(
		LineariserStorage::VOLTAGE_DAC,
		labPSU->getVoltageDACLinearizer());
}

CurrentADCCalibrationCommand::CurrentADCCalibrationCommand()
	: CalibrationCommand(CURRENT_ADC_CAL_COMMAND)
{
		
}

Linearizer& CurrentADCCalibrationCommand::getLinearizer(LabPSU *labPSU) const
{
	return labPSU->getCurrentADCLinearizer();	
}
	
void CurrentADCCalibrationCommand::saveLinearizer(LabPSU *labPSU) const
{
	LineariserStorage::saveLinearizer(
		LineariserStorage::CURRENT_ADC,
		labPSU->getCurrentADCLinearizer());
}

CurrentDACCalibrationCommand::CurrentDACCalibrationCommand()
	: CalibrationCommand(CURRENT_DAC_CAL_COMMAND)
{
		
}
	
Linearizer& CurrentDACCalibrationCommand::getLinearizer(LabPSU *labPSU) const
{
	return labPSU->getCurrentDACLinearizer();
}
	
void CurrentDACCalibrationCommand::saveLinearizer(LabPSU *labPSU) const
{
	LineariserStorage::saveLinearizer(
		LineariserStorage::CURRENT_DAC,
		labPSU->getCurrentDACLinearizer());
}
