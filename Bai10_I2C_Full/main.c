#include "main.h"
// Noi khai bao hang so
#define     LED     PORTD
#define     ON      1
#define     OFF     0
#define     ENABLE      1
#define     DISABLE     0

#define     ADDRESS_FIRST_PROGRAM   0x20

#define     ADDRESS_HOUR_ALARM      0x10
#define     ADDRESS_MINUTE_ALARM    0x11
#define     ADDRESS_BIT_ALARM       0x12
#define     ADDRESS_FLAG_ALARM      0x13

#define     INIT_SYSTEM         0
#define     SET_HOUR_ALARM      1
#define     SET_MINUTE_ALARM    2
#define     BIT_ALARM           3
#define     COMPARE             4
#define     ALARM               5

#define     SET_HOUR            6
#define     SET_MINUTE          7
#define     SET_DAY             8
#define     SET_DATE            9
#define     SET_MONTH           10
#define     SET_YEAR            11
// Noi khai bao bien toan cuc
unsigned char second = 0,minute = 0,hour = 0;
unsigned char day = 0;
unsigned char date = 0,month = 0,year = 0;
unsigned char statusSetUpAlarm = INIT_SYSTEM;
unsigned char statusSetUpTime = INIT_SYSTEM;
unsigned char statusAlarm = INIT_SYSTEM;
unsigned char hourAlarm = 0, minuteAlarm = 0, bitAlarm = 0;
unsigned char timeBlink = 0;
unsigned int timeAlarm = 0;
unsigned char flagAlarm = 0;
unsigned char bitEnable = ENABLE;
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);

void SetupTimeForRealTime();
void SetupForFirstProgram(void);
void ReadDataFromDS1307(void);
void BaiTap_I2C();
void DisplayTime();
void SetUpAlarm();
void SetUpTime();
unsigned char isButtonMode();
unsigned char isButtonAlarm();
unsigned char isButtonModeHold();
unsigned char isButtonAlarmHold();
unsigned char isButtonIncrease();
unsigned char isButtonDecrease();
void DisplayAlarmTime();
unsigned char CompareTime();
void Alarm();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	unsigned int k = 0;
        
	init_system();
//        lcd_clear();
        LcdClearS();
        SetupForFirstProgram();
        delay_ms(1000);
        LcdPrintString(0,6,"ALARM CLOCK");    
        ReadDataFromDS1307();
        //delay_ms(2000);
	while (1)
	{
            while (!flag_timer3);
            flag_timer3 = 0;
            scan_key_matrix();
            k = (k+1)%2000;
            
            DisplayTime();
            SetUpTime();
            SetUpAlarm();
            Alarm();
            DisplayLcdScreenOld();
	}
}
// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void init_system(void)
{
        TRISB = 0x00;		//setup PORTB is output
        TRISD = 0x00;
        init_lcd();
//        LED = 0x00;
        init_interrupt();
        delay_ms(1000);
        init_timer0(4695);//dinh thoi 1ms sai so 1%
        init_timer1(9390);//dinh thoi 2ms
        init_timer3(46950);//dinh thoi 10ms
        SetTimer0_ms(2);
        SetTimer1_ms(10);
        SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
        init_key_matrix();
        init_i2c();
}

///////I2C
void SetupTimeForRealTime()
{
    second = 55;
    minute = 59;
    hour = 23;
    day = 7;
    date = 31;
    month = 12;
    year = 13;
    Write_DS1307(ADDRESS_SECOND, second);
    Write_DS1307(ADDRESS_MINUTE, minute);
    Write_DS1307(ADDRESS_HOUR, hour);
    Write_DS1307(ADDRESS_DAY, day);
    Write_DS1307(ADDRESS_DATE, date);
    Write_DS1307(ADDRESS_MONTH, month);
    Write_DS1307(ADDRESS_YEAR, year);
}

