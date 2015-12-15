/* 
* CalibrateADCCommand.cpp
*
* Created: 9/22/2015 11:52:01 PM
* Author: tom
*/


#include "CalibrateVoltageADCZeroCommand.h"
#include "stdio.h"

namespace
{
    const char *CALIBRATE_ADC_VOLTAGE_ZERO_COMMAND_NAME = "CalVoltageADCZero";
}

CalibrateVoltageADCZeroCommand::CalibrateVoltageADCZeroCommand(LabPSU *psu) 
    : Command(CALIBRATE_ADC_VOLTAGE_ZERO_COMMAND_NAME,psu)
{
    
}

void CalibrateVoltageADCZeroCommand::handleSetCommand( const char *params )
{
    m_psu->calibrateVoltageADCZeroOffset();
    printf("Voltage Zero offset calibration complete\r\n");
}

void CalibrateVoltageADCZeroCommand::handleGetCommand() const
{
    printf(
        "Voltage Zero Calibration Value = 0x%lx\r\n",
        m_psu->getVoltageZeroCalibrationCoefficient());
}

