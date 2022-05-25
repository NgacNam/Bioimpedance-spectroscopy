#include  <stdint.h>
#include  <avr/io.h>
#include  <avr/interrupt.h>
#include  <util/twi.h>
#include  "myusart.h"
#include  "mytwi.h"
/*Atmega32  I2C  bus  status*/
#define  START  0xa4
#define  Stop  0x94
#define  Trans  0x84

/*AD5933*/
#define  SLA_read  0x1B
#define  SLA_write  0x1A

#define  SUCCES  0xff


/*Wait  for  TWINT  flag  set*/
void  TWI_wait(void){
	int  j=0;
	while(!(TWCR  &(1<<TWINT))){
		j++;
	}
}

/*Initialize  TWI*/
int  TWI_init(void){
	TWBR=10;  /*Setter  SCL  frekvensen  til  ca.  100kHz  */
	TWCR=0x04;  /*Send  start  condition*/
	return  1;
}


unsigned  char  Send_start(void)
{
	TWCR=START;  //Send  START
	TWI_wait();  //Wait  for  TWI  interrupt  flag  to  be  set
	if((TWSR  &  0xF8)!=0x08  ||  (TWSR  &  0xF8)!=0x10)
	{
		return  TWSR;  //If  it  failed,  return  the  TWSR  value
	}
	return  0xFF;  //If  succeeded,  return  SUCCESS
}
/*Send  stop  condition*/
void  TWI_stop(void){
	TWCR=Stop;
}


/*Send  address*/
unsigned  char  TWI_send_adr(unsigned  char  adr){
	TWI_wait();
	TWDR=adr;
	TWCR=Trans;
	TWI_wait();
	/*If  nack  received  from  slave:*/
	if((TWSR  &  0xF8)!=  0x18  ||  (TWSR  &  0xF8)!=  0x40){
		return  TWSR;
	}
	return  SUCCES;
}


/*Send  one  byte  to  the  bus*/
unsigned  char  TWI_send_byte(unsigned  char  data){
	TWI_wait();
	TWDR=data;
	TWCR=Trans;
	TWI_wait();
	if((TWSR  &  0xF8)  !=  0x28)
	{
		/*If  ack  received  from  slave*/
		return  TWSR;
	}
	else{
		return  SUCCES;
	}
}
unsigned  char  TWI_set_memloc(unsigned  char  mem_location){
	Send_start();
	TWI_send_adr(SLA_write);
	TWI_send_byte(0xB0);  /*Adress  pointer  see  page  26  of  AD5933 	manual*/
	TWI_send_byte(mem_location);  /*Send  memory  location*/
	return  1;  /*Return  1  if  succeded*/
}

unsigned  char  TWI_byte_write(unsigned  char  reg_addr, unsigned  char  data){
	Send_start();
	TWI_send_adr(SLA_write);
	TWI_send_byte(reg_addr);
	TWI_send_byte(data);
	TWI_stop();
	return  1;  /*Return  1  when  succeded*/
}
/*Write  several  bytes  of  data*/
/*byte_number=number  of  data  bytes  to  be  sendt*/
unsigned  char  TWI_block_write(unsigned  char  reg_location, unsigned  char  byte_number,  unsigned  char  *TWI_data)
{
	int  i;
	TWI_set_memloc(reg_location);
	Send_start();
	TWI_send_adr(SLA_write);
	TWI_send_byte(0xA0);  /*Block  write  command,  page  26  AD5933 	manual*/
	TWI_send_byte(byte_number);
	for  (i=0;  i<byte_number;  i++){
		TWI_send_byte(*(TWI_data+i));
	}
	TWI_stop();
return  1; }

unsigned  char  TWI_byte_read(unsigned  reg_addr){
	TWI_set_memloc(reg_addr);
	Send_start();
	TWI_send_adr(SLA_read); TWCR=Trans;
	TWI_wait();
return  TWDR; }

unsigned  char    TWI_block_read(unsigned  char  reg_addr, unsigned  char  byte_number,  unsigned  char  *TWI_data)
{
	int  i;
	TWI_set_memloc(reg_addr);
	Send_start();
	TWI_send_adr(SLA_write);
	TWI_send_byte(0xA0);
	TWI_send_byte(byte_number); TWI_init();
	TWI_send_byte(SLA_read);
	for(i=0; i<byte_number; i++){
		*(TWI_data  +i)=TWDR;
		TWI_wait();
		TWCR|=(1<<TWEA);  /*Send  ACK  after  each  byte*/
	}
	TWCR=(0<<TWEA);  /*Send  NACK  to  signalise  last  byte 	(end  of 	read)*/
	TWI_wait();
	TWI_stop();
	return  *TWI_data;
}
