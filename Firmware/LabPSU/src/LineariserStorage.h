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
	static void saveLinearizer(const LineariserType type);
	
private:
	//
	//	Static only class
	//
	LineariserStorage();
	~LineariserStorage();
};

#endif //__LINEARISERSTORAGE_H__
