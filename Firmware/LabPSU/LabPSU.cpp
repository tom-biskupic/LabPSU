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
#include <math.h>

namespace
{
    const AD5689DAC::Channel VOLTAGE_DAC_CHANNEL = AD5689DAC::DAC_A;	
    const AD5689DAC::Channel CURRENT_DAC_CHANNEL = AD5689DAC::DAC_B;

    const AD7705ADC::Channel VOLTAGE_ADC_CHANNEL = AD7705ADC::CHANNEL_0;
    const AD7705ADC::Channel CURRENT_ADC_CHANNEL = AD7705ADC::CHANNEL_1;
    
    //
    //	These constants define the max current limit when we are in the
    //	series and parallel configuration for the transformer taps
    //
    const float SERIES_TAP_MAX_CURRENT_LIMIT=3.0;
    const float PARA_TAP_MAX_CURRENT_LIMIT=4.8;
    
    //
    //  These are both on the D port
    //
    const int BIAS_VOLTAGE_CTRL_PIN = 6;
    const int RELAY_CONTROL_PIN = 7;
    const int CONST_CURRENT_MODE_PIN=5;
    
    const int DAC_SS_PIN=1;
    const int ADC_SS_PIN=0;
    const int ADC_DATA_READY_PIN=4; // PORTD
    
    //
    //	This is the highest voltage we will go to before switching to the
    //	series configuration of the transformer taps. In series mode the current
    //	limit drops also.
    //
    const float THIRTY_VOLT_MODE_THRESHOLD=15.0;
    
    Linearizer::Point VOLTS_POINTS[] =
    {
       {0x0,       +9.95773822E-01},
       {0x7ff,     +9.38519687E-01},
       {0xffe,     +1.87576760E+00},
       {0x17fd,    +2.81310263E+00},
       {0x1ffc,    +3.75031374E+00},
       {0x27fb,    +4.68753207E+00},
       {0x2ffa,    +5.62476359E+00},
       {0x37f9,    +6.56201560E+00},
       {0x3ff8,    +7.49936507E+00},
       {0x47f7,    +8.43640837E+00},
       {0x4ff6,    +9.37364824E+00},
       {0x57f5,    +1.03107634E+01},
       {0x5ff4,    +1.12480540E+01},
       {0x67f3,    +1.21854413E+01},
       {0x6ff2,    +1.31225182E+01},
       {0x77f1,    +1.40596194E+01},
       {0x7ff0,    +1.49968045E+01},
       {0x87ef,    +1.59339592E+01},
       {0x8fee,    +1.68711708E+01},
       {0x97ed,    +1.78083346E+01},
       {0x9fec,    +1.87456026E+01},
       {0xa7eb,    +1.96827735E+01},
       {0xafea,    +2.06199274E+01},
       {0xb7e9,    +2.15570392E+01},
       {0xbfe8,    +2.24941692E+01},
       {0xc7e7,    +2.34314382E+01},
       {0xcfe6,    +2.43685758E+01},
       {0xd7e5,    +2.53056592E+01},
       {0xdfe4,    +2.62429038E+01},
       {0xe7e3,    +2.71801479E+01},
       {0xefe2,    +2.81172681E+01},
       {0xf7e1,    +2.90543960E+01},
       {0xffe0,    +2.99912247E+01}
    };
     
    int NUM_VOLTS_POINTS = sizeof(VOLTS_POINTS)/sizeof(VOLTS_POINTS[0]);
    
