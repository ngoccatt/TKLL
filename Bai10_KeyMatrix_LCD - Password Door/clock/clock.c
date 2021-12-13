#include "clock.h"

#include "..\i2c\i2c.h"
#include "..\lcd\lcd.h"
#include "..\button_matrix\button.h"
#include "..\password_door\password_door.h"

#define     ADDRESS_FIRST_PROGRAM   0x20

unsigned char clockState =  INIT_CLOCK;

unsigned char second = 0,minute = 0,hour = 0;
unsigned char temp_hour = 0;
unsigned char temp_minute = 0;
unsigned char temp_second = 0;

unsigned char late_hour = 0;
unsigned char late_minute = 0;
unsigned char late_second = 0;

void SetupTimeForRealTime();
void SetupForFirstProgram(void);
void DisplayRealTime();

void DisplayTempTime(unsigned char hour, unsigned char minute, unsigned char second);
void smolClock();
void smolLateClock();
unsigned char isButtonIncrease();
unsigned char isButtonDecrease();
unsigned char isButtonMode();

unsigned int blinking = 0;


//CLOCK SECTION



void SetupTimeForRealTime()
{
    second = 50;
    minute = 59;
    hour = 23;
    
    write_ds1307(ADDRESS_SECOND, second);
    write_ds1307(ADDRESS_MINUTE, minute);
    write_ds1307(ADDRESS_HOUR, hour);
}

void SetupForFirstProgram(void)
{
    if(Read_DS1307(ADDRESS_FIRST_PROGRAM) != 0x22)
    {
        SetupTimeForRealTime();
        Write_DS1307(ADDRESS_FIRST_PROGRAM, 0x22);
    } 
}

void DisplayRealTime()
{
    second = Read_DS1307(ADDRESS_SECOND);
    minute = Read_DS1307(ADDRESS_MINUTE);
    hour = Read_DS1307(ADDRESS_HOUR);
//    date = Read_DS1307(ADDRESS_DATE);
//    month = Read_DS1307(ADDRESS_MONTH);

    if(hour < 10)
    {
        LcdPrintStringS(1,7,"0");
        LcdPrintNumS(1,8,hour);
    }
    else
        LcdPrintNumS(1,7,hour);
    
    LcdPrintStringS(1,9,":");
    if(minute < 10)
    {
        LcdPrintStringS(1,10,"0");
        LcdPrintNumS(1,11,minute);
    }
    else
        LcdPrintNumS(1,10,minute);

    LcdPrintStringS(1,12,":");
    if(second < 10)
    {
        LcdPrintStringS(1,13,"0");
        LcdPrintNumS(1,14,second);
    }
    else
        LcdPrintNumS(1,13,second);
    
}

void DisplayTempTime(unsigned char hour, unsigned char minute, unsigned char second)
{
//    second = Read_DS1307(ADDRESS_SECOND);
//    minute = Read_DS1307(ADDRESS_MINUTE);
//    hour = Read_DS1307(ADDRESS_HOUR);
//    date = Read_DS1307(ADDRESS_DATE);
//    month = Read_DS1307(ADDRESS_MONTH);

    if(hour < 10)
    {
        LcdPrintStringS(1,7,"0");
        LcdPrintNumS(1,8,hour);
    }
    else
        LcdPrintNumS(1,7,hour);
    
    LcdPrintStringS(1,9,":");
    if(minute < 10)
    {
        LcdPrintStringS(1,10,"0");
        LcdPrintNumS(1,11,minute);
    }
    else
        LcdPrintNumS(1,10,minute);

    LcdPrintStringS(1,12,":");
    if(second < 10)
    {
        LcdPrintStringS(1,13,"0");
        LcdPrintNumS(1,14,second);
    }
    else
        LcdPrintNumS(1,13,second);
    
}