void SetupForFirstProgram(void)
{
    if(Read_DS1307(ADDRESS_FIRST_PROGRAM) != 0x22)
    {
        SetupTimeForRealTime();
        Write_DS1307(ADDRESS_HOUR_ALARM, 0);
        Write_DS1307(ADDRESS_MINUTE_ALARM, 0);
        Write_DS1307(ADDRESS_BIT_ALARM, 0);
        Write_DS1307(ADDRESS_FLAG_ALARM, 0);

        Write_DS1307(ADDRESS_FIRST_PROGRAM, 0x22);
    }
}

void ReadDataFromDS1307(void)
{
    hourAlarm = Read_DS1307(ADDRESS_HOUR_ALARM);
    minuteAlarm = Read_DS1307(ADDRESS_MINUTE_ALARM);
    bitAlarm = Read_DS1307(ADDRESS_BIT_ALARM);
    flagAlarm = Read_DS1307(ADDRESS_FLAG_ALARM);
}
void BaiTap_I2C()
{
    second = Read_DS1307(ADDRESS_SECOND);
    minute = Read_DS1307(ADDRESS_MINUTE);
    hour = Read_DS1307(ADDRESS_HOUR);
    day = Read_DS1307(ADDRESS_DAY);
    date = Read_DS1307(ADDRESS_DATE);
    month = Read_DS1307(ADDRESS_MONTH);
    year = Read_DS1307(ADDRESS_YEAR);
    
    LcdPrintNumS(0,0,year);
    LcdPrintNumS(0,3,month);
    LcdPrintNumS(0,6,date);
    LcdPrintNumS(0,9,day);
    LcdPrintNumS(1,0,hour);
    LcdPrintNumS(1,3,minute);
    LcdPrintNumS(1,6,second);
}

void DisplayTime()
{
    second = Read_DS1307(ADDRESS_SECOND);
    minute = Read_DS1307(ADDRESS_MINUTE);
    hour = Read_DS1307(ADDRESS_HOUR);
    day = Read_DS1307(ADDRESS_DAY);
    date = Read_DS1307(ADDRESS_DATE);
    month = Read_DS1307(ADDRESS_MONTH);
    year = Read_DS1307(ADDRESS_YEAR);

    //////day
    switch(day)
    {
        case 1:
            LcdPrintStringS(0,0,"SUN");
            break;
        case 2:
            LcdPrintStringS(0,0,"MON");
            break;
        case 3:
            LcdPrintStringS(0,0,"TUE");
            break;
        case 4:
            LcdPrintStringS(0,0,"WED");
            break;
        case 5:
            LcdPrintStringS(0,0,"THU");
            break;
        case 6:
            LcdPrintStringS(0,0,"FRI");
            break;
        case 7:
            LcdPrintStringS(0,0,"SAT");
            break;
    }
    if(hour < 10)
    {
        LcdPrintStringS(0,4,"0");
        LcdPrintNumS(0,5,hour);
    }
    else
        LcdPrintNumS(0,4,hour);
    
    LcdPrintStringS(0,6,":");
    if(minute < 10)
    {
        LcdPrintStringS(0,7,"0");
        LcdPrintNumS(0,8,minute);
    }
    else
        LcdPrintNumS(0,7,minute);

    LcdPrintStringS(0,9,":");
    if(second < 10)
    {
        LcdPrintStringS(0,10,"0");
        LcdPrintNumS(0,11,second);
    }
    else
        LcdPrintNumS(0,10,second);

    switch(bitAlarm)
    {
        case 0:
            LcdPrintStringS(0,13,"OFF");
            break;
        case 1:
            LcdPrintStringS(0,13,"ON ");
            break;
    }
    
    switch(month)
    {
        case 1:
            LcdPrintStringS(1,2,"JAN");
            break;
        case 2:
            LcdPrintStringS(1,2,"FEB");
            break;
        case 3:
            LcdPrintStringS(1,2,"MAR");
            break;
        case 4:
            LcdPrintStringS(1,2,"APR");
            break;
        case 5:
            LcdPrintStringS(1,2,"MAY");
            break;
        case 6:
            LcdPrintStringS(1,2,"JUN");
            break;
        case 7:
            LcdPrintStringS(1,2,"JUL");
            break;
        case 8:
            LcdPrintStringS(1,2,"AUG");
            break;
        case 9:
            LcdPrintStringS(1,2,"SEP");
            break;
        case 10:
            LcdPrintStringS(1,2,"OCT");
            break;
        case 11:
            LcdPrintStringS(1,2,"NOV");
            break;
        case 12:
            LcdPrintStringS(1,2,"DEC");
            break;
    }

    LcdPrintStringS(1,5," ");
    if(date < 10)
    {
        LcdPrintStringS(1,6," ");
        LcdPrintNumS(1,7,date);
    }
    else
        LcdPrintNumS(1,6,date);
    LcdPrintStringS(1,8," ");
    LcdPrintNumS(1,9,20);
    LcdPrintNumS(1,11,year);

}