    Linearizer::Point VOLTS_ADC_POINTS[] =
    {
       {0x2e,    +3.22477947E-03},
       {0x7ff,    +9.38451916E-01},
       {0xfee,    +1.87573599E+00},
       {0x17dc,    +2.81304941E+00},
       {0x1fcb,    +3.75028297E+00},
       {0x27b9,    +4.68750535E+00},
       {0x2fa7,    +5.62477225E+00},
       {0x3796,    +6.56199362E+00},
       {0x3f84,    +7.49936197E+00},
       {0x4772,    +8.43641000E+00},
       {0x4f60,    +9.37365094E+00},
       {0x574e,    +1.03107889E+01},
       {0x5f3c,    +1.12480715E+01},
       {0x672a,    +1.21854936E+01},
       {0x6f18,    +1.31225339E+01},
       {0x7706,    +1.40595926E+01},
       {0x7ef4,    +1.49968153E+01},
       {0x86e2,    +1.59339337E+01},
       {0x8ed0,    +1.68712148E+01},
       {0x96bf,    +1.78083948E+01},
       {0x9ead,    +1.87456234E+01},
       {0xa69c,    +1.96827937E+01},
       {0xae8a,    +2.06198563E+01},
       {0xb679,    +2.15570343E+01},
       {0xbe67,    +2.24942104E+01},
       {0xc655,    +2.34314013E+01},
       {0xce43,    +2.43685778E+01},
       {0xd631,    +2.53057058E+01},
       {0xde1f,    +2.62429755E+01},
       {0xe60d,    +2.71802315E+01},
       {0xedfc,    +2.81173747E+01},
       {0xf5ea,    +2.90544988E+01},
       {0xfdd7,    +2.99913063E+01}
    };
    int NUM_VOLTS_ADC_POINTS = sizeof(VOLTS_ADC_POINTS)/sizeof(VOLTS_ADC_POINTS[0]);
    
    Linearizer::Point AMPS_DAC_POINTS[] =
    {
        {0x0,       -4.38587476E-06},
        {0x7ff,     +1.55715911E-01},
        {0xffe,     +3.11097678E-01},
        {0x17fd,    +4.66622468E-01},
        {0x1ffc,    +6.22086977E-01},
        {0x27fb,    +7.77574302E-01},
        {0x2ffa,    +9.33018828E-01},
        {0x37f9,    +1.08851207E+00},
        {0x3ff8,    +1.24406660E+00},
        {0x47f7,    +1.39932335E+00},
        {0x4ff6,    +1.55489044E+00},
        {0x57f5,    +1.71048744E+00},
        {0x5ff4,    +1.86584434E+00},
        {0x67f3,    +2.02130321E+00},
        {0x6ff2,    +2.17674566E+00},
        {0x77f1,    +2.33230084E+00},
        {0x7ff0,    +2.48762629E+00},
        {0x87ef,    +2.64301006E+00},
        {0x8fee,    +2.79849092E+00},
        {0x97ed,    +2.95404066E+00},
        {0x9fec,    +3.10930152E+00},
        {0xa7eb,    +3.26483874E+00},
        {0xafea,    +3.42021662E+00},
        {0xb7e9,    +3.57560562E+00},
        {0xbfe8,    +3.73111396E+00},
        {0xc7e7,    +3.88655942E+00},
        {0xcfe6,    +4.04214679E+00},
        {0xd7e5,    +4.19748544E+00},
        {0xdfe4,    +4.35293429E+00},
        {0xe7e3,    +4.50543425E+00},
        {0xefe2,    +4.65568860E+00}
    };    
    int NUM_AMPS_DAC_POINTS = sizeof(AMPS_DAC_POINTS)/sizeof(AMPS_DAC_POINTS[0]);
      
    Linearizer::Point AMPS_ADC_POINTS[] =
    {
        {0xf,       +2.30258425E-05},
        {0x80d,     +1.56049770E-01},
        {0x1017,    +3.12077284E-01},
        {0x181e,    +4.68133608E-01},
        {0x2026,    +6.24187785E-01},
        {0x282d,    +7.80224242E-01},
        {0x3038,    +9.36328250E-01},
        {0x3840,    +1.09238470E+00},
        {0x4047,    +1.24835330E+00},
        {0x4851,    +1.40443072E+00},
        {0x5058,    +1.56048843E+00},
        {0x585f,    +1.71647454E+00},
        {0x6068,    +1.87254738E+00},
        {0x6870,    +2.02862836E+00},
        {0x7078,    +2.18464638E+00},
        {0x7880,    +2.34069333E+00},
        {0x8089,    +2.49675959E+00},
        {0x8892,    +2.65280991E+00},
        {0x909a,    +2.80891241E+00},
        {0x98a2,    +2.96490286E+00},
        {0xa0ac,    +3.12100844E+00},
        {0xa8b4,    +3.27697014E+00},
        {0xb0bc,    +3.43313605E+00},
        {0xb8c4,    +3.58915892E+00},
        {0xc0ce,    +3.74519996E+00},
        {0xc8d5,    +3.90121124E+00},
        {0xd0dc,    +4.05721984E+00},
        {0xd8e6,    +4.21334172E+00},
        {0xe0ec,    +4.36928275E+00},
        {0xe91d,    +4.52846892E+00},
        {0xefaa,    +4.65613044E+00}
    };
    int NUM_AMPS_ADC_POINTS = sizeof(AMPS_ADC_POINTS)/sizeof(AMPS_ADC_POINTS[0]);
}

