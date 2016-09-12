/* 
 * SPIDevice.h
 *
 * Copyright Runcible Software Pty Ltd.
 *
 * Created: 7/3/2015 7:51:53 PM
 * Author: tom.biskupic@gmail.com
 */

#include <stdint.h>

#ifndef __SPIDEVICE_H__
#define __SPIDEVICE_H__

/***********************************************************************
* Defines a base class for implementing abstractions of SPI Devices.
* Provide functions for configuring the SPI support and for sending
* and recieving data from those devices                                                            
***********************************************************************/

class SPIDevice
{
public:

    enum BitOrder
    {
        MSB_FIRST,
        LSB_FIRST
    };
    
    enum ClockPolarity
    {    
        CLOCK_POLARITY_RISING_LEADS,
        CLOCK_POLARITY_FALLING_LEADS 
    };
          
    enum ClockPhase
    {
        CLOCK_PHASE_SAMPLE_ON_LEADING,
        CLOCK_PHASE_SAMPLE_ON_TRAILING   
    };
    
    enum ClockRate
    {
        RATE_DIV_4=0,
        RATE_DIV_16=1,
        RATE_DIV_64=2,
        RATE_DIV_128=3,
        RATE_DIV_DBL_2=4,
        RATE_DIV_DBL_8=5,
        RATE_DIV_DBL_32=6,
        RATE_DIV_DBL_64=7
    };
    
    /*
    Constructs the SPI Device with default setup.
    By default we are MSB first, SPI Master and interrupts
    are disabled.
    */
    SPIDevice(bool master, int selectPin);

    /*
    Sets the SPI Mode (0-2) which sets the clock phase and polarity
    Mode 0 = CLOCK_POLARITY_RISING_LEADS | CLOCK_PHASE_SAMPLE_ON_LEADING
    Mode 1 = CLOCK_POLARITY_RISING_LEADS | CLOCK_PHASE_SAMPLE_ON_TRAILING
    Mode 2 = CLOCK_POLARITY_FALLING_LEADS | CLOCK_PHASE_SAMPLE_ON_LEADING
    Mode 3 = CLOCK_POLARITY_FALLING_LEADS | CLOCK_PHASE_SAMPLE_ON_TRAILING
    */
    void setSPIMode(const int mode);
    
    /*
    Returns true if the next operation will be done using interrupts.
    */
    void enableInterrup(bool enable=true);
    
    /*
    Returns true if interrupts are enabled for the next operation
    */
    bool isInterruptsEnabled() const;
    
    /*
    Sets the bit order that will be used for the next operation.
    */
    void setBitOrdering( const BitOrder order );
    
    /*
    Returns the bit order that will be used for the next operation.
    */
    const BitOrder getBitOrdering() const;
    
    /*
    Sets the mode of the AVR. Set to true if the AVR is the master and 
    false otherwise
    */
    void setMaster( bool master = true);
    
    /*
    Returns the mode of the AVR for the next communication. 
    Set to true if the AVR is the master
    */
    bool isMaster() const;
    
    /*
    Sets the clock rate divider that will set the rate used to
    communicate on the SPI bus for the next communication.
    */
    void setClockRate( const ClockRate rate );
    
    /*
    Returns the clock rate divider that will set the rate used
    to communicate on the SPI bus for the next communication.
    */
    const ClockRate getClockRate() const;
    
    /*
    Returns the clock polarity that will be used for the next
    transaction.
    */
    const ClockPolarity getClockPolarity() const;
    
    /*
    Sets the clock polarity that will be used for the next transaction
    */
    void setClockPolarity(const ClockPolarity polarity);

    /*
    Returns the clock phas used for the next transaction
    */
    const ClockPhase getClockPhase() const;
    
    /*
    Sets the clock phase that will be used for the next
    transaction
    */
    void setClockPhase(const ClockPhase phase);
    
    /*
    Enablse double speed mode on the SPI bus for the next
    operation.
    */
    void setDoubleSpeedModeEnabled( bool enable=true);

    /*
    Returns true if double speed is enabled for the next
    operation.
    */
    bool isDoubleSpeedModeEnabled() const;
    
protected:

    /*
    Returns the status of the interrupt flag from the SPI Status
    register. This indicates if the operation has completed.
    */
    bool getInterruptStatus() const;
    
    /*
    Returns true if a collision occurred on the SPI bus during
    the last operation. This is derived from the collision flag
    in the SPI status register
    */
    bool getWriteCollisionFlag() const;
    
    /*
    Sets up the SPI hardware ready for an operation.
    */
    void setup() const;
    
    /*
    Asserts the select line so the device knows we are talking to it
    */
    void setupSelectLine() const;
    
    /*
    Clears the select line because the transaction is finished
    */
    void clearSelectLine() const;
    
    /*
    Initiates a write operation with the byte specified
    Returns the value returned by the device
    */
    uint8_t writeByte( uint8_t byte );
    
    /*
    Reads a byte of data from the device
    */
    uint8_t readByte() const;
    
private:

    int             m_selectPin;
    BitOrder        m_bitOrder;
    ClockPolarity   m_clockPolarity;    
    ClockPhase      m_clockPhase;
    ClockRate       m_clockRate;
    bool            m_interruptEnabled;
    bool            m_master;
    bool            m_doubleClockSpeed;
};

#endif //__SPIDEVICE_H__
