/* 
* SPIDevice
* 
* Copyright Runcible Software Pty Ltd 2015

* Created: 7/3/2015 7:51:53 PM
* Author: tom.biskupic@gmail.com
*/

#include "SPIDevice.h"
#include <avr/io.h>
#include "stdio.h"

namespace
{
    const int SCK_PIN = DDB5;    
    const int MISO_PIN = DDB4;
    const int MOSI_PIN = DDB3;
    const int SS_PIN = DDB2;
}

SPIDevice::SPIDevice(bool master, int selectPin)
    :   m_selectPin(selectPin),
        m_bitOrder(MSB_FIRST),
        m_clockPolarity(CLOCK_POLARITY_RISING_LEADS),
        m_clockPhase(CLOCK_PHASE_SAMPLE_ON_LEADING),
        m_clockRate(RATE_DIV_16),
        m_interruptEnabled(false),
        m_master(master),
        m_doubleClockSpeed(false)
{
    //
    //  If we are the master then we want to immediately set the select pin as an
    //  output and set it high. We also set the SS Pin as an output to stop spurious 
    //  signals taking us out of master mode.
    //
    if ( m_master )
    {
        DDRB |= (1<<m_selectPin) | (1<<SS_PIN);
        
        //
        //  Set MISO as an input
        //
        DDRB &= ~(1<<MISO_PIN);
        
        //
        //  Set the select pin high
        //
        PORTB |= (1<<m_selectPin);
    }
}

void SPIDevice::enableInterrup(bool enable)
{
    m_interruptEnabled = enable;
}    
    
bool SPIDevice::isInterruptsEnabled() const
{
    return m_interruptEnabled;
}
    
void SPIDevice::setBitOrdering( const BitOrder order )
{
    m_bitOrder = order;
}
    
const SPIDevice::BitOrder SPIDevice::getBitOrdering() const
{
    return m_bitOrder;
}
    
void SPIDevice::setMaster( bool master )
{
    m_master = master;
}
    
bool SPIDevice::isMaster() const
{
    return m_master;
}
    
void SPIDevice::setClockRate( const ClockRate rate )
{
    m_clockRate = rate;
}
    
const SPIDevice::ClockRate SPIDevice::getClockRate() const
{
    return m_clockRate;
}

const SPIDevice::ClockPolarity SPIDevice::getClockPolarity() const
{
    return m_clockPolarity;
}    
    
void SPIDevice::setClockPolarity(const ClockPolarity polarity)
{
    m_clockPolarity = polarity;
}

const SPIDevice::ClockPhase SPIDevice::getClockPhase() const
{
    return m_clockPhase;
}    
    
void SPIDevice::setClockPhase(const ClockPhase phase)
{
    m_clockPhase = phase;
}

void SPIDevice::setDoubleSpeedModeEnabled( bool enable )
{
    m_doubleClockSpeed = enable;
}

bool SPIDevice::isDoubleSpeedModeEnabled() const
{
    return m_doubleClockSpeed;
}
    
void SPIDevice::setup() const
{
    if ( m_master )
    {
        //
        //  Set MOSI, SCK and the select pin as outputs
        //
        DDRB |= (1<<MOSI_PIN) | (1<<SCK_PIN) | (1<<SS_PIN) | (1<<m_selectPin);
        
        //
        //  Set MISO as an input
        //
        DDRB &= ~(1<<MISO_PIN);
        PORTB &= ~(1<<MISO_PIN);
    }
    else
    {
        DDRB |= (1<<MISO_PIN);
    }
    
    int newSPCR = (1 << SPE ) |
            ((m_bitOrder)<<DORD) |
            ((m_master?1:0)<< MSTR) | 
            ((m_interruptEnabled?1:0) << SPIE) | 
            ((m_clockPolarity?1:0) << CPOL) | 
            ((m_clockPhase?1:0) << CPHA) |
            (m_clockRate & 0x3);
    
    SPCR = newSPCR;
    
    if ( m_doubleClockSpeed )
    {
        SPSR |= ((m_doubleClockSpeed?1:0) << SPI2X);
    }
    else
    {
        SPSR &= ~((m_doubleClockSpeed?1:0) << SPI2X);
    }           
}

bool SPIDevice::getWriteCollisionFlag() const
{
    return (SPSR & (1<<WCOL)) != 0;
}

bool SPIDevice::getInterruptStatus() const
{
    return (SPSR & (1<<SPIF)) != 0;
}

void SPIDevice::setupSelectLine() const
{
    //
    //  Pull select low (as it is active low)
    //
    PORTB &= ~(1 << m_selectPin);    
}    
    
void SPIDevice::clearSelectLine() const
{
    PORTB |= (1 << m_selectPin);
}

uint8_t SPIDevice::writeByte( uint8_t byte )
{
    SPDR = byte;
    while( !getInterruptStatus() );    
    return SPDR;
}
    
uint8_t SPIDevice::readByte() const
{
    SPDR = 0;
    while( !getInterruptStatus() );
    return SPDR;
}
