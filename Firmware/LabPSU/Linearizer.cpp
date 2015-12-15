/* 
* Linearizer.cpp
*
* Created: 23/08/2015 9:54:45 PM
* Author: tom
*/

#include "Linearizer.h"
#include "math.h"

const struct Linearizer::Point Linearizer::ZERO_POINT = {0,0.0f};
	
Linearizer::Linearizer(const Point *points, int numPoints,uint16_t min,uint16_t max) 
    : m_points(points), m_numPoints(numPoints),m_min(min),m_max(max)
{
}

const uint16_t Linearizer::valueToCode(const float value) const
{
	//
	//	If the value is between zero and the first point
	//	we interpolate between zero and this point and use Zero as the
	//	base point
	if ( value < m_points[0].value )
	{
		return interpolateCode(ZERO_POINT,m_points[1],ZERO_POINT,value);
	}
	
	for(int i=1;i<m_numPoints;i++)
	{
		if ( value < m_points[i].value )
		{
			return interpolateCode(m_points[i-1],m_points[i],m_points[i-1],value);
		}
	}
	
	//
	//	So the value is greater than the biggest point. In this case we extrapolate
	//	from the last point using the gradient between the last two points
	//
	if ( m_numPoints < 2 )
	{
		return interpolateCode(
		    ZERO_POINT,					//	Second last point
		    m_points[m_numPoints-1],	//	Last Point
		    m_points[m_numPoints-1],	//	Last Point
		    value);
	}
	else
	{
		return interpolateCode(
			m_points[m_numPoints-2],	//	Second last point
			m_points[m_numPoints-1],	//	Last Point
			m_points[m_numPoints-1],	//	Last Point
			value);
	}
}
	
const float Linearizer::codeToValue(const uint16_t code) const
{
	//
	//	If the value is between zero and the first point
	//	we interpolate between zero and this point and use Zero as the
	//	base point
	if ( code < m_points[0].code )
	{
		return interpolateValue(ZERO_POINT,m_points[1],ZERO_POINT,code);
	}
	
	for(int i=1;i<m_numPoints;i++)
	{
		if ( code < m_points[i].code )
		{
			return interpolateValue(m_points[i-1],m_points[i],m_points[i-1],code);
		}
	}
	
	//
	//	So the value is greater than the biggest point. In this case we extrapolate
	//	from the last point using the gradient between the last two points
	//
	if ( m_numPoints < 2 )
	{
		return interpolateValue(
			ZERO_POINT,					//	Second last point
			m_points[m_numPoints-1],	//	Last Point
			m_points[m_numPoints-1],	//	Last Point
			code);
	}
	else
	{
		return interpolateValue(
			m_points[m_numPoints-2],	//	Second last point
			m_points[m_numPoints-1],	//	Last Point
			m_points[m_numPoints-1],	//	Last Point
			code);
	}
}

uint16_t Linearizer::interpolateCode(
	const Point&	point1,
	const Point&	point2,
	const Point&	basePoint,
	const float		value  ) const
{
	float gradient = (point2.value - point1.value)/(float)(point2.code - point1.code);
	
	double codeAsFloat = (double)((value - basePoint.value)/gradient + (float)basePoint.code);
    
    if ( codeAsFloat > (double)m_max )
    {
        codeAsFloat = (double)m_max;
    }
    
    if ( codeAsFloat < (double)m_min )
    {
        codeAsFloat = (double)m_min;
    }
    
    return (uint16_t)round(codeAsFloat);
}

float Linearizer::interpolateValue(
	const Point&	point1,
	const Point&	point2,
	const Point&	basePoint,
	const uint16_t	code ) const
{
	float gradient = (point2.code - point1.code)/(float)(point2.value - point1.value);
	
	return (float)(code - basePoint.code)/gradient + (float)basePoint.value;
};
