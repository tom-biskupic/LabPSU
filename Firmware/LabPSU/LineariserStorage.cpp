/* 
* LineariserStorage.cpp
*
* Created: 8/09/2016 1:00:56 AM
* Author: tom
*/

#include "LineariserStorage.h"
#include <avr/eeprom.h>
#include <string.h>

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
	
	Linearizer::Point VOLTS_DAC_POINTS[] =
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
	    
	int NUM_VOLTS_DAC_POINTS = sizeof(VOLTS_DAC_POINTS)/sizeof(VOLTS_DAC_POINTS[0]);
	    
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

void LineariserStorage::loadLineariser(
	const LineariserType	type,
	Linearizer&				linearizer )
{
	uint16_t address = calculateAddress(type);
	
	if ( ! checkHeader(type,address) )
	{
		loadDefault(type,linearizer);
	}
	
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
}
	
void LineariserStorage::saveLinearizer(const LineariserType type,const Linearizer& linearizer)
{
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
	return (STORED_LINEARIZER_MAX_SIZE + STORED_LINEARIZER_SPACING) * (int)type;
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
	const char *header = getHeaderString(type);
	
	char readHeader[HEADER_LENGTH];
	eeprom_read_block((void *)readHeader,(void *)address,HEADER_LENGTH);
	address += HEADER_LENGTH;
	
	return memcmp(readHeader,header,4)==0;
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
	Linearizer::Point	*points = NULL;
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
	for(uint16_t i=0;i<numPoints;i++)
	{
		linearizer.setPoint(i,points[i]);
	}
}
