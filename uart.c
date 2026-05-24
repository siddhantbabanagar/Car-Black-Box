#include <xc.h>
#include "uart.h"

#define _XTAL_FREQ 20000000

void init_uart(void)
{
    TRISC6 = 0;  // TX pin
    TRISC7 = 1;  // RX pin

    SPBRG = 129; // 9600 baud for 20MHz
    BRGH = 1;

    SYNC = 0;
    SPEN = 1;

    TXEN = 1;
}

void putch(unsigned char data)
{
    while(TXIF == 0);   // wait until buffer empty
    TXREG = data;

    __delay_us(50);     // ? IMPORTANT (stability for virtual COM)
}

void puts_uart(const char *str)
{
    while(*str)
    {
        putch(*str++);
    }
}
/*#include <xc.h>
#include "uart.h"

void init_uart(unsigned long baud)
{
    /* Setting RC6 and RC7 to work as Serial Port */
    //SPEN = 1;
    
    /* Continuous Reception Enable Bit */
    //CREN = 1;
    
    /* Baud Rate Setting Register */
    /*SPBRG = (FOSC / (16 * baud)) - 1;
}*/

/*unsigned char getchar(void)
{
    // Wait for the byte to be received
    while (RCIF != 1)
    {
        continue;
    }
    
    // Clear the interrupt flag 
    RCIF = 0;
    
    // Return the data to the caller 
    return RCREG;
}*/

/*void putchar(unsigned char data)
{
    // Transmit the data to the Serial Port 
    TXREG = data;
    
    // Wait till the transmission is complete 
    do
    {
        continue;
    } while (TXIF != 1);
    
    // Clear the interrupt flag 
    TXIF = 0;
}*/

/*void puts(const char *s)
{
    while (*s)
    {
        putchar(*s++);
    }
}*/