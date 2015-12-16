/* 
* LabPSU.cpp
*
* Created: 19/07/2015 2:15:23 AM
* Author: tom
*/

#define F_CPU 16000000

#include "LabPSU.h"
#include "math.h"
#include "stdio.h"
#include <avr/io.h>
#include "util/delay.h"


namespace
{
	const AD5689DAC::Channel VOLTAGE_DAC_CHANNEL = AD5689DAC::DAC_A;	
	const AD5689DAC::Channel CURRENT_DAC_CHANNEL = AD5689DAC::DAC_B;

	const AD7705ADC::Channel VOLTAGE_ADC_CHANNEL = AD7705ADC::CHANNEL_0;
	const AD7705ADC::Channel CURRENT_ADC_CHANNEL = AD7705ADC::CHANNEL_1;
	//const AD7705ADC::Channel CURRENT_ADC_CHANNEL = AD7705ADC::CHANNEL_0_1;
    
	//
	//	This is the highest voltage we will go to before switching to the 
	//	series configuration of the transformer taps. In series mode the current
	//	limit drops also.
	//
	const float SERIES_TAP_LOW_VOLTAGE_THRESHOLD=14.0;
	
	//
	//	These constants define the max current limit when we are in the
	//	series and parallel configuration for the transformer taps
	//
	const float SERIES_TAP_MAX_CURRENT_LIMIT=3.0;
	const float PARA_TAP_MAX_CURRENT_LIMIT=5.0;
    
    //
    //  These are both on the D port
    //
    const int BIAS_VOLTAGE_CTRL_PIN = 6;
    const int RELAY_CONTROL_PIN = 7;
	const int CONST_CURRENT_MODE_PIN=5;
    
	const int DAC_SS_PIN=1;
	const int ADC_SS_PIN=0;
	const int ADC_DATA_READY_PIN=4; // PORTD
    
    const float THIRTY_VOLT_MODE_THRESHOLD=15.0;
	
    Linearizer::Point VOLTS_POINTS[] =
	{
 		{0x0,		+6.67462243E-03},
 		{0x878,		+9.97950583E-01},
 		{0x10fb,		+1.99597810E+00},
 		{0x197d,		+2.99327013E+00},
 		{0x2200,		+3.99115619E+00},
 		{0x2a83,		+4.98904250E+00},
 		{0x2a83,		+4.98904132E+00},
 		{0x3b89,		+6.98480286E+00},
 		{0x4c8e,		+8.97990683E+00},
 		{0x5d93,		+1.09748723E+01},
 		{0x6e99,		+1.29705096E+01},
 		{0x7f9e,		+1.49652618E+01},
 		{0x907d,		+1.69439974E+01},
 		{0xa184,		+1.89398320E+01},
 		{0xb28c,		+2.09362116E+01},
 		{0xc392,		+2.29314486E+01},
 		{0xd499,		+2.49271145E+01},
 		{0xe59f,		+2.69223353E+01},
 		{0xf6a4,		+2.89169182E+01}
 	};
     
	int NUM_VOLTS_POINTS = sizeof(VOLTS_POINTS)/sizeof(VOLTS_POINTS[0]);
	
    Linearizer::Point VOLTS_ADC_POINTS[] =
    {
		{222,           +4.02142359E-03},
		{255,           +9.90742509E-02},
		{339,           +1.99195608E-01},
		{438,           +2.99264166E-01},
		{541,           +3.99331883E-01},
		{658,           +4.99378535E-01},
		{796,           +5.99410862E-01},
		{954,           +6.99542653E-01},
		{1120,          +7.99148798E-01},
		{1309,          +8.99242274E-01},
		{1512,          +9.99286945E-01},
		{1720,          +1.09934337E+00},
		{1940,          +1.19937705E+00},
		{2159,          +1.29937958E+00},
		{2371,          +1.39910114E+00},
		{2594,          +1.49913824E+00},
		{2813,          +1.59919187E+00},
		{3031,          +1.69923720E+00},
		{3258,          +1.79932291E+00},
		{3478,          +1.89944678E+00},
		{3704,          +1.99951597E+00},
		{3924,          +2.09960247E+00},
		{4155,          +2.19919025E+00},
		{4374,          +2.29925266E+00},
		{4596,          +2.39928900E+00},
		{4821,          +2.49932493E+00},
		{5041,          +2.59935546E+00},
		{5265,          +2.69947534E+00},
		{5481,          +2.79958683E+00},
		{5701,          +2.89962892E+00},
		{5922,          +2.99921383E+00},
		{6142,          +3.09924987E+00},
		{6365,          +3.19932360E+00},
		{6590,          +3.29933818E+00},
		{6813,          +3.39946862E+00},
		{7035,          +3.49952375E+00},
		{7259,          +3.59953050E+00},
		{7478,          +3.69918256E+00},
		{7707,          +3.79929556E+00},
		{7928,          +3.89933731E+00},
		{8149,          +3.99926416E+00},
		{8372,          +4.09949857E+00},
		{8598,          +4.19959328E+00},
		{8814,          +4.29962172E+00},
		{9037,          +4.39967912E+00},
		{9266,          +4.49968540E+00},
		{9478,          +4.59926259E+00},
		{9703,          +4.69945143E+00},
		{9928,          +4.79949218E+00},
		{10143,         +4.89953365E+00},
		{10367,         +4.99956600E+00},
		{10366,         +4.99957197E+00},
		{12597,         +5.99968100E+00},
		{14814,         +6.99984891E+00},
		{17039,         +7.99942111E+00},
		{19264,         +8.99948483E+00},
		{21487,         +9.99960875E+00},
		{23711,         +1.09993744E+01},
		{25922,         +1.19994561E+01},
		{28135,         +1.30006874E+01},
		{30348,         +1.39997333E+01},
		{32563,         +1.50004635E+01},
		{34752,         +1.59987167E+01},
		{36969,         +1.69981331E+01},
		{39186,         +1.79987058E+01},
		{41406,         +1.89984197E+01},
		{43633,         +1.99989470E+01},
		{45855,         +2.09992208E+01},
		{48080,         +2.19991827E+01},
		{50303,         +2.29993793E+01},
		{52520,         +2.39996451E+01},
		{54738,         +2.49998110E+01},
		{56954,         +2.60005235E+01},
		{59171,         +2.69998550E+01},
		{61395,         +2.80002515E+01},
		{63584,         +2.89994907E+01},
		{65535,         +3.00009794E+01}
	};
	