void DisplayAlarmTime()
{
    LcdPrintStringS(0,0,"      ALARM     ");
    LcdPrintStringS(1,0,"  ");
    if(hourAlarm < 10)
    {
        LcdPrintStringS(1,2,"0");
        LcdPrintNumS(1,3,hourAlarm);
    }
    else
        LcdPrintNumS(1,2,hourAlarm);
    LcdPrintStringS(1,4,":");
    if(minuteAlarm < 10)
    {
        LcdPrintStringS(1,5,"0");
        LcdPrintNumS(1,6,minuteAlarm);
    }
    else
        LcdPrintNumS(1,5,minuteAlarm);
    LcdPrintStringS(1,7,"   ");
    switch(bitAlarm)
    {
        case 0:
            LcdPrintStringS(1,10,"OFF");
            break;
        case 1:
            LcdPrintStringS(1,10,"ON ");
            break;
    }
    LcdPrintStringS(1,13,"    ");
    
}

void SetUpTime()
{
    switch(statusSetUpTime)
    {
        case INIT_SYSTEM:
            if(isButtonMode() && (bitEnable == ENABLE))
                statusSetUpTime = SET_HOUR;
            break;
        case SET_HOUR:
            bitEnable = DISABLE;
            timeBlink = (timeBlink + 1)%20;
            if(timeBlink > 15)
                LcdPrintStringS(0,4,"  ");
            if(isButtonIncrease())
            {
                hour = (hour + 1)%24;
                Write_DS1307(ADDRESS_HOUR,hour);
            }
            if(isButtonDecrease())
            {
                hour = (hour - 1);
                if(hour > 23)
                    hour = 23;
                Write_DS1307(ADDRESS_HOUR,hour);
            }
            if(isButtonMode())
                statusSetUpTime = SET_MINUTE;
            if(isButtonModeHold())
            {
                LcdClearS();
                bitEnable = ENABLE;
                statusSetUpTime = INIT_SYSTEM;
            }
            break;
        case SET_MINUTE:
            bitEnable = DISABLE;
            timeBlink = (timeBlink + 1)%20;
            if(timeBlink > 15)
                LcdPrintStringS(0,7,"  ");
            if(isButtonIncrease())
            {
                minute = (minute + 1)%60;
                Write_DS1307(ADDRESS_MINUTE,minute);
            }
            if(isButtonDecrease())
            {
                minute = (minute - 1);
                if(minute > 59)
                    minute = 59;
                Write_DS1307(ADDRESS_MINUTE,minute);
            }
            if(isButtonMode())
                statusSetUpTime = SET_DAY;
            if(isButtonModeHold())
            {
                LcdClearS();
                bitEnable = ENABLE;
                statusSetUpTime = INIT_SYSTEM;
            }
            break;
        case SET_DAY:
            bitEnable = DISABLE;
            timeBlink = (timeBlink + 1)%20;
            if(timeBlink > 15)
                LcdPrintStringS(0,0,"   ");
            if(isButtonIncrease())
            {
                day = day + 1;
                if(day > 7)
                    day = 1;
                Write_DS1307(ADDRESS_DAY,day);
            }
            if(isButtonDecrease())
            {
                day = day - 1;
                if(day > 7)
                    day = 7;
                Write_DS1307(ADDRESS_DAY,day);
            }
            if(isButtonMode())
                statusSetUpTime = SET_DATE;
            if(isButtonModeHold())
            {
                LcdClearS();
                bitEnable = ENABLE;
                statusSetUpTime = INIT_SYSTEM;
            }
            break;
        case SET_DATE:
            bitEnable = DISABLE;
            timeBlink = (timeBlink + 1)%20;
            if(timeBlink > 15)
                LcdPrintStringS(1,6,"  ");
            if(isButtonIncrease())
            {
                date = date + 1;
                if(date > 31)
                    date = 1;
                Write_DS1307(ADDRESS_DATE,date);
            }
            if(isButtonDecrease())
            {
                date = date - 1;
                if(date < 1)
                    date = 31;
                Write_DS1307(ADDRESS_DATE,date);
            }
            if(isButtonMode())
                statusSetUpTime = SET_MONTH;
            if(isButtonModeHold())
            {
                LcdClearS();
                bitEnable = ENABLE;
                statusSetUpTime = INIT_SYSTEM;
            }
            break;
        case SET_MONTH:
            bitEnable = DISABLE;
            timeBlink = (timeBlink + 1)%20;
            if(timeBlink > 15)
                LcdPrintStringS(1,2,"   ");
            if(isButtonIncrease())
            {
                month = month + 1;
                if(month > 12)
                    month = 1;
                Write_DS1307(ADDRESS_MONTH,month);
            }
            if(isButtonDecrease())
            {
                month = month - 1;
                if(month < 1)
                    month = 12;
                Write_DS1307(ADDRESS_MONTH,month);
            }
            if(isButtonMode())
                statusSetUpTime = SET_YEAR;
            if(isButtonModeHold())
            {
                LcdClearS();
                bitEnable = ENABLE;
                statusSetUpTime = INIT_SYSTEM;
            }
            break;
        case SET_YEAR:
            bitEnable = DISABLE;
            timeBlink = (timeBlink + 1)%20;
            if(timeBlink > 15)
                LcdPrintStringS(1,9,"    ");
            if(isButtonIncrease())
            {
                year = year + 1;
                if(year > 99)
                    year = 0;
                Write_DS1307(ADDRESS_YEAR,year);
            }
            if(isButtonDecrease())
            {
                year = year - 1;
                if(year > 99)
                    month = 99;
                Write_DS1307(ADDRESS_YEAR,year);
            }
            if(isButtonMode())
            {
                LcdClearS();
                bitEnable = ENABLE;
                statusSetUpTime = INIT_SYSTEM;
            }
            if(isButtonModeHold())
            {
                LcdClearS();
                bitEnable = ENABLE;
                statusSetUpTime = INIT_SYSTEM;
            }
            break;
        default:
            bitEnable = ENABLE;
            statusSetUpAlarm = INIT_SYSTEM;
            break;

    }
}

