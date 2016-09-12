/* 
* LineariserStorage.h
*
* Created: 8/09/2016 1:00:56 AM
* Author: tom
*/

#ifndef __LINEARISERSTORAGE_H__
#define __LINEARISERSTORAGE_H__

#include "Linearizer.h"

class LineariserStorage
{
public:

	enum LineariserType
	{
		VOLTAGE_DAC,
		VOLTAGE_ADC,
		CURRENT_DAC,
		CURRENT_ADC
    };
	
	/*
	Loads the linearizer from EEPROM
	*/
	static void loadLineariser(
		const LineariserType	type,
		Linearizer&				linearizer );
	
	/*
	Saves the linarizer to EEPROM. There can only be one linearizer
	of each type stored.
	*/
	static void saveLinearizer(const LineariserType type,const Linearizer& linearizer);
	
private:
	//
	//	Static only class
	//
	LineariserStorage();
	~LineariserStorage();
	
	//
	//	Calculate the address of a linearizer in EEPROM
	//
	static uint16_t calculateAddress(LineariserType type);
	
	//
	//	Loads the default linearizer of the type specified
	//
	static void loadDefault(LineariserType type,Linearizer& linearizer);
	
	//
	//	Reads a uint from EEPROM at the address given and moves
	//	the address pointer along by the size of the uint
	//	Returns the value read
	//
	static uint16_t readWord(uint16_t& address);

	//
	//	Writes a uint to EEPROM at the address given and moves
	//	the address pointer along by the size of the uint
	//
	static void writeWord(uint16_t value, uint16_t& address);
	
	//
	//	As above but reads a float
	//
	static float readFloat(uint16_t& address);

	//
	//	As above but writes a float
	//
	static void writeFloat(float value,uint16_t& address);
	
	//
	//	Checks the header at the EEPROM address provided
	//	is correct for the linearizer type
	//
	static bool checkHeader(LineariserType type,uint16_t& address);
	
	//
	//	Writes the header at the EEPROM address provided
	//
	static void writeHeader(LineariserType type,uint16_t& address);

	static const char *getHeaderString(LineariserType type);
};

#endif //__LINEARISERSTORAGE_H__
