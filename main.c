/*
 * ad5933_nocal.c
 *
 * Created: 27/05/2022 5:27:30 pm
 * Author : DELL
 */ 


#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "myusart.h"
#include "mytwi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


/*AD5933  registers  adresses*/
#define  real_high_reg  0x94
#define  real_low_reg  0x95
#define  im_high_reg  0x96
#define  im_low_reg  0x97
#define  status_reg  0x8F
#define  control_high_reg  0x80
#define  control_low_reg  0x81
#define  NumSet_high  0x8A
#define  NumSet_low  0x8B
#define  startfreq_reg  0x82
#define  freqinc_reg  0x85
#define  incsteps_reg  0x88


#define  AD5933CLK  16776000

double Rcalibration = 10000;
int Rgain[100];
int Igain[100];
double Impedance, phase;
double gainfactor[100];
double sys_phase[100];
char txBuf[100] = {0};
int cal_check=0;

unsigned  long  int  hextodec(unsigned  char  data_high, unsigned  char  data_low)
{
	unsigned  long  int  data;
	data=(unsigned  long  int)data_high*256+data_low;
	return  data;
}



int  main  (void)
{
	DDRD=(1<<PD4);
	//Declaration  of  variables
	unsigned  char*  data;
	
	int R, I, j;
	unsigned  long  int  i,  kl;
	char *val;
	unsigned char real_high, real_low;
	unsigned char im_low, im_high;
	unsigned long int  startfreq, numbinc, freqinc, stopfreq;
	/*Initializing  USART*/
	USART_init();
	/*Initializing  TWI*/
	i=TWI_init();


	/*Allocating  memory  for  pointer  which  is
	used  to  store  data  to  be  written  to  the  AD5933*/
	data  =  (unsigned  char*)  malloc(10*sizeof(unsigned  char));
	/*Allocating  memory  for  pointer  which is  used  to  store  data  from  usart*/
	val=(char*)malloc(10*sizeof(char));
	//Setting  up  connection  with  the  PC  interface
	
	com:
	val=USART_CharReceive();
	if((i=strncmp(val,  "z",1))==0)
	{
		if (val!="")
		{
			//nothing
		}
		
	}
	else
	{
		goto  com;
	}

	//USART_CharTransmit("AD5933");
	//Setting  startfreq  routine
	//startfreq=atoi(USART_CharReceive());
	startfreq=5000;
	stopfreq=100000;
	i=startfreq*32.0023195;
	*data=0x000000ff  &  (i>>16);
	*(data+1)=0x000000ff  &  (i>>8);
	*(data+2)=0x000000ff  &  i;
	TWI_block_write(startfreq_reg, 3, data);
	//sprintf(txBuf, "startfreq = %u", startfreq);
	//USART_CharTransmit(txBuf);
	_delay_ms(200);

	//Setting  number  of  increments
	//numbinc=atoi(USART_CharReceive());
	numbinc=200;
	*data=0x000000ff & (numbinc>>8);
	*(data+1)=0x000000ff & numbinc;
	TWI_block_write(incsteps_reg,  2,  data);
	
	//Setting  frequency  increment
	//freqinc=atoi(USART_CharReceive());
	freqinc = (stopfreq - startfreq)/numbinc;
	i=freqinc*32.0023195;
	*data=0x000000ff  &  (i>>16);
	*(data+1)=0x000000ff  &  (i>>8);
	*(data+2)=0x000000ff  &  i;
	TWI_block_write(freqinc_reg,  3,  data);
	//sprintf(txBuf, "freqinc = %u", freqinc);
	//USART_CharTransmit(txBuf);
	_delay_ms(200);
	
	/*User  input  on  number  of  settlings  from  USART*/
	/*Could  have  been  made  user  settable the  same  way  as  the  output  excitation  voltage*/
	TWI_byte_write(NumSet_high,  0x00);
	TWI_byte_write(NumSet_low,  0x64);
	//sprintf(txBuf, "numbinc = %u", numbinc);
	//USART_CharTransmit(txBuf);
	_delay_ms(200);

	//receiving  desired  voltrange  from  PC
	//val=USART_CharReceive();
	//if((i=strncmp(val,  "V1",2))==0){
	//j=1;
	//}
	//else  if((i=strncmp(val,  "V2",2))==0){
	//j=2;
	//}
	//else  if((i=strncmp(val,  "V3",2))==0){
	//j=3;
	//}
	//else  if((i=strncmp(val,  "V4",2))==0){
	//j=4;
	//}
	
	//set output voltage:
	//j=1: 2Vpp    j=2: 1Vpp    j=3: 400mVpp    j=4: 200mVpp
	j=1;
	//USART_CharTransmit("volranges");

	/*PLacing  AD5933  in  standby  mode,  see  manual 	p. 	20-21*/
	if(j==1){
		TWI_byte_write(control_high_reg, 0xb1);
	}
	else  if(j==2){
		TWI_byte_write(control_high_reg, 0xb7);
	}
	else  if(j==3){
		TWI_byte_write(control_high_reg, 0xb5);
	}
	else  if(j==4){
		TWI_byte_write(control_high_reg, 0xb3);
	}
	//Initialize  with  start  frequency:
	if(j==1){
		TWI_byte_write(control_high_reg, 0x11);
	}
	else  if(j==2){
		TWI_byte_write(control_high_reg, 0x17);
	}
	else  if(j==3){
		TWI_byte_write(control_high_reg, 0x15);
	}
	else  if(j==4){
		TWI_byte_write(control_high_reg, 0x13);
	}
	

	//Some  settling  time
	for(i=0;i==100;i++);

	//Start  sample  routine
	while(1){
		// 	val=USART_CharReceive();
		// 	if((i=strncmp(val,  "StSample",8))==0)
		// 	{
		/*Programming  start  frequency  sweep	and  voltage  range  and  PGA  gain*/
		if(j==1){
			TWI_byte_write(control_high_reg,  0x21);
		}
		else  if(j==2){
			TWI_byte_write(control_high_reg,  0x27);
		}
		else  if(j==3){
			TWI_byte_write(control_high_reg,  0x25);
		}
		else  if(j==4){
			TWI_byte_write(control_high_reg,  0x23);
		}
		break;
	}
	// 	}
	//USART_CharTransmit("StSample");
	
	start:
	//Waits  until  the  real  and  imaginary  data  in  the  AD5933 	is  valid
	while(!(TWI_byte_read(status_reg)  &  0x02));
	
	PORTD=(1<<PD4);
	//Reads  the  two  hex  values  from  the  real  register
	real_high=TWI_byte_read(real_high_reg);
	real_low=TWI_byte_read(real_low_reg);
	//Converting  the  real  value  to  decimal
	R = hextodec(real_high, real_low);
	_delay_ms(10);	
	
	im_high = TWI_byte_read(im_high_reg);
	im_low = TWI_byte_read(im_low_reg);
	I = hextodec(im_high, im_low);
	_delay_ms(10);
	
	
	//Transmit R,I 
	sprintf(txBuf, "%d", R);
	USART_CharTransmit(txBuf);
	_delay_ms(100);
	USART_transmit(',');
	sprintf(txBuf, "%d", I);
	USART_CharTransmit(txBuf);
	_delay_ms(100);
	USART_transmit('\n');
	
	PORTD=(0<<PD4);_delay_ms(100);
	//Test  if  the  sweep  is  complete,  if  not  complete  program  increment  frequency
	if((TWI_byte_read(status_reg)  &  0x04)==0)
	{
		for(kl=0;  kl==10000;  kl++);
		if(j==1){
			TWI_byte_write(control_high_reg,  0x31);
		}
		else  if(j==2){
			TWI_byte_write(control_high_reg,  0x37);
		}
		else  if(j==3){
			TWI_byte_write(control_high_reg,  0x35);
		}
		else  if(j==4){
			TWI_byte_write(control_high_reg,  0x33);
		}
		goto  start;
	}

	//If  complete  programming  power  down  mode
	else{
		TWI_byte_write(control_high_reg,  0xA1);
		
		goto  com;
	}
}




