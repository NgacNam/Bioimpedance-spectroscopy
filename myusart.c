#include  <avr/io.h>
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  "myusart.h"

int  USART_init(void){
	//Initializing  the  USART
	//  Turn  on  the  transmission  and  reception  :
	UCSRB  |=  (1  <<  RXEN)  |  (1  <<  TXEN);
	//Use  8-bit  character  sizes:
	UCSRC  |=  (1  <<  URSEL)  |  (1  <<  UCSZ0)  |  (1  <<  UCSZ1);
	//  Load  lower  8-bits  of  the  baud  rate
	//value  into  the  low  byte  of  the  UBRR register
	UBRRL  =  BAUD_PRESCALE;
	//  Load  upper  8-bits  of  the  baud  rate
	//value  into  the  high  byte  of  the  UBRR  register
	UBRRH  =  (BAUD_PRESCALE  >>  8);
	return  1;
}


void  USART_transmit(char  data){
	//  Do nothing until UDR is ready for more data to be written to it
	while  ((UCSRA  &  (1  <<  UDRE))  ==  0)  {};
	UDR  =  data;
}

int  USART_receive(void){
	//char  data;
	//  Do  nothing  until  data  have  been  recieved  and  is  ready  to  be  read 	from  UDR
	while  ((UCSRA  &  (1  <<  RXC))  ==  0)  {};
	return  UDR;
}

void  USART_CharTransmit(char*  data)
{
	int  n;
	n=0;
	while  (1)
	{
		
		//  Do  nothing  until  UDR  is  ready  for  more  data
		while  ((UCSRA  &  (1  <<  UDRE))  ==  0)  {};
		UDR  =*(data+n);
		n++;
		if(!(*(data+(n)))){
			break;
		}
	}
	
	//USART_transmit('\n');
	//USART_transmit('\r');
}

char*  USART_CharReceive(void){
	int  i;
	char*  data;
	char  temp;
	data  =  (char*)  malloc(30*sizeof(char));
	for(i=0;  i<30;  i++){
		temp  =USART_receive();
		//If  received  data  is  equal  to  carriage  return
		if(temp=='\r'){
			break;
		}
		*(data+i)=temp;
	}
	*(data  +i)='\0';
	return  data;
}