void SetUpAlarm()
{
    switch(statusSetUpAlarm)
    {
        case INIT_SYSTEM:
            if(isButtonAlarm() && (bitEnable == ENABLE))
                statusSetUpAlarm = SET_HOUR_ALARM;
            break;
        case SET_HOUR_ALARM:
            bitEnable = DISABLE;
            DisplayAlarmTime();
            timeBlink = (timeBlink + 1)%20;
            if(timeBlink > 15)
                LcdPrintStringS(1,2,"  ");
            if(isButtonIncrease())
            {
                hourAlarm = (hourAlarm + 1)%24;
                Write_DS1307(ADDRESS_HOUR_ALARM,hourAlarm);
            }
            if(isButtonDecrease())
            {
                hourAlarm = (hourAlarm - 1);
                if(hourAlarm > 23)
                    hourAlarm = 23;
                Write_DS1307(ADDRESS_HOUR_ALARM,hourAlarm);
            }
            if(isButtonAlarm())
                statusSetUpAlarm = SET_MINUTE_ALARM;
            if(isButtonAlarmHold())
            {
                LcdClearS();
                bitEnable = ENABLE;
                statusSetUpAlarm = INIT_SYSTEM;
            }
            break;
        case SET_MINUTE_ALARM:
            bitEnable = DISABLE;
            DisplayAlarmTime();
            timeBlink = (timeBlink + 1)%20;
            if(timeBlink > 15)
                LcdPrintStringS(1,5,"  ");
            if(isButtonIncrease())
            {
                minuteAlarm = (minuteAlarm + 1)%60;
                Write_DS1307(ADDRESS_MINUTE_ALARM,minuteAlarm);
            }
            if(isButtonDecrease())
            {
                minuteAlarm = (minuteAlarm - 1);
                if(minuteAlarm > 59)
                    minuteAlarm = 59;
                Write_DS1307(ADDRESS_MINUTE_ALARM,minuteAlarm);
            }
            if(isButtonAlarm())
                statusSetUpAlarm = BIT_ALARM;
            if(isButtonAlarmHold())
            {
                LcdClearS();
                bitEnable = ENABLE;
                statusSetUpAlarm = INIT_SYSTEM;
            }
            break;
        case BIT_ALARM:
            bitEnable = DISABLE;
            DisplayAlarmTime();
            timeBlink = (timeBlink + 1)%20;
            if(timeBlink > 15)
                LcdPrintStringS(1,10,"   ");
            if(isButtonIncrease())
            {
                bitAlarm = (bitAlarm + 1)%2;
                Write_DS1307(ADDRESS_BIT_ALARM,bitAlarm);
            }
            if(isButtonDecrease())
            {
                bitAlarm = (bitAlarm - 1);
                if(bitAlarm > 1)
                    bitAlarm = 1;
                Write_DS1307(ADDRESS_BIT_ALARM,bitAlarm);
            }
            if(isButtonAlarm())
            {
                LcdClearS();
                statusSetUpAlarm = INIT_SYSTEM;
                bitEnable = ENABLE;
            }
            if(isButtonAlarmHold())
            {
                LcdClearS();
                bitEnable = ENABLE;
                statusSetUpAlarm = INIT_SYSTEM;
            }
            break;
        default:
            break;
    }
}

