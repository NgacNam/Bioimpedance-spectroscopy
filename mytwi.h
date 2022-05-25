/*Prosedures*/
int  TWI_init(void);
void  TWI_wait_int(void);
void  TWI_stop(void);

/*Functions*/
unsigned  char  TWI_send_adr(unsigned  char  adr);
unsigned  char  TWI_set_memloc(unsigned  char  mem_location);
unsigned  char  TWI_send_byte(unsigned  char  data);
unsigned  char  TWI_byte_write(unsigned  char  reg_addr,  unsigned  char  data);
unsigned  char  TWI_block_write(unsigned  char  reg_location, unsigned  char  byte_number,  unsigned  char  *TWI_data);
unsigned  char  TWI_byte_read(unsigned  reg_addr);
unsigned  char  TWI_block_read(unsigned  char  reg_addr, unsigned  char  byte_number,  unsigned  char  *TWI_data);
