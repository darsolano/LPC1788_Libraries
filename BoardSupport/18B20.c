#include "18B20.h"


#define SET_DQ() GPIO_SetValue(1,(1<<5));	 
#define CLR_DQ() GPIO_ClearValue(1,(1<<5));
#define OUT_DQ() GPIO_SetDir(1,(1<<5), 1); 
#define IN_DQ()  GPIO_SetDir(1,(1<<5), 0);
#define GET_DQ() (GPIO_ReadValue(1) & (1<<5))

void delay_us (unsigned long zms) 
{
 	unsigned long i,j;
	for(j=zms;j>0;j--)
		for(i=20;i>0;i--);
}

//-----复位-----
void resetOnewire(void) 
{
	OUT_DQ();
    CLR_DQ();
    delay_us(750);
    SET_DQ();
	IN_DQ(); 
	while(GET_DQ());
	while(!(GET_DQ()));
	OUT_DQ();
}

//-----读数据-----
uint8_t rOnewire(void)
{
    uint8_t data=0,i=0;
    for(i=0;i<8;i++)
    {
		data=data>>1;
		OUT_DQ();
        CLR_DQ();
        delay_us(5);
		SET_DQ();
		delay_us(15);
		IN_DQ();
        if(GET_DQ()) data|=0x80;
        else while(!(GET_DQ()));
		delay_us(60);
		SET_DQ();
	}
	return(data);
}
//-----写数据-----
void wOnewire(uint8_t data)
{
    uint8_t i=0;
	OUT_DQ();
    for(i=0;i<8;i++)
    {
        if(data&0x01)
		{
            CLR_DQ();
            delay_us(5);
            SET_DQ();
            delay_us(85); //65
		}
   	 	else
   	 	{
       		CLR_DQ(); 
        	delay_us(90);  //65
        	SET_DQ();
        	delay_us(5);
    	}
    	data=data>>1;
    }
}
uint8_t* readID(void) 
{ 
	uint8_t ID[8],i,*p;
	resetOnewire(); 
	wOnewire(0x33);
	for(i=0;i<8;i++)
	{ID[i]=rOnewire();}
	p=ID;
	return p;
}

//-----DS18B20转换温度-----
void convertDs18b20(void) 
{ 
    resetOnewire(); 
    wOnewire(0xcc); 
    wOnewire(0x44); 
} 
//------------DS18BB0读温度----------
unsigned int readTemp(void) 
{ 
	uint8_t temp1,temp2;
	convertDs18b20();
    resetOnewire(); 
    wOnewire(0xcc); 
    wOnewire(0xbe); 	
    temp1=rOnewire(); 
    temp2=rOnewire(); 
	return (((temp2<<8)|temp1)*6.25); //0.0625=xx, 0.625=xx.x, 6.25=xx.xx
}

