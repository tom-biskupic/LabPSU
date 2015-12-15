/* 
* LabPSU.h
*
* Created: 19/07/2015 2:15:23 AM
* Author: tom
*/

#ifndef __LABPSU_H__
#define __LABPSU_H__

#include "AD5689DAC.h"
#include "AD7705ADC.h"
#include "Linearizer.h"

/*
The LabPSU class models the actual power supply hardware. This class provides
functions to set/read the output voltage and current as well as checking various
flags like the CC mode.
*/
class LabPSU
{
public:
	LabPSU();
	
    /*
    Allows us to do all the initialization well after the CPU
    has started and the hardware has settled
    */
    void init();
    
	/*
	Sets the output voltage.
	*/
	void setOutputVoltageLimit(const float value);
	
	/*
	Returns the set voltage (not the current output voltage)
	*/
	const float getOutputVoltageLimit() const;
	
	/*
	Returns the current output voltage read from the DAC
	*/
	float getOutputVoltage() const;
	
	/*
	Set the current limit
	*/
	void setCurrentLimit(const float value);
	
	/*
	Returns the current limit.
	*/
	const float getCurrentLimit() const;
	
	/*
	Returns the maximum output current based on the transformer
	winding configuration. If the output voltage is set above the single
	winding threshold then the current limit drops.
	*/
	const float getMaxCurrentLimit() const;
	
	/*
	Returns the current measurement from the ADC.
	*/
	const float getOutputCurrent() const;
	
	/*
	Enables or disables the PSU output
	*/
	void enableOutput(const bool enable=true);
	
	/*
	Returns true if the output is enabled
	*/
	const bool isOutputEnabled() const;
	
	/*
	Returns true if the supply is in current limit mode
	*/
	bool isInCurrentLimit();
	
    /*
    Set the power supply output voltage to zero and initiates
    a calibration of the ADC for zero offset.
    */
    void calibrateVoltageADCZeroOffset();
    
    /*
    Set the power supply output voltage to zero and initiates
    a calibration of the ADC for zero offset.
    */
    void calibrateVoltageADCFullScale();
    
    /*
    Reads the zero calibration coefficient from the ADC
    */
    uint32_t getVoltageZeroCalibrationCoefficient();

    /*
    Reads the full scale calibration coefficient from the ADC
    */
    uint32_t getVoltageFullScaleCalibrationCoefficient();
    
private:
	
    void setThirtyVoltMode(const bool enable);
    void initADC() const;
	
	uint16_t readADC( const AD7705ADC::Channel channel ) const;
	
	float	m_setVoltage;
	float	m_currentLimit;
	bool	m_outputEnabled;
	bool    m_inThirtyVoltMode;

	Linearizer    m_voltsLinearizer;
	Linearizer    m_voltsADCLinearizer;
	
	float	m_ampsPerStep;
	
	AD5689DAC m_dac;
	mutable AD7705ADC m_adc;
};

#endif //__LABPSU_H__