	int NUM_VOLTS_ADC_POINTS = sizeof(VOLTS_ADC_POINTS)/sizeof(VOLTS_ADC_POINTS[0]);
}

LabPSU::LabPSU() :	m_setVoltage(0.0f),
					m_currentLimit(0.0f),
					m_outputEnabled(false),
                    m_inThirtyVoltMode(true),
					m_voltsLinearizer(VOLTS_POINTS,NUM_VOLTS_POINTS),
					m_voltsADCLinearizer(VOLTS_ADC_POINTS,NUM_VOLTS_ADC_POINTS),
					m_ampsPerStep(4.9958/65535.0),
                    m_dac(DAC_SS_PIN),
					m_adc(ADC_SS_PIN,ADC_DATA_READY_PIN)
{
    //
    //  Set the relay and bias control as outputs
    //
    DDRD |= (1 << BIAS_VOLTAGE_CTRL_PIN) | ( 1<< RELAY_CONTROL_PIN );
	
	//
	//	Set the constant current mode pin as an input
	//
	DDRD &= ~(1 << CONST_CURRENT_MODE_PIN);
}

void LabPSU::init()
{
    setThirtyVoltMode(false);
    m_dac.reset();
    setOutputVoltageLimit(m_setVoltage);
	setCurrentLimit(m_currentLimit);
	
    //initADC();    
}

void LabPSU::setThirtyVoltMode(const bool enable)
{
    if ( m_inThirtyVoltMode != enable )
    {
        if ( enable )
        {
            PORTD |= ( 1 << RELAY_CONTROL_PIN);
            PORTD &= ~( 1 << BIAS_VOLTAGE_CTRL_PIN );
        }
        else
        {
            PORTD |= ( 1 << BIAS_VOLTAGE_CTRL_PIN );
            PORTD &= ~( 1 << RELAY_CONTROL_PIN);           
        }
        
        m_inThirtyVoltMode = enable;
    }
}

void LabPSU::setOutputVoltageLimit(const float value)
{
	if ( m_setVoltage != value )
	{
		m_setVoltage = value;

		if ( m_outputEnabled )
		{
			setVoltageADC(value);
		}
	}
}


void LabPSU::setVoltageADC(const float voltage)
{
	setThirtyVoltMode(voltage > THIRTY_VOLT_MODE_THRESHOLD);
	uint16_t count = m_voltsLinearizer.valueToCode(voltage);
	printf("Setting DAC A to 0x%x\r\n",count);

	m_dac.setOutput(count,VOLTAGE_DAC_CHANNEL);
}

const float LabPSU::getOutputVoltageLimit() const
{
	return m_setVoltage;
}
	
float LabPSU::getOutputVoltage() const
{
	uint16_t code = readADC(VOLTAGE_ADC_CHANNEL);
	return m_voltsADCLinearizer.codeToValue(code);
}

void LabPSU::setCurrentLimit(const float value)
{
	if ( m_currentLimit != value )
	{
		m_currentLimit = value;
		
		if ( m_outputEnabled )
		{
			setCurrentADC(value);
		}
	}
}

void LabPSU::setCurrentADC(const float value)
{
	//
	//	TODO: Use a linearizer
	//
	uint16_t count = round(value/m_ampsPerStep);
	printf("Setting DAC B to 0x%x\r\n",count);
	m_dac.setOutput(count,CURRENT_DAC_CHANNEL);\
}