void smolClock() {
    
    switch (clockState) {
        case INIT_CLOCK:
                LcdPrintLineS(0,"CHANGE TIME");
                DisplayRealTime();
                if (isButtonMode()) {
                    clockState = GET_TIME;
                }
            break;
        case GET_TIME:
                temp_second = Read_DS1307(ADDRESS_SECOND);
                temp_minute = Read_DS1307(ADDRESS_MINUTE);
                temp_hour = Read_DS1307(ADDRESS_HOUR);
                clockState = CHANGE_HOUR;
            break;
            case CHANGE_HOUR:
                
                DisplayTempTime(temp_hour, temp_minute, temp_second);
                if (key_code[0] == 0 && key_code[1] == 0) {
                    if (blinking < 10) {
                         LcdPrintStringS(1,7,"  ");
                    }
                }
                if (isButtonIncrease()) {
                    temp_hour = (temp_hour + 1) % 24;
                }
                if (isButtonDecrease()) {
                    temp_hour--;
                    if (temp_hour > 23) temp_hour = 23;
                }
                if (isButtonMode()) {
                    clockState = CHANGE_MINUTE;
                }
            break;
            case CHANGE_MINUTE:
               
                DisplayTempTime(temp_hour, temp_minute, temp_second);
                if (key_code[0] == 0 && key_code[1] == 0) {
                    if (blinking < 10) {
                         LcdPrintStringS(1,10,"  ");
                    }
                }
                if (isButtonIncrease()) {
                    temp_minute = (temp_minute + 1) % 60;
                }
                if (isButtonDecrease()) {
                    temp_minute--;
                    if (temp_minute > 59) temp_minute = 59;
                }
                if (isButtonMode()) {
                    clockState = CHANGE_SECOND;
                }
            break;
            case CHANGE_SECOND:
                
                DisplayTempTime(temp_hour, temp_minute, temp_second);
                if (key_code[0] == 0 && key_code[1] == 0) {
                    if (blinking < 10) {
                         LcdPrintStringS(1,13,"  ");
                    }
                }
                if (isButtonIncrease()) {
                    temp_second = (temp_second + 1) % 60;
                }
                if (isButtonDecrease()) {
                    temp_second--;
                    if (temp_second > 59) temp_second = 59;
                }
                if (isButtonMode()) {
                    clockState = SET_TIME;
                }
            break;
            case SET_TIME:
                write_ds1307(ADDRESS_SECOND, temp_second);
                write_ds1307(ADDRESS_MINUTE, temp_minute);
                write_ds1307(ADDRESS_HOUR, temp_hour);
                clockState = INIT_CLOCK;
            break;
        default:
            clockState = INIT_CLOCK;
            break;
            
    }
    blinking = (blinking + 1) % 20;
}

void smolLateClock() {
    switch (clockState) {
        case INIT_CLOCK:
            LcdPrintLineS(0,"CHANGE LATE TIME");
            DisplayTempTime(late_hour, late_minute, late_second);
            if (isButtonMode()) {
                clockState = CHANGE_HOUR;
            }
        break;

        case CHANGE_HOUR:
            
            DisplayTempTime(late_hour, late_minute, late_second);
            if (key_code[0] == 0 && key_code[1] == 0) {
                if (blinking < 10) {
                     LcdPrintStringS(1,7,"  ");
                }
            }
            if (isButtonIncrease()) {
                late_hour = (late_hour + 1) % 24;
                
            }
            if (isButtonDecrease()) {
                late_hour--;
                if (late_hour > 23) late_hour = 23;
                
            }
            if (isButtonMode()) {
                clockState = CHANGE_MINUTE;
            }
        break;
        case CHANGE_MINUTE:
            
            DisplayTempTime(late_hour, late_minute, late_second);
            if (key_code[0] == 0 && key_code[1] == 0) {
                if (blinking < 10) {
                     LcdPrintStringS(1,10,"  ");
                }
            }
            if (isButtonIncrease()) {
                late_minute = (late_minute + 1) % 60;
                
            }
            if (isButtonDecrease()) {
                late_minute--;
                if (late_minute > 59) late_minute = 59;
                
            }
            if (isButtonMode()) {
                clockState = CHANGE_SECOND;
            }
        break;
        case CHANGE_SECOND:
            
            DisplayTempTime(late_hour, late_minute, late_second);
            if (key_code[0] == 0 && key_code[1] == 0) {
                if (blinking < 10) {
                     LcdPrintStringS(1,13,"  ");
                }
            }
            if (isButtonIncrease()) {
                late_second = (late_second + 1) % 60;
                
            }
            if (isButtonDecrease()) {
                late_second--;
                if (late_second > 59) late_second = 59;
                
            }
            if (isButtonMode()) {
                clockState = INIT_CLOCK;
            }
        break;
        default:
            clockState = INIT_CLOCK;
        break;
            
    }
    blinking = (blinking + 1) % 20;
}

unsigned char isButtonIncrease() {
    if (key_code[0] == 1 || (key_code[0] > 20 && key_code[0] % 5 == 0)) return 1;
    else return 0;
}

unsigned char isButtonDecrease() {
    if (key_code[1] == 1  || (key_code[1] > 20 && key_code[1] % 5 == 0)) return 1;
    else return 0;
}

unsigned char isButtonMode() {
    if (key_code[2] == 1) return 1;
    else return 0;
}