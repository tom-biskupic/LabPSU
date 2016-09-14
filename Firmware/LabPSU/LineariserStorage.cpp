/* 
* LineariserStorage.cpp
*
* Created: 8/09/2016 1:00:56 AM
* Author: tom
*/

#include "LineariserStorage.h"
#include <avr/eeprom.h>
#include <string.h>
#include <stdio.h>
namespace
{
	//
	//	Don't start at zero as it can get over-written in the event of
	//	a brown out
	//
	const int START_ADDRESS=0x10;

	const int HEADER_LENGTH=4;
	
	//
	//	Using a struct would be better but I am concerned about RAM
	//	usage. Hence I am reading the points one at a time and need
	//	to calculate lengths this way
	//
	const int STORED_LINEARIZER_MAX_SIZE = 
		HEADER_LENGTH  +
		sizeof(uint16_t) * 2 +			// Min Max
		sizeof(uint16_t) +				// Num points
		Linearizer::MAX_POINTS * sizeof(Linearizer::Point);
	
	//
	//	Gap between linearizers
	//	
	const int STORED_LINEARIZER_SPACING = 10;
	
	const char *VDAC_HEAD = "VDAC";
	const char *VADC_HEAD = "VADC";
	const char *IDAC_HEAD = "IDAC";	
	const char *IADC_HEAD = "IADC";		
	
	const Linearizer::Point VOLTS_DAC_POINTS[] =
	{
		{0x0,       +9.95773822E-01},
		{0x7ff0,    +1.49968045E+01},
		{0xffe0,    +2.99912247E+01}
	};
	    
	const int NUM_VOLTS_DAC_POINTS = sizeof(VOLTS_DAC_POINTS)/sizeof(VOLTS_DAC_POINTS[0]);
	    
	const Linearizer::Point VOLTS_ADC_POINTS[] =
	{
		{0x2e,    +3.22477947E-03},
		{0x7ef4,    +1.49968153E+01},
		{0xfdd7,    +2.99913063E+01}
	};
	const int NUM_VOLTS_ADC_POINTS = sizeof(VOLTS_ADC_POINTS)/sizeof(VOLTS_ADC_POINTS[0]);
	    
	const Linearizer::Point AMPS_DAC_POINTS[] =
	{
		{0x0,       -4.38587476E-06},
		{0x7ff0,    +2.48762629E+00},
		{0xefe2,    +4.65568860E+00}
	};
	const int NUM_AMPS_DAC_POINTS = sizeof(AMPS_DAC_POINTS)/sizeof(AMPS_DAC_POINTS[0]);
	    
	const Linearizer::Point AMPS_ADC_POINTS[] =
	{
		{0xf,       +2.30258425E-05},
		{0x7078,    +2.18464638E+00},
		{0xefaa,    +4.65613044E+00}
	};
	const int NUM_AMPS_ADC_POINTS = sizeof(AMPS_ADC_POINTS)/sizeof(AMPS_ADC_POINTS[0]);
}

void LineariserStorage::loadLineariser(
	const LineariserType	type,
	Linearizer&				linearizer )
{
    //printf("Loading linearizer\r\n");
	uint16_t address = calculateAddress(type);
	
	if ( ! checkHeader(type,address) )
	{
		loadDefault(type,linearizer);
        return;
	}
	
    //printf("Loading linearizer from EEPROM\r\n");
	uint16_t max = readWord(address);
	uint16_t min = readWord(address);
	linearizer.setMinMax(min,max);
	
	uint16_t numPoints = readWord(address);
	linearizer.setNumPoints(numPoints);
	for(uint16_t i=0;i<numPoints;i++)
	{
		Linearizer::Point point;
		point.code = readWord(address);
		point.value = readFloat(address);
		linearizer.setPoint(i,point);
	}
    //printf("Loading linearizer from EEPROM - Done\r\n");
}
	