const float LabPSU::getCurrentLimit() const
{
	return m_currentLimit;
}

const float LabPSU::getMaxCurrentLimit() const
{
	if ( m_setVoltage > SERIES_TAP_LOW_VOLTAGE_THRESHOLD )
	{
		return SERIES_TAP_MAX_CURRENT_LIMIT;
	}	
	else
	{
		return PARA_TAP_MAX_CURRENT_LIMIT;
	}
}

const float LabPSU::getOutputCurrent() const
{
	uint16_t code = readADC(CURRENT_ADC_CHANNEL);
	
	//
	//	Need to use a linearizer here probably. Just do this for now.
	//
	return 5.0 * (float)code / (float)0xffff;
}
	
void LabPSU::enableOutput(const bool enable )
{
	if ( enable != m_outputEnabled )
	{
		m_outputEnabled = enable;
	
		if ( m_outputEnabled )
		{
			setVoltageADC(m_setVoltage);
			setCurrentADC(m_currentLimit);
		}
		else
		{
			setVoltageADC(0.0f);
			setCurrentADC(0.0f);
		}
	}
}
	
const bool LabPSU::isOutputEnabled() const
{
	return m_outputEnabled;
}

bool LabPSU::isInCurrentLimit()
{
    if ( m_outputEnabled )
    {
	    return ((PIND & (1<<CONST_CURRENT_MODE_PIN)) != 0);
    }
    else
    {
        return false;
    }        
}

void LabPSU::initADC() const
{
	//
	//	We run the ADC in bipolar mode so we can get the full 0..5V range.
	//	The AIN- is tied to the reference so the input is a bipolar number
	//	relative to that.
	//
	//	Because of the bipolar requirement we can't use gain.
	//	I am using a 2MHz crystal which means clockdiv is 1. 
	//	An update rate of 50Hz or less is fine so filter select is 0
	//
    m_adc.reset();
	m_adc.setClockRegister(AD7705ADC::CLOCK_2,0);

	m_adc.setSetupRegister(
	    VOLTAGE_ADC_CHANNEL,                    //  Channel
	    AD7705ADC::AD7705ADC::MODE_SELF_CAL,    //	CAL mode
	    AD7705ADC::GAIN_1,					    //	Gain = 1
	    AD7705ADC::BIPOLAR,				        //	Polarity
	    false,								    //	Not buffered
	    false);								    //	F Sync off
	
    m_adc.reset();
    	
	m_adc.setSetupRegister(
	    CURRENT_ADC_CHANNEL,                    //  Channel
	    AD7705ADC::AD7705ADC::MODE_SELF_CAL,    //	CAL mode
	    AD7705ADC::GAIN_1,					    //	Gain = 1
	    AD7705ADC::BIPOLAR,                     //	Polarity
	    false,								    //	Not buffered
	    false);								    //	F Sync off
}

uint16_t LabPSU::readADC( const AD7705ADC::Channel channel ) const
{
    m_adc.reset();
    
	uint16_t value = m_adc.getValue(channel);
    
    printf("Raw ADC Value Read %u from ADC channel %d\r\n",value,channel);
	
    return value;
}

void LabPSU::calibrateVoltageADCZeroOffset()
{
    float oldSetVoltage = m_setVoltage;
    
    setOutputVoltageLimit(0.0f);
    _delay_ms(5000.0);
    
    m_adc.setSetupRegister(
    	VOLTAGE_ADC_CHANNEL,                         //  Channel
    	AD7705ADC::AD7705ADC::MODE_ZERO_SCALE_CAL,  //	CAL mode
    	AD7705ADC::GAIN_1,					        //	Gain = 1
    	AD7705ADC::BIPOLAR,						    //	Polarity
    	false,								        //	Not buffered
    	false);								        //	F Sync off
        
    setOutputVoltageLimit(oldSetVoltage);
}    
    
void LabPSU::calibrateVoltageADCFullScale()
{
    float oldSetVoltage = m_setVoltage;
    
    setOutputVoltageLimit(30.0f);
    _delay_ms(5000.0);
    
    m_adc.setSetupRegister(
        VOLTAGE_ADC_CHANNEL,                        //  Channel
        AD7705ADC::AD7705ADC::MODE_FULL_SCALE_CAL,  //	CAL mode
        AD7705ADC::GAIN_1,					        //	Gain = 1
        AD7705ADC::BIPOLAR,						    //	Polarity
        false,								        //	Not buffered
        false);								        //	F Sync off
    
    setOutputVoltageLimit(oldSetVoltage);    
}

uint32_t LabPSU::getVoltageZeroCalibrationCoefficient()
{
    return m_adc.readOffsetRegister(VOLTAGE_ADC_CHANNEL);
}

uint32_t LabPSU::getVoltageFullScaleCalibrationCoefficient()
{
    return m_adc.readFullScaleRegister(VOLTAGE_ADC_CHANNEL);
}
