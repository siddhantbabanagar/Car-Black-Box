#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#define DASH_BOARD       0x01
#define MENU_FLAG        0x02
#define PASSWORD_SCREEN  0x03
#define VIEW_LOG         0x04
#define CLEAR_LOG        0x05
#define DOWNLOAD_LOG     0x06
#define CHANGE_PSWD      0x07
#define SET_TIME         0x08

#define RESET_NOTHING    0x00
#define RESET_MENU       0x12
#define RESET_PASSWORD   0x13
#define RESET_VIEW_LOG   0x14
#define RESET_MEMEORY    0x15

#define RETURN_SUCCESS   0xA0
#define RETURN_BACK      0xA1
#define TASK_FAIL        0xA3 


void display_dashboard(unsigned char *event,unsigned char speed);
void log_event(unsigned char event[],unsigned char speed);

unsigned char check_password(unsigned char key, unsigned char reset_flag);
//unsigned char menu_screen(unsigned char key,unsigned char reset_flag);
unsigned char menu_screen(unsigned char key,unsigned char reset_flag, unsigned char *menu_pos);

void view_log(unsigned char key,unsigned char reset_flag);
void clear_log(void);
unsigned char change_password(unsigned char key,unsigned char reset_flag);
unsigned char set_time(unsigned char key, unsigned char reset_flag);
void download_log(void);
#endif	/* XC_HEADER_TEMPLATE_H */

