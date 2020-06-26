#include <chip.h>
#include "gfx/TouchPanel/TouchPanel.h"
#include <gfx/open1788_bsp.h>
#include <ssp_17xx_40xx.h>

#define BUF_SIZE    0x01   // 1 Byte
// SPI Data Setup structure variable   

/* Private variables ---------------------------------------------------------*/
static Matrix matrix;
static Coordinate DispX;
static Coordinate screen;


Coordinate ScreenSample[3];

Coordinate DisplaySample[3] = { { 45, 45 }, { 400, 60 }, { 240, 250 } };

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2
static FunctionalState setCalibrationMatrix( Coordinate * displayPtr,Coordinate * screenPtr,Matrix * matrixPtr);
static Coordinate *TP_GetAdXY(uint32_t *x,uint32_t *y);
static Coordinate *Read_XTP2046(void);
static void delay_ms(uint32_t ms);


/*******************************************************************************
 * Function Name  : delay_ms
 * Description    : Delay Time
 * Input          : - cnt: Delay Time
 * Output         : None
 * Return         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
static void delay_ms(uint32_t ms) {
	uint16_t i, j;
	for (i = 0; i < ms; i++) {
		for (j = 0; j < 1141; j++)
			;
	}
}

/*******************************************************************************
 * Function Name  : delay_ms
 * Description    : Delay Time
 * Input          : - cnt: Delay Time
 * Output         : None
 * Return         : None
 * Return         : None
 * Attention		 : None
 *******************************************************************************/
static void delay_us(uint32_t cnt) {
	uint16_t i;
	for (i = 0; i < cnt; i++) {
		uint8_t us = 12;
		while (us--) {
			;
		}
	}
}

static uint8_t SPI_WriteByte(uint8_t data) {
	while (!(LPC_SSP1->SR & SSP_STAT_TFE));
	LPC_SSP1->DR = data;		// send a byte
	while (!(LPC_SSP1->SR & SSP_STAT_RNE));
	return LPC_SSP1->DR;		// Receive a byte
}


/*******************************************************************************
 * Function Name  : setCalibrationMatrix
 * Description    : Calculate K A B C D E F
 * Input          : None
 * Output         : None
 * Return         :
 * Attention		 : None
 *******************************************************************************/
static FunctionalState setCalibrationMatrix(Coordinate * displayPtr,
		Coordinate * screenPtr, Matrix * matrixPtr) {

	FunctionalState retTHRESHOLD = ENABLE;
	matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x)
			* (screenPtr[1].y - screenPtr[2].y))
			- ((screenPtr[1].x - screenPtr[2].x)
					* (screenPtr[0].y - screenPtr[2].y));
	if (matrixPtr->Divider == 0) {
		retTHRESHOLD = DISABLE;
	} else {
		matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x)
				* (screenPtr[1].y - screenPtr[2].y))
				- ((displayPtr[1].x - displayPtr[2].x)
						* (screenPtr[0].y - screenPtr[2].y));

		matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x)
				* (displayPtr[1].x - displayPtr[2].x))
				- ((displayPtr[0].x - displayPtr[2].x)
						* (screenPtr[1].x - screenPtr[2].x));

		matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x
				- screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y
				+ (screenPtr[0].x * displayPtr[2].x
						- screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y
				+ (screenPtr[1].x * displayPtr[0].x
						- screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y;

		matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y)
				* (screenPtr[1].y - screenPtr[2].y))
				- ((displayPtr[1].y - displayPtr[2].y)
						* (screenPtr[0].y - screenPtr[2].y));

		matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x)
				* (displayPtr[1].y - displayPtr[2].y))
				- ((displayPtr[0].y - displayPtr[2].y)
						* (screenPtr[1].x - screenPtr[2].x));

		matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y
				- screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y
				+ (screenPtr[0].x * displayPtr[2].y
						- screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y
				+ (screenPtr[1].x * displayPtr[0].y
						- screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y;
	}
	return (retTHRESHOLD);
}

/*******************************************************************************
 * Function Name  : TP_GetAdXY
 * Description    : Read ADS7843
 * Input          : None
 * Output         : None
 * Return         :
 * Attention		 : None
 *******************************************************************************/
static Coordinate *TP_GetAdXY(uint32_t *x, uint32_t *y) {
	uint32_t tmpx;
	uint32_t tmpy;
	//static Coordinate screen;

	TouchPanel_CS_SELECTED();
//    delay_us(5);
	SPI_WriteByte(CHX);
	/* read X */
	tmpx = (SPI_WriteByte(0x00) & 0x7F) << 5; /* read MSB bit[11:8] */
//    delay_us(5);
	tmpx |= SPI_WriteByte(0X00) >> 3; /* read LSB bit[7:0] and prepare read Y */
//    delay_us(5);
	SPI_WriteByte(CHY);
//    delay_us(5);
	tmpy = (SPI_WriteByte(0x00) & 0x7F) << 5; /* read MSB bit[11:8] */
//    delay_us(5);
	tmpy |= SPI_WriteByte(0x00) >> 3; /* read LSB bit[7:0] */
//    delay_us(5);
	SPI_WriteByte(1 << 7); /* �ٴδ��ж� */
//    delay_us(5);
	TouchPanel_CS_DESELECTED();
	screen.x = tmpx;
	screen.y = tmpy;
	*x = tmpx;
	*y = tmpy;

	return &screen;
}


/*******************************************************************************
 * Function Name  : Read_Ads7846
 * Description    : Get TouchPanel X Y
 * Input          : None
 * Output         : None
 * Return         : Coordinate *
 * Attention		 : None
 *******************************************************************************/
