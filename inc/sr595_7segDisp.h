/* 
 * File:   sr595_7segDisp.h
 * Author: dsolano
 *
 * Created on July 20, 2014, 2:17 PM
 */

#ifndef SR595_7SEGDISP_H
#define	SR595_7SEGDISP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <chip.h>


#define SR_CHANNEL              LPC_SSP1
#define SET_DOT                 0b10000000
#define CLEAR_DOT               0b01111111

    typedef enum{
        Letter,
        Number
    }TYPEOF_t;

void InitShiftRegister(void);
void ClearDot(void);
void SetDot(void);
void Count2Display(void);
void ClearDisplays(void);
void ShowLetters(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SR595_7SEGDISP_H */

