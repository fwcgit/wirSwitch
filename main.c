#include<reg52.h>
#include<Uart.h>

sfr WDT_CONTR   = 0xc1;  
sfr P1M1 = 0x91;
sfr P1M0 = 0x92;

void sysInit();
void Timer0Init(void);

unsigned int waitCount = 0;
unsigned int lookDogCount = 0;

unsigned char SEND_FLAG = 0;
unsigned char LOOK_DOG_FLAG = 0;

void main()
{
	
	P1M1 = 0x00;
	P1M0 = 0x00;
	
	sysInit();
	
	WDT_CONTR = 0x06;
	WDT_CONTR |= 0x20;
	
	while(1)
	{
		if(LOOK_DOG_FLAG)
		{
			LOOK_DOG_FLAG = 0;
			WDT_CONTR |= 1 << 4;
		}
		
		if(SEND_FLAG)
		{
			SEND_FLAG = 0;
		}
		
		paraerData();
	}
}


void sysInit()
{
	
	Timer0Init();
	
	UART1_Init();
	
}

void Timer0Init(void)		
{
	AUXR |= 0x80;	
	TMOD &= 0xF0;		
	TL0 = 0x40;		
	TH0 = 0xa2;		
	TF0 = 0;
	ET0 = 1;
	TR0 = 1;		
}

void tm0_isr() interrupt 1 using 1
{
	

	
	if(waitCount++ ==3000)
	{
		SEND_FLAG = 1;
		waitCount = 0;
	}
	if(lookDogCount++ == 1000)
	{
		lookDogCount = 0;
		LOOK_DOG_FLAG = 1;
	}
		
}