unsigned char isButtonMode()
{
    if (key_code[4] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonModeHold()
{
    if (key_code[4] == 10)
        return 1;
    else
        return 0;
}

unsigned char isButtonAlarm()
{
    if (key_code[8] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonAlarmHold()
{
    if (key_code[8] == 10)
        return 1;
    else
        return 0;
}

unsigned char isButtonIncrease()
{
    if (key_code[5] == 1 || (key_code[5] > 10 && key_code[5]%3 == 1))
        return 1;
    else
        return 0;
}

unsigned char isButtonDecrease()
{
    if (key_code[9] == 1 || (key_code[9] > 10 && key_code[9]%3 == 1))
        return 1;
    else
        return 0;
}

unsigned char CompareTime()
{
    if((hour == hourAlarm) && (minute == minuteAlarm) && (second == 0) && (bitAlarm == ON) && (bitEnable == ENABLE))
        return 1;
    else
        return 0;
}

void Alarm()
{
    static unsigned char timeBlink = 0;
    switch(statusAlarm)
    {
        case INIT_SYSTEM:
            statusAlarm = COMPARE;
            break;
        case COMPARE:
            if(CompareTime())
            {
                flagAlarm = 1;
                Write_DS1307(ADDRESS_FLAG_ALARM,flagAlarm);
            }
            if(flagAlarm)
                statusAlarm = ALARM;
            break;
        case ALARM:
            /// bao chuong
            bitEnable = DISABLE;
            timeBlink = (timeBlink + 1)%20;
            if(timeBlink < 10)
            {
                LcdClearS();
            }
            timeAlarm ++;
            if(timeAlarm > 400)
            {
                bitEnable = ENABLE;
                flagAlarm = 0;
                Write_DS1307(ADDRESS_FLAG_ALARM,flagAlarm);
                timeAlarm = 0;
                LcdClearS();
                statusAlarm = INIT_SYSTEM;
            }
            break;
    }
}