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
        {0x0,		    +1.17590640E-02},
        {0x7ff,		+9.40177747E-01},
        {0xffe,		+1.87743827E+00},
        {0x17fd,		+2.81477950E+00},
        {0x1ffc,		+3.75216770E+00},
        {0x27fb,		+4.68946553E+00},
        {0x2ffa,		+5.62678085E+00},
        {0x37f9,		+6.56406812E+00},
        {0x3ff8,		+7.50145700E+00},
        {0x47f7,		+8.43868950E+00},
        {0x4ff6,		+9.37601271E+00},
        {0x57f5,		+1.03132273E+01},
        {0x5ff4,		+1.12505271E+01},
        {0x67f3,		+1.21882230E+01},
        {0x6ff2,		+1.31252916E+01},
        {0x77f1,		+1.40623678E+01},
        {0x7ff0,		+1.49995497E+01},
        {0x87ef,		+1.59377774E+01},
        {0x8fee,		+1.68749290E+01},
        {0x97ed,		+1.78119898E+01},
        {0x9fec,		+1.87490699E+01},
        {0xa7eb,		+1.96862159E+01},
        {0xafea,		+2.06232978E+01},
        {0xb7e9,		+2.15603995E+01},
        {0xbfe8,		+2.24974797E+01},
        {0xc7e7,		+2.34345509E+01},
        {0xcfe6,		+2.43716721E+01},
        {0xd7e5,		+2.53088281E+01},
        {0xdfe4,		+2.62458910E+01},
        {0xe7e3,		+2.71830544E+01},
        {0xefe2,		+2.81200973E+01},
        {0xf7e1,		+2.90570186E+01},
        {0xffe0,		+2.99936415E+01}
    };
     
    int NUM_VOLTS_POINTS = sizeof(VOLTS_POINTS)/sizeof(VOLTS_POINTS[0]);
    
    Linearizer::Point VOLTS_ADC_POINTS[] =
    {
       {0xf,    +6.48396023E-03},
       {0x7f8,    +9.40396497E-01},
       {0xfe8,    +1.87773986E+00},
       {0x17d9,    +2.81513633E+00},
       {0x1fc9,    +3.75261122E+00},
       {0x27b7,    +4.68994442E+00},
       {0x2fa9,    +5.62733188E+00},
       {0x3797,    +6.56465975E+00},
       {0x3f85,    +7.50215128E+00},
       {0x4776,    +8.43942074E+00},
       {0x4f67,    +9.37683885E+00},
       {0x5756,    +1.03140562E+01},
       {0x5f46,    +1.12514758E+01},
       {0x6734,    +1.21891751E+01},
       {0x6f22,    +1.31263503E+01},
       {0x7713,    +1.40634575E+01},
       {0x7eff,    +1.50007643E+01},
       {0x86eb,    +1.59390294E+01},
       {0x8ed8,    +1.68761927E+01},
       {0x96c8,    +1.78133443E+01},
       {0x9eb6,    +1.87505267E+01},
       {0xa6a5,    +1.96876695E+01},
       {0xae93,    +2.06249099E+01},
       {0xb683,    +2.15621128E+01},
       {0xbe71,    +2.24992732E+01},
       {0xc661,    +2.34365085E+01},
       {0xce4f,    +2.43736895E+01},
       {0xd63e,    +2.53108240E+01},
       {0xde2c,    +2.62481103E+01},
       {0xe61c,    +2.71853444E+01},
       {0xee09,    +2.81224708E+01},
       {0xf5f8,    +2.90594625E+01},
       {0xfde4,    +2.99961230E+01}
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
    
    initADC();    
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
            setVoltageDAC(value);
        }
    }
}

void LabPSU::setVoltageDACCount(unsigned int count)
{
    m_outputEnabled=true;
    m_setVoltage = (30.0/(float)0xffff) * (float)count;
    setThirtyVoltMode(m_setVoltage > THIRTY_VOLT_MODE_THRESHOLD);
    m_dac.setOutput(count,VOLTAGE_DAC_CHANNEL);
}

void LabPSU::setVoltageDAC(const float voltage)
{
    setThirtyVoltMode(voltage > THIRTY_VOLT_MODE_THRESHOLD);
    uint16_t count = m_voltsLinearizer.valueToCode(voltage);
    //printf("Setting DAC A to 0x%x\r\n",count);

    m_dac.setOutput(count,VOLTAGE_DAC_CHANNEL);
}

const float LabPSU::getOutputVoltageLimit() const
{
    return m_setVoltage;
}
    
float LabPSU::getOutputVoltage() const
{
    uint16_t code = getOutputVoltageCount();
    return m_voltsADCLinearizer.codeToValue(code);
}

uint16_t LabPSU::getOutputVoltageCount() const
{
    return readADC(VOLTAGE_ADC_CHANNEL);
}

void LabPSU::setCurrentLimit(const float value)
{
    if ( m_currentLimit != value )
    {
        m_currentLimit = value;
        
        if ( m_outputEnabled )
        {
            setCurrentDAC(value);
        }
    }
}

void LabPSU::setCurrentDAC(const float value)
{
    //
    //	TODO: Use a linearizer
    //
    uint16_t count = round(value/m_ampsPerStep);
    //printf("Setting DAC B to 0x%x\r\n",count);
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
            setVoltageDAC(m_setVoltage);
            setCurrentDAC(m_currentLimit);
        }
        else
        {
            setVoltageDAC(0.0f);
            setCurrentDAC(0.0f);
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
    
    //printf("Raw ADC Value Read %u from ADC channel %d\r\n",value,channel);
    
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
