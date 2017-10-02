#include "Uart.h"
#include<string.h>
#include<reg52.h>
typedef unsigned int uint;
typedef unsigned char byte;

byte buy = 0;
uint rxIndex = 0;
byte idata buff[REVEIVE_BUFF_SIZE] ;
sfr IAP_CONTR   =   0xC7; 

byte isRec = 0;
byte isHand = 0;

void Uart1() interrupt 4 using 1
{
	byte rByte;
	
	if(RI)
	{
		RI = 0;
		
		rByte = SBUF;
			
		if(rxIndex >= REVEIVE_BUFF_SIZE)
		{
			isRec = 0;
			rxIndex = 0;
		}
		if(rByte == 0x3C)
		{
			isRec = 1;
		}
		
		if(isRec)
		{
			buff[rxIndex++] = rByte;
		}
		
		
		if(rByte == 0x0D && isRec)
		{
			isHand = 1;
			isRec = 0;
			rxIndex = 0;
		}
	}
	
	if(TI)
	{
		TI = 0;
		buy = 0;
	}
}

void sendData(byte dat)
{
		while(buy);
	
		buy = 1;
		SBUF = dat;
}

void sendStr(char *dat)
{
	while(*dat)
	{
		sendData(*dat++);
	}
}
void sendStrlen(char *dat,int len)
{
	uint i;
	for(i=0;i<len;i++)
	{
		sendData(*(dat+i));
	}
}



void paraerData()
{
	uint check = 0;
	byte resp[4];
	
	if(isHand)
	{
		
		if(*buff == 0x3C)
		{
			//check=*(buff+1)+*(buff+2)+*(buff+3);
			//check &= 0xff;
			//if(check == *(buff+4))
			//{
					if(*(buff+1) == 0xA0)
				{
						if(*(buff+2) == 0x01)
						{
							RELAY_S1 = 0;
						}
						else
						{
							RELAY_S1 = 1;
						}
						
						if(*(buff+3) == 0x01)
						{
							RELAY_S2 = 0;
						}
						else
						{
							RELAY_S2 = 1;
						}
						resp[0] = 0x3C;
						resp[1] = 0xAD;
						resp[2] = 0x0D;
						sendStrlen(resp,3);
				}
				else if(*(buff+1) = 0xaa)
				{
					upload_status();
				}
				else if(*(buff+1) == 0xdd)
				{
					IAP_CONTR = 0x60;
				}
			//}

		}
		memset(buff,0,sizeof(buff));
		isHand = 0;
	}
	
}


void sleep(uint sleep)
{
	uint i,j;
	for(i = sleep ; i > 0 ; i --)
		for(j = 2000 ; j > 0 ; j --);
}
void sleep_us(unsigned int t)
{
	uint i,j;
	for(i = t ; i > 0 ; i --)
		for(j = 3 ; j > 0 ; j --);
}

void UART1_Init()
{
	SCON = 0X50;
	T2L = (65536 - (FOSC/4/BAUD));
	T2H = (65536 - (FOSC/4/BAUD)) >> 8;
	AUXR |= 0X14;
	AUXR |= 0X01;
	ES =1;
	EA = 1;
}

void upload_status()
{
	unsigned char temp;
	unsigned char dat[5];
	
	temp = RELAY_S1;
	temp = temp << 4 | RELAY_S2;
	
	dat[0] = 0x3a;
	dat[1] = 0xaa;
 	dat[2] = temp;
	dat[3] = (dat[1] + dat[2]) & 0xff;
	dat[4] = 0x0d;
}