void LineariserStorage::saveLinearizer(const LineariserType type,const Linearizer& linearizer)
{
    //printf("Saving linearizer\r\n");
	uint16_t address = calculateAddress(type);
	
	writeHeader(type,address);
	
	writeWord(linearizer.getMax(),address);
	writeWord(linearizer.getMin(),address);

	uint16_t numPoints = linearizer.getNumPoints();
	writeWord(numPoints,address);
	for(uint16_t i=0;i<numPoints;i++)
	{
		Linearizer::Point point = linearizer.getPoint(i);
		writeWord(point.code,address);
		writeFloat(point.value,address);
	}
}

uint16_t LineariserStorage::calculateAddress(LineariserType type)
{
	return 10 + (STORED_LINEARIZER_MAX_SIZE + STORED_LINEARIZER_SPACING) * (int)type;
}

const char *LineariserStorage::getHeaderString(LineariserType type)
{
	const char *header = NULL;
	
	switch(type)
	{
		case CURRENT_ADC:
			header = IADC_HEAD;
			break;
		case CURRENT_DAC:
			header = IDAC_HEAD;
			break;
		case VOLTAGE_ADC:
			header = VADC_HEAD;
			break;
		case VOLTAGE_DAC:
			header = VDAC_HEAD;
			break;
		default:
			header="NFIM";
			break;
	}

	return header;
}

bool LineariserStorage::checkHeader(LineariserType type,uint16_t& address)
{
    //printf("Checking header\r\n");
	const char *header = getHeaderString(type);
	
	char readHeader[HEADER_LENGTH];
	eeprom_read_block((void *)readHeader,(void *)address,HEADER_LENGTH);
    //printf("eeprom_read_block\r\n");
	address += HEADER_LENGTH;
	
	return memcmp(readHeader,header,HEADER_LENGTH)==0;
}

void LineariserStorage::writeHeader(LineariserType type,uint16_t& address)
{
	const char *header = getHeaderString(type);
	eeprom_write_block(header,(void *)address,HEADER_LENGTH);
	address += HEADER_LENGTH;	
}

uint16_t LineariserStorage::readWord(uint16_t& address)
{
	uint16_t value = eeprom_read_word((uint16_t *)address);
	address += sizeof(uint16_t);
	return value;
}

void LineariserStorage::writeWord(uint16_t value, uint16_t& address)
{
	eeprom_write_word((uint16_t *)address,value);
	address += sizeof(uint16_t);	
}

void LineariserStorage::writeFloat(float value,uint16_t& address)
{
	eeprom_write_float((float *)address,value);
	address += sizeof(float);
}

float LineariserStorage::readFloat(uint16_t& address)
{
	float value = eeprom_read_float((float *)address);
	address += sizeof(float);
	return value;
}

void LineariserStorage::loadDefault(LineariserType type,Linearizer& linearizer)
{
    //printf("Loading default linearizer\r\n");
	const Linearizer::Point	*points = NULL;
	uint16_t			numPoints = 0;
	
	switch(type)
	{
		case CURRENT_ADC:
			points = AMPS_ADC_POINTS;
			numPoints = NUM_AMPS_ADC_POINTS;
			break;
		case CURRENT_DAC:
			points = AMPS_DAC_POINTS;
			numPoints = NUM_AMPS_DAC_POINTS;
			break;
		case VOLTAGE_ADC:
			points = VOLTS_ADC_POINTS;
			numPoints = NUM_VOLTS_ADC_POINTS;
			break;
		case VOLTAGE_DAC:
			points = VOLTS_DAC_POINTS;
			numPoints = NUM_VOLTS_DAC_POINTS;
			break;
	}
	
	linearizer.setMinMax(0,0xffff);
	linearizer.setNumPoints(numPoints);
    //printf("numPoints=%d\r\n",numPoints);
	for(uint16_t i=0;i<numPoints;i++)
	{
        //printf("Setting point %d\r\n",i);
		linearizer.setPoint(i,points[i]);
	}
    
    //printf("Finished loading default\r\n");
}
