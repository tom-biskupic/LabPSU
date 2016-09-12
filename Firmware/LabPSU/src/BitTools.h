/*
 * BitTools.h
 *
 * Created: 9/09/2015 9:05:22 PM
 *  Author: tom
 */ 


#ifndef BITTOOLS_H_
#define BITTOOLS_H_

class BitTools
{
public:
    static inline uint8_t boolToBit(const bool value)
    {
        return value==true ? 1 : 0;
    }	
};

#endif /* BITTOOLS_H_ */