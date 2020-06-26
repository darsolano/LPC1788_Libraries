#ifndef _18B20_H
#define _18B20_H
#include "lpc177x_8x_gpio.h"
#include "bsp.h"


uint8_t* readID(void) ;

//------------DS18B20----------
unsigned int readTemp(void);

#endif /*_18B20_H*/

