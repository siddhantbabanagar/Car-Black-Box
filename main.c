
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

#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)

unsigned char *gear[] = {"GN","GR","G1","G2","G3","G4"};

extern unsigned char return_time;

static void init_config(void) {
    init_i2c(100000); //100K
    init_digital_keypad();
    init_clcd();
    init_adc();
    init_timer2();
    init_ds1307();
    init_uart();
    
    PEIE = 1;
    GIE = 1;
}

void main(void) {

    init_config();
    unsigned char screen_flag = DASH_BOARD;
    unsigned char reset_flag = RESET_NOTHING;
    unsigned char speed = 0;
    unsigned char event[3] = "ON";  
    unsigned char key;
    unsigned char gr=0;
    unsigned char menu_pos;
    
    log_event(event,speed);
    //password is "1111"
    write_external_eeprom(0x00,'1');
    write_external_eeprom(0x01,'1');
    write_external_eeprom(0x02,'1');
    write_external_eeprom(0x03,'1');
    
    while (1) 
    {
        //read speed
        speed=(unsigned char)(read_adc()/10);
        if(speed > 99)
            speed = 99;
        //detect event based on key press
        key=read_digital_keypad(STATE);
        
        for(int j=3000;j--;);
        if(key == SW1)
        {
            strcpy(event, " C");
            //store event, speed, time into external eeprom
            log_event(event, speed);
        }
        
        else if(key == SW2){
            if(gr < 5)
                gr++; //event= "G1" "G2" "G3"..
            
            strcpy(event,gear[gr]);
            
            log_event(event, speed);
        }
        else if(key == SW3){
            if(gr > 0)
                gr--;
            strcpy(event,gear[gr]);
            
            log_event(event, speed);
        }
        else if((key == SW5 || key == SW4) && screen_flag == DASH_BOARD)
        {
            clear_screen();
            clcd_print(" Enter  Password ",LINE1(0));
            clcd_write(CURSOR_POS, 0);
            clcd_write(DISP_ON_AND_CURSOR_ON,0);
            __delay_us(100);
            screen_flag = PASSWORD_SCREEN;
            reset_flag = RESET_PASSWORD;
            TMR2ON = 1;
            
        }
        
        //if menu screen, long press the switch update the screen
        else if(screen_flag == MENU_FLAG && key == SW6)
        {
            switch(menu_pos)
            {
                case 0:
                    clear_screen();
                    clcd_print("# TIME     E  SP",LINE1(0));
                    screen_flag = VIEW_LOG;
                    reset_flag = RESET_VIEW_LOG;
                    break;
                case 1:
                    log_event("CL",speed);
                    clear_screen();
                    screen_flag = CLEAR_LOG;
                    reset_flag = RESET_MEMEORY;
                    break;
                case 2:
                    screen_flag = DOWNLOAD_LOG;
                    break;
                case 3: 
                    log_event("ST",speed);
                    clear_screen();
                    screen_flag = SET_TIME;
                    reset_flag = RESET_PASSWORD;
                    break;
                case 4: 
                    log_event("CP",speed);
                    clear_screen();
                    screen_flag = CHANGE_PSWD;
                    reset_flag = RESET_PASSWORD;
                    break;    
                    
            }
            return_time = 5;
        }
        
        //based on screen time display particular screen
        switch(screen_flag)
        {
            case DASH_BOARD :
                display_dashboard(event, speed);
                break;
            case PASSWORD_SCREEN :
                switch(check_password(key, reset_flag))
                {
                    case RETURN_BACK:
                            screen_flag = DASH_BOARD;
                            reset_flag=RESET_MENU;
                            clear_screen();
                            clcd_write(DISP_ON_AND_CURSOR_OFF,0);
                            TMR2ON = 0;
                        break;
                    case RETURN_SUCCESS:
                            screen_flag = MENU_FLAG;
                            reset_flag = RESET_MENU;
                            clear_screen();
                            clcd_write(DISP_ON_AND_CURSOR_OFF,0);
                            TMR2ON = 1;
                            return_time=5;
                            continue;
                            //break;
                        //break;                        
                }
                break;
            case MENU_FLAG:
                
            {
                unsigned char ret = menu_screen(key, reset_flag, &menu_pos);

                if(ret == RETURN_BACK)
                {
                    screen_flag = DASH_BOARD;
                    reset_flag=RESET_MENU;
                    clear_screen();
                    clcd_write(DISP_ON_AND_CURSOR_OFF, 0);
                    TMR2ON = 0;   // stop timer
                }
                else
                {
                    menu_pos = ret;
                }
                break;
            }
            case VIEW_LOG:
            {
                if(return_time == 0)
                {
                    screen_flag = MENU_FLAG;
                    reset_flag = RESET_MENU;
                    clear_screen();
                    clcd_write(DISP_ON_AND_CURSOR_OFF, 0);
                    TMR2ON = 0;
                }
                else
                {
                    view_log(key, reset_flag);
                }
                break;
            }
            case CLEAR_LOG:
                clear_log();
                clear_screen();
                screen_flag = MENU_FLAG;
                reset_flag = RESET_MENU;
                return_time=5;
                TMR2ON=1;
                break;    
            case CHANGE_PSWD:
                switch(change_password(key,reset_flag))
                    {
                        case RETURN_SUCCESS:
                            clear_screen();
                            clcd_write(DISP_ON_AND_CURSOR_OFF,INST_MODE);
                            __delay_us(100);
                            screen_flag=MENU_FLAG;
                            reset_flag=RESET_MENU;
                            continue;
                            //break;
                    }

                    break;
                    case SET_TIME:
                    {
                        char ret = set_time(key, reset_flag);

                        if(ret == RETURN_BACK)
                        {
                            screen_flag = MENU_FLAG;
                            reset_flag = RESET_MENU;
                            clear_screen();

                            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE); // ? VERY IMPORTANT

                            TMR2ON = 0;
                        }
                        break;
                    }   
                    case DOWNLOAD_LOG:
{
    clear_screen();
    clcd_print("Downloading...", LINE1(0));

    download_log();

    __delay_ms(500);   // ? VERY IMPORTANT (UART flush)

    clear_screen();
    clcd_print("Done", LINE1(0));
    __delay_ms(1500);

    screen_flag = MENU_FLAG;
    reset_flag = RESET_MENU;
    return_time = 5;
    TMR2ON = 1;

    break;
}
        }
        reset_flag = RESET_NOTHING; 
    }
    
}
