 
#ifndef EEPROM_H
#define EEPROM_H

#define SLAVE_READ_E		0xA1
#define SLAVE_WRITE_E		0xA0


void write_external_eeprom(unsigned char address1,  unsigned char data);
unsigned char read_external_eeprom(unsigned char address1);


/*#define SLAVE_WRITE_EEPROM  0xA0
#define SLAVE_READ_EEPROM   0xA1

void ext_eeprom_24C02_byte_write(unsigned char addr,char data);
unsigned char ext_eeprom_24C02_byte_read(unsigned char addr);*/

#endif	/* XC_HEADER_TEMPLATE_H */