LabPSU::LabPSU() :	m_setVoltage(0.0f),
                    m_currentLimit(0.0f),
                    m_outputEnabled(false),
                    m_inThirtyVoltMode(true),
                    m_voltsLinearizer(VOLTS_POINTS,NUM_VOLTS_POINTS),
                    m_voltsADCLinearizer(VOLTS_ADC_POINTS,NUM_VOLTS_ADC_POINTS),
                    m_ampsDACLinearizer(AMPS_DAC_POINTS,NUM_AMPS_DAC_POINTS),
                    m_ampsADCLinearizer(AMPS_ADC_POINTS,NUM_AMPS_ADC_POINTS),
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
    //printf("Doing a DAC Reset\r\n");
    m_dac.reset();

    setOutputVoltageLimit(m_setVoltage);
    setCurrentLimit(m_currentLimit);
    
    //printf("Doing a ADC Init\r\n");   
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
        setThirtyVoltMode(m_setVoltage > THIRTY_VOLT_MODE_THRESHOLD);

        //
        //  Adjust the current limit in case we switched from parallel to series
        //  mode
        //
        float maxCurrentLimit = getMaxCurrentLimit();
        if ( m_currentLimit > maxCurrentLimit )
        {
            setCurrentLimit(maxCurrentLimit);
        }

        if ( m_outputEnabled )
        {
            setVoltageDAC(m_setVoltage);
        }
    }
}

void LabPSU::setVoltageDACCount(unsigned int count)
{
    m_outputEnabled=true;
    m_setVoltage = (30.0/(float)0xffff) * (float)count;
    setThirtyVoltMode(m_setVoltage > THIRTY_VOLT_MODE_THRESHOLD);
    
    //
    //  Adjust the current limit in case we switched from parallel to series
    //  mode
    //
    float maxCurrentLimit = getMaxCurrentLimit();
    if ( m_currentLimit > maxCurrentLimit )
    {
        setCurrentLimit(maxCurrentLimit);
    }
    m_dac.setOutput(count,VOLTAGE_DAC_CHANNEL);
}

void LabPSU::setVoltageDAC(const float voltage)
{
    uint16_t count = m_voltsLinearizer.valueToCode(voltage);

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
        float maxCurrentLimit = getMaxCurrentLimit();
        if ( value > maxCurrentLimit )
        {
            m_currentLimit = maxCurrentLimit;    
        }
        else
        {
             m_currentLimit = value;
        }
       
        if ( m_outputEnabled )
        {
            setCurrentDAC(m_currentLimit);
        }
    }
}

void LabPSU::setCurrentDACCount(unsigned int count)
{
    m_currentLimit = (5.0/(float)0xffff)*(float)count;
    m_dac.setOutput(count,CURRENT_DAC_CHANNEL);  
}

void LabPSU::setCurrentDAC(const float value)
{
    uint16_t count = m_ampsDACLinearizer.valueToCode(value);
    m_dac.setOutput(count,CURRENT_DAC_CHANNEL);
}

const float LabPSU::getCurrentLimit() const
{
    return m_currentLimit;
}

const float LabPSU::getMaxCurrentLimit() const
{
    if ( m_inThirtyVoltMode )
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
    return m_ampsADCLinearizer.codeToValue(code);
}

unsigned int LabPSU::getOutputCurrentCount() const
{
    return readADC(CURRENT_ADC_CHANNEL);    
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
