#include <xc.h>

unsigned char secs,return_time;
void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {
        if (++count == 1250) // 1sec
        {
            count = 0;

    if(secs > 0)
    {
        secs--;
    }

    if(return_time > 0)
    {
        return_time--;
    }
        
        }
        
        TMR2IF = 0;
    }
}