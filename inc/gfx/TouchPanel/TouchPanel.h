/*********************************************************************************************************
*
* File                : TouchPanel.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef _TOUCHPANEL_H_
#define _TOUCHPANEL_H_

#include  <define_pins.h>

/* Private typedef -----------------------------------------------------------*/
typedef	struct POINT 
{
   uint16_t x;
   uint16_t y;
}Coordinate;

typedef struct Matrix{
long double An,  
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,     
            Divider ;
} Matrix ;



/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2


/* Private define ------------------------------------------------------------*/

#define	CHX 	0x90
#define	CHY 	0xD0
DEFINE_PIN(TouchPanel_CS,1,8)
DEFINE_PIN(PENIRQ,2,15)

void 		touch_init(void);
void 		touch_calibrate(void);
FunctionalState touch_get_displaypoint(Coordinate * displayPtr,Coordinate * screenPtr,Matrix * matrixPtr );
Matrix* 	touch_get_matrix_handle(void);
Coordinate* touch_get_display_coord_handle(void);
Coordinate* touch_get_screen_coord_handle(void);

#endif  /*_TOUCHPANEL_H_*/