static Coordinate* Read_XTP2046(void) {
	//static Coordinate screen;
	uint32_t m0, m1, m2, TP_X[1], TP_Y[1], temp[3];
	uint8_t count = 0;
	int buffer[2][9] = { { 0 }, { 0 } };

	do {
		TP_GetAdXY(TP_X, TP_Y);
		buffer[0][count] = TP_X[0];
		buffer[1][count] = TP_Y[0];
		count++;
	}while (!PENIRQ_READ() && count < 9); /* TP_INT_IN  */

	if (count == 9) /* Average X Y  */
	{
		/* Average X  */
		temp[0] = (buffer[0][0] + buffer[0][1] + buffer[0][2]) / 3;
		temp[1] = (buffer[0][3] + buffer[0][4] + buffer[0][5]) / 3;
		temp[2] = (buffer[0][6] + buffer[0][7] + buffer[0][8]) / 3;

		m0 = temp[0] - temp[1];
		m1 = temp[1] - temp[2];
		m2 = temp[2] - temp[0];

		m0 = m0 > 0 ? m0 : (-m0);
		m1 = m1 > 0 ? m1 : (-m1);
		m2 = m2 > 0 ? m2 : (-m2);

		if (m0 > THRESHOLD && m1 > THRESHOLD && m2 > THRESHOLD)
			return 0;

		if (m0 < m1) {
			if (m2 < m0)
				screen.x = (temp[0] + temp[2]) / 2;
			else
				screen.x = (temp[0] + temp[1]) / 2;
		} else if (m2 < m1)
			screen.x = (temp[0] + temp[2]) / 2;
		else
			screen.x = (temp[1] + temp[2]) / 2;

		/* Average Y  */
		temp[0] = (buffer[1][0] + buffer[1][1] + buffer[1][2]) / 3;
		temp[1] = (buffer[1][3] + buffer[1][4] + buffer[1][5]) / 3;
		temp[2] = (buffer[1][6] + buffer[1][7] + buffer[1][8]) / 3;
		m0 = temp[0] - temp[1];
		m1 = temp[1] - temp[2];
		m2 = temp[2] - temp[0];
		m0 = m0 > 0 ? m0 : (-m0);
		m1 = m1 > 0 ? m1 : (-m1);
		m2 = m2 > 0 ? m2 : (-m2);
		if (m0 > THRESHOLD && m1 > THRESHOLD && m2 > THRESHOLD)
			return 0;

		if (m0 < m1) {
			if (m2 < m0)
				screen.y = (temp[0] + temp[2]) / 2;
			else
				screen.y = (temp[0] + temp[1]) / 2;
		} else if (m2 < m1)
			screen.y = (temp[0] + temp[2]) / 2;
		else
			screen.y = (temp[1] + temp[2]) / 2;
		return &screen;

	}
	return 0;
}

void touch_init(void) {
	/*IRQ*/
	PENIRQ_INPUT();
	TouchPanel_CS_OUTPUT();
	TouchPanel_CS_DESELECTED();
	Chip_SSP_Init(LPC_SSP1);
	Chip_SSP_SetBitRate(LPC_SSP1, 1000000);
	Chip_SSP_Enable(LPC_SSP1);
}

/*******************************************************************************
 * Function Name  : getDisplayPoint
 * Description    : Touch panel X Y to display X Y
 * Input          : None
 * Output         : None
 * Return         :
 * Attention		 : None
 *******************************************************************************/
FunctionalState touch_get_displaypoint(Coordinate * displayPtr, Coordinate * screenPtr,
		Matrix * matrixPtr) {
	FunctionalState retTHRESHOLD = ENABLE;

	if (matrixPtr->Divider != 0) {
		/* XD = AX+BY+C */
		displayPtr->x = ((matrixPtr->An * screenPtr->x)
				+ (matrixPtr->Bn * screenPtr->y) + matrixPtr->Cn)
				/ matrixPtr->Divider;

		/* YD = DX+EY+F */
		displayPtr->y = ((matrixPtr->Dn * screenPtr->x)
				+ (matrixPtr->En * screenPtr->y) + matrixPtr->Fn)
				/ matrixPtr->Divider;
	} else {
		retTHRESHOLD = DISABLE;
	}
	return (retTHRESHOLD);
}

/*******************************************************************************
 * Function Name  : TouchPanel_Calibrate
 * Description    :
 * Input          : None
 * Output         : none
 * Return         : None
 * Attention	  : get current read from Touch, store in ScreenSample array
 * 					and calibrate Store results in MATRIX structure
 *******************************************************************************/
void touch_calibrate(void) {
	uint8_t i;
	Coordinate * Ptr;

	for (i = 0; i < 3; i++) {
		open1788_LCD_Clear(0X0000);
		//GUI_Text(120, 40, "Touch crosshair to calibrate", White, Black);
		delay_ms(5000);
		open1788_LCD_display_coord(DisplaySample[i].x, DisplaySample[i].y, Red, Blue);

		// Get Touch X, Y current reading
		do {
			Ptr = Read_XTP2046();
		} while (Ptr == (void*) 0);

		ScreenSample[i].x = Ptr->x;
		ScreenSample[i].y = Ptr->y;
	}
	setCalibrationMatrix(&DisplaySample[0], &ScreenSample[0], &matrix);
	open1788_LCD_Clear(0X0000);
}

Matrix* touch_get_matrix_handle(void){
	return &matrix;
}

Coordinate* touch_get_display_coord_handle(void){
	return &DispX;
}

Coordinate* touch_get_screen_coord_handle(void){
	return Read_XTP2046();
}

/*********************************************************************************************************
 END FILE
 *********************************************************************************************************/

