#define F_CPU 8000000UL
#define  USART_BAUDRATE  9600
#define  BAUD_PRESCALE  (((F_CPU  /  (USART_BAUDRATE  *  16UL)))  -  1)
#define  DEBUG  0
//Funksjonsdefinisjoner:
int  USART_init(void);
void  USART_transmit(char  data);
int  USART_receive(void);
void  USART_CharTransmit(char*  data);
char*  USART_CharReceive(void);
