#include <xc.h>
#include <string.h>
#include "i2c.h"
#include"ds1307.h"
#include "clcd.h"
#include "digital_keypad.h"
#include "helper.h"
#include "adc.h"
#include "eeprom.h"
#include "timers.h"
#include "uart.h"

unsigned char clock_reg[3];
char time[7];  // "HH:MM:SS"
unsigned char log[11];
unsigned char pos = 0;//0 1 2 3 4 5 6 7 8 9 10 ->0

extern unsigned char secs;

extern unsigned char return_time;

unsigned char *menu[] = {"view log", "clear log", "Download log", "Set time", "Change pwd"};

unsigned int event_count=0;

static void get_time()
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
    
    /* To store the time in HH:MM:SS format */
    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    clcd_putch(time[0],LINE2(0));
    clcd_putch(time[1],LINE2(1));
    
    clcd_putch(':',LINE2(2));
    
    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    
    clcd_putch(time[2],LINE2(3));
    clcd_putch(time[3],LINE2(4));
    
    clcd_putch(':',LINE2(5));
    
    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
    
    clcd_putch(time[4],LINE2(6));
    clcd_putch(time[5],LINE2(7));
    
}

void display_dashboard(unsigned char *event,unsigned char speed)
{
    clcd_print("TIME      EV  SP",LINE1(0));
    get_time();
    clcd_print(event, LINE2(10));
    clcd_putch(speed/10 + '0',LINE2(14));//65->6+'0'->'6'
    clcd_putch(speed%10 + '0',LINE2(15));//65->5+'0'->'6'
}
void log_event(unsigned char event[], unsigned char speed)
{
    unsigned char add;

    add = pos * 10;   // use current position first

    // Clear previous data (important)
    for(int i = 0; i < 10; i++)
    {
        write_external_eeprom(add + i, 0x00);
    }

    // Get latest time
    get_time();

    // Store HHMMSS
    strncpy(log, time, 6);

    // Store event (2 chars)
    strncpy(&log[6], event, 2);

    // Store speed
    log[8] = speed/10 + '0';
    log[9] = speed%10 + '0';

    log[10] = '\0';

    // Write to EEPROM
    for(int i = 0; i < 10; i++)
    {
        write_external_eeprom(add + i, log[i]);
    }

    // Update position AFTER writing
    pos++;
    if(pos == 10)
        pos = 0;

    if(event_count < 10)
        event_count++;
}
unsigned char check_password(unsigned char key, unsigned char reset_flag)
{
    static char user_pwd[4];
    static unsigned char attempt_rem,i;
    char svd_pwd[4];
    if(reset_flag==RESET_PASSWORD)
    {
        return_time=5;
        attempt_rem='3';
        i=0;
        user_pwd[0]='\0';
        user_pwd[1]='\0';
        user_pwd[2]='\0';
        user_pwd[3]='\0';
        key=ALL_RELEASED;
    }
    if(return_time==0)
    {
        return RETURN_BACK;
    }
    if (key==SW4 && i<4) //SW4 ->'0'
    {
        user_pwd[i]='0';
        clcd_putch('*',LINE2(6 + i));
        i++;
        return_time=5;
    }
    else if(key==SW5 && i<4) //SW5 ->'1'
    {
        user_pwd[i]='1';
        clcd_putch('*',LINE2(6+i));
        i++;
        return_time=5;
    }
    if(i==4)
    {
        for (int j=0;j<4;j++)
        {
            svd_pwd[j] = read_external_eeprom(j);
        }
        if (strncmp(user_pwd, svd_pwd,4)==0)
        {
            //Menu Screen
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, 0);
            clcd_print("Login Success", LINE1(0));
            __delay_ms(1500);
            return RETURN_SUCCESS;
        }
        else //wrong Password
        {
            attempt_rem--;
            if(attempt_rem=='0')
            {
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print(" You are blocked",LINE1(0));
                clcd_print("Wait...for 60sec",LINE2(0));
                secs=60;
                while(secs!=0)
                {
                    clcd_putch((secs/10) + '0', LINE2(11));
                    clcd_putch((secs%10) + '0', LINE2(12));
                }
                attempt_rem='3';
            }
            else
            {
                clear_screen();
                //prompt wrong Password 3sec
                clcd_print(" WRONG PASSWORD ",LINE1(0));
                clcd_putch(attempt_rem,LINE2(0));
                clcd_print(" attempt remain",LINE2(1));
                __delay_ms(3000);
            }
            clear_screen();
            clcd_print(" ENTER  PASSWORD ",LINE1(0));
            clcd_write(CURSOR_POS, INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON,INST_MODE);
            __delay_us(100);
            i=0;
            return_time=5;
        } 
    }
    return 0x10;
}
/*unsigned char check_password(unsigned char key, unsigned char reset_flag)
{
    static unsigned char user_pwd[4];   // FIXED (static)
    static unsigned char svd_pwd[4];
    static unsigned char attempt, i;

    if(reset_flag == RESET_PASSWORD)
    {
        attempt = 3;
        i = 0;
        memset(user_pwd, 0, 4);   // clear old values
        return_time = 5;
        key = ALL_RELEASED;
    }

    // Read user input
    if(key == SW4 && i < 4)
    {
        user_pwd[i] = '0';
        i++;
        clcd_putch('*', LINE2(i+4));
        return_time = 5;
    }
    else if(key == SW5 && i < 4)
    {
        user_pwd[i] = '1';
        i++;
        clcd_putch('*', LINE2(i+4));
        return_time = 5;
    }

    if(return_time == 0)
    {
        return RETURN_BACK;
    }

    // Compare after 4 inputs
    if(i == 4)
    {
        for(int j = 0; j < 4; j++)
        {
            svd_pwd[j] = read_external_eeprom(j);
        }

        if(strncmp(user_pwd, svd_pwd, 4) == 0)
        {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, 0);
            clcd_print("Login Success", LINE1(0));
            __delay_ms(1500);
            return RETURN_SUCCESS;
        }
        else
        {
            attempt--;

            if(attempt == 0)
            {
                clear_screen();
                clcd_print("Blocked 60 sec", LINE1(0));

                secs = 60;
                while(secs != 0)
                {
                    clcd_putch(secs/10 + '0', LINE2(9));
                    clcd_putch(secs%10 + '0', LINE2(10));
                    __delay_ms(1000);
                    secs--;
                }
                attempt = 3;
            }
            else
            {
                clear_screen();
                clcd_print("Wrong Password", LINE1(0));
                clcd_print("Attempts:", LINE2(0));
                clcd_putch(attempt + '0', LINE2(10));
                __delay_ms(1500);
            }

            clear_screen();
            clcd_print("Enter Password", LINE1(0));
            clcd_write(DISP_ON_AND_CURSOR_ON, 0);

            i = 0;
            memset(user_pwd, 0, 4);   // ? reset again
            return_time = 5;
        }
    }

    return 0x10;
}*/
unsigned char menu_screen(unsigned char key,unsigned char reset_flag, unsigned char *menu_pos)
{
    if(return_time == 0)
    {
        return RETURN_BACK;
    }

    if(reset_flag==RESET_MENU)
    {
        *menu_pos = 0;
        return_time=5;
        clear_screen();
    }

    if(key==SW5 && *menu_pos<4)
    {
        (*menu_pos)++;
        clear_screen();
        return_time=5;
    }
    else if(key==SW4 && *menu_pos>0)
    {
        (*menu_pos)--;
        clear_screen();
        return_time=5;
    }

    if(*menu_pos<4)
    {
        clcd_putch('*',LINE1(0));
        clcd_print(menu[*menu_pos],LINE1(2));
        clcd_print(menu[*menu_pos+1],LINE2(2));
    }
    else
    {
        clcd_print(menu[*menu_pos-1],LINE1(2));
        clcd_print(menu[*menu_pos],LINE2(2));
        clcd_putch('*',LINE2(0));
    }

    return *menu_pos;
}
void view_log(unsigned char key, unsigned char reset_flag)
{
    char rlog[11];
    unsigned char add;
    static unsigned char rpos;
    static unsigned char once = 1;   //flag to avoid blinking

    if(event_count == 0)
    {
        if(once)
        {
            clear_screen();
            clcd_print("# TIME     E  SP", LINE1(0));
            clcd_print("No Logs", LINE1(3));
            once = 0;
        }
        return;
    }

    once = 1; // reset flag when logs exist

    if(reset_flag == RESET_VIEW_LOG)
    {
        rpos = 0;
        return_time = 5;
        clear_screen();
    }

    if(key == SW5 && rpos < (event_count - 1))
    {
        rpos++;
        return_time = 5;
        clear_screen();
    }
    else if(key == SW4 && rpos > 0)
    {
        rpos--;
        return_time = 5;
        clear_screen();
    }

    clcd_print("# TIME     E  SP", LINE1(0));

    add = rpos * 10;

    for(int i = 0; i < 10; i++)
    {
        rlog[i] = read_external_eeprom(add + i);
    }

    clcd_putch(rpos + '0', LINE2(0));

    clcd_putch(rlog[0],LINE2(2));
    clcd_putch(rlog[1],LINE2(3));
    clcd_putch(':',LINE2(4));

    clcd_putch(rlog[2],LINE2(5));
    clcd_putch(rlog[3],LINE2(6));
    clcd_putch(':',LINE2(7));

    clcd_putch(rlog[4],LINE2(8));
    clcd_putch(rlog[5],LINE2(9));

    clcd_putch(rlog[6],LINE2(11));
    clcd_putch(rlog[7],LINE2(12));

    clcd_putch(rlog[8],LINE2(14));
    clcd_putch(rlog[9],LINE2(15));
}
void clear_log(void)
{
    // Erase all 100 bytes (10 logs × 10 bytes)
    for(int i = 0; i < 100; i++)
    {
        write_external_eeprom(i, 0xFF);  // or 0x00
    }

    // Reset log tracking variables
    pos = 0;
    event_count = 0;

    // Display message
    clear_screen();
    clcd_print("Logs Cleared", LINE1(0));
    clcd_print("Successfully", LINE2(0));
    __delay_ms(2000);
}
unsigned char change_password(unsigned char key, unsigned char reset_flag)
{
    static char pwd[9];
    static int pos, once;

    if(reset_flag == RESET_PASSWORD)
    {
        pos = 0;
        once = 1;
    }

    if(pos < 4 && once)
    {
        once = 0;
        clcd_print("Enter New Pwd", LINE1(0));
        clcd_write(LINE2(0), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
    }
    else if(pos >= 4 && once == 0)
    {
        once = 1;
        clear_screen();
        clcd_print("Re-enter Pwd", LINE1(0));
        clcd_write(LINE2(0), INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
    }

    if(key == SW4 && pos < 8)
    {
        pwd[pos] = '0';   // consistent with login
        clcd_putch('*', LINE2(pos % 4));
        pos++;
    }
    else if(key == SW5 && pos < 8)
    {
        pwd[pos] = '1';
        clcd_putch('*', LINE2(pos % 4));
        pos++;
    }

    if(pos == 8)
    {
        if(strncmp(pwd, &pwd[4], 4) == 0)
        {
            for(int i = 0; i < 4; i++)
            {
                write_external_eeprom(i, pwd[i]);
            }

            clear_screen();
            clcd_print("Pwd Changed", LINE1(0));
            clcd_print("Successfully", LINE2(0));
            __delay_ms(2000);
        }
        else
        {
            clear_screen();
            clcd_print("Pwd Mismatch", LINE1(0));
            __delay_ms(2000);
        }

        return RETURN_SUCCESS;
    }

    return 0x10;
}
unsigned char set_time(unsigned char key, unsigned char reset_flag)
{
    static unsigned char hour, min, sec;
    static unsigned char field;
    static unsigned char once;
    static unsigned char prev_key;

    if(reset_flag == RESET_PASSWORD)
    {
        unsigned char hr = read_ds1307(HOUR_ADDR);
        unsigned char mn = read_ds1307(MIN_ADDR);
        unsigned char sc = read_ds1307(SEC_ADDR);

        hour = ((hr >> 4) * 10) + (hr & 0x0F);
        min  = ((mn >> 4) * 10) + (mn & 0x0F);
        sec  = ((sc >> 4) * 10) + (sc & 0x0F);

        field = 0;
        prev_key = ALL_RELEASED;
        once = 1;
        return_time = 5;
    }

    // Timeout
    if(return_time == 0)
    {
        return RETURN_BACK;
    }

    // First time display
    if(once)
    {
        clear_screen();
        clcd_print("SET TIME", LINE1(4));
        once = 0;
    }

    // -------- DISPLAY TIME --------
    clcd_putch((hour/10)+'0', LINE2(4));
    clcd_putch((hour%10)+'0', LINE2(5));

    clcd_putch(':', LINE2(6));

    clcd_putch((min/10)+'0', LINE2(7));
    clcd_putch((min%10)+'0', LINE2(8));

    clcd_putch(':', LINE2(9));

    clcd_putch((sec/10)+'0', LINE2(10));
    clcd_putch((sec%10)+'0', LINE2(11));

    // -------- CURSOR USING '_' --------
    if(field == 0)
        clcd_putch('_', LINE2(5));   // HH
    else if(field == 1)
        clcd_putch('_', LINE2(8));   // MM
    else
        clcd_putch('_', LINE2(11));  // SS

    // -------- EDGE DETECTION --------
    if(key != ALL_RELEASED && prev_key == ALL_RELEASED)
    {
        // SW6 ? increment
        if(key == SW6)
        {
            if(field == 0)
                hour = (hour + 1) % 24;
            else if(field == 1)
                min = (min + 1) % 60;
            else
                sec = (sec + 1) % 60;

            return_time = 5;
        }

        // SW5 ? backward field
        else if(key == SW5)
        {
            field = (field == 0) ? 2 : field - 1;
            return_time = 5;
        }

        // SW4 ? forward field
        else if(key == SW4)
        {
            field = (field + 1) % 3;
            return_time = 5;
        }

        // SW1 ? SAVE
        else if(key == SW1)
        {
            write_ds1307(HOUR_ADDR, ((hour/10)<<4) | (hour%10));
            write_ds1307(MIN_ADDR,  ((min/10)<<4)  | (min%10));
            write_ds1307(SEC_ADDR,  ((sec/10)<<4)  | (sec%10));

            clear_screen();
            clcd_print("Time Updated", LINE1(0));
            __delay_ms(1500);

            return RETURN_BACK;
        }

        // SW2 ? EXIT without saving
        else if(key == SW2)
        {
            return RETURN_BACK;
        }
    }

    prev_key = key;

    return 0x10;
}

#define LOG_SIZE   10
#define MAX_LOGS   10

extern unsigned int event_count;

void download_log(void)
{
    unsigned char i, j;
    unsigned char address;
    unsigned char log[LOG_SIZE];

    // CSV HEADER
    puts_uart("Index,Time,Event,Speed\r\n");

    if(event_count == 0)
    {
        puts_uart("No Logs\r\n");
        return;
    }

    for(i = 0; i < event_count; i++)
    {
        address = i * LOG_SIZE;

        // Read log from EEPROM
        for(j = 0; j < LOG_SIZE; j++)
        {
            log[j] = read_external_eeprom(address + j);
        }

        // Index
        putch(i + '0');
        putch(',');

        // Time HH:MM:SS
        putch(log[0]); putch(log[1]); putch(':');
        putch(log[2]); putch(log[3]); putch(':');
        putch(log[4]); putch(log[5]);
        putch(',');

        // Event
        putch(log[6]); putch(log[7]);
        putch(',');

        // Speed
        putch(log[8]); putch(log[9]);

        puts_uart("\r\n");
    }

    puts_uart("END\r\n");
}