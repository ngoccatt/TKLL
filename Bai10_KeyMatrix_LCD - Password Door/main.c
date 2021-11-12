#include "main.h"
#include <ctype.h>
// Noi khai bao hang so
#define     LED     PORTD
#define     ON      1
#define     OFF     0

#define     INIT_SYSTEM        0
#define     ENTER_PASSWORD     1
#define     CHECK_PASSWORD     2
#define     UNLOCK_DOOR        3
#define     WRONG_PASSWORD     4
#define     ENTER_ID           5
#define     CHECK_ID           6
#define     INVALID_ID         7
// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char statusOutput[8] = {0,0,0,0,0,0,0,0};
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void OpenOutput(int index);
void CloseOutput(int index);
void TestOutput(void);
void ReverseOutput(int index);
void Test_KeyMatrix();
//Chuong trinh Password Door
#define PASSWORD_LENGTH 6
#define MAX_ACCOUNT     10
#define ERROR_RETURN    0xffff
#define MAX_ID_LENGTH   3
unsigned char arrayMapOfNumber [16] = {1,2,3,'A',4,5,6,'B',
                                       7,8,9,'C','*',0,'E','D'};
unsigned char arrayMapOfPassword [5][PASSWORD_LENGTH]= {
  {1,2,3,4,5,6},
  {2,7,8,9,7,8},
  {3,3,3,3,3,3},
  {4,8,6,8,2,1},
  {5,'A','B','C','D', "F"},
};

typedef struct user_account {
    unsigned int ID;
    unsigned char password[PASSWORD_LENGTH];
} user_account;

user_account account[MAX_ACCOUNT] = {
    {000, {1,2,3,4,5,6}},
    {001, {2,7,8,9,7,8}},
    {002, {3,3,3,3,3,3}},
    {003, {4,8,6,8,2,1}},
    {004, {5,'A','B','C','D', "F"}}
};

unsigned char arrayPassword[PASSWORD_LENGTH];
unsigned int ID_value = 0;

unsigned char statusPassword = INIT_SYSTEM;

unsigned char indexOfNumber = 0;
unsigned char indexOfID = 0;
unsigned char timeDelay = 0;

unsigned int current_user = 0;
unsigned int num_of_user = 5;

void App_PasswordDoor();
unsigned char CheckPassword();
unsigned char isButtonNumber();
unsigned char numberValue;
unsigned char isButtonEnter();
unsigned char isButtonBack();
void UnlockDoor();
void LockDoor();
void displayID(int x, int y, unsigned int value, unsigned char indexOfID);

#define     LIGHT_ON      1
#define     LIGHT_OFF     0
void BaiTap_Den();
unsigned char isButtonLight();
unsigned char statusLight = LIGHT_OFF;
void LightOn();
void LightOff();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	unsigned int k = 0;
	init_system();
        //TestOutput();
	while (1)
	{
            while (!flag_timer3);
            flag_timer3 = 0;
            // thuc hien cac cong viec duoi day
            scan_key_matrix();
            App_PasswordDoor();
            DisplayLcdScreen();
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
        LcdClearS();
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
}

void OpenOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED | arrayMapOfOutput[index];
	}

}

void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED & ~arrayMapOfOutput[index];
	}
}

void ReverseOutput(int index)
{
    if (statusOutput[index]  == ON)
    {
        CloseOutput(index);
        statusOutput[index] = OFF;
    }
    else
    {
        OpenOutput(index);
        statusOutput[index] = ON;
    }
}

void TestOutput(void)
{
	int k;
	for (k=0;k<=7 ;k++ )
	{
		OpenOutput(k);
		delay_ms(500);
		CloseOutput(k);
		delay_ms(500);
	}
}

unsigned int CheckID(unsigned int id) {
    int i = 0;
    for(i = 0; i < num_of_user; i++) {
        if (account[i].ID == id) return i;
    }
    return ERROR_RETURN;
}

void App_PasswordDoor()
{
    switch (statusPassword)
    {
        case INIT_SYSTEM:
            LcdPrintStringS(0,0,"PRESS # FOR PASS");
            LcdPrintStringS(1,0,"                ");
            LockDoor();
            if (isButtonEnter())
            {
                indexOfNumber = 0;
                timeDelay = 0;
                ID_value = 0;
                indexOfID = 0;
                statusPassword = ENTER_ID;
            }
            break;
        case ENTER_ID:
            timeDelay++;
            LcdPrintStringS(0, 0, "ENTER ID        ");
            if (isButtonNumber()) {
                //phim A dung de xoa khi bam sai.
                if (numberValue == 'A') {
                    if (indexOfID > 0) {
                        ID_value = ID_value / 10;
                        indexOfID--;
                        LcdPrintStringS(1,0, "   ");
                    }
                }
                else if (numberValue > 9) {
                    statusPassword = INVALID_ID;
                    timeDelay = 0;
                }
                else {
                    ID_value = ID_value * 10 + numberValue;
                    indexOfID++;
                }
                //everytime we press a button, it mean that we're active, so reset this
                //timeDelay pls
                timeDelay = 0;
            }
            
            displayID(1,0,ID_value,indexOfID);
            LcdPrintStringS(1,3, "             ");
            if (indexOfID >= MAX_ID_LENGTH || isButtonEnter()) {
                indexOfNumber = 0;
                timeDelay = 0;
                statusPassword = CHECK_ID;
            }
            if (isButtonBack()) {
                statusPassword = INIT_SYSTEM;
            }
            if (timeDelay >= 100) {
                statusPassword = INIT_SYSTEM;
            }
            break;
        case CHECK_ID:
            timeDelay = 0;
            current_user = CheckID(ID_value);
            if (current_user != ERROR_RETURN) {
                statusPassword = ENTER_PASSWORD;
                LcdClearS();
            }
            else
                statusPassword = INVALID_ID;
            break;
        case INVALID_ID: 
            timeDelay++;
            LcdPrintStringS(0,0,"Invalid ID      ");
            if (timeDelay >= 40)
                statusPassword = INIT_SYSTEM;
            break;
        case ENTER_PASSWORD:
            LcdPrintStringS(0,0,"PASS FOR ID ");
            LcdPrintNumS(0, 13, current_user);
            timeDelay++;
            if (isButtonNumber())
            {
                if (numberValue == 'A') {
                   if (indexOfNumber > 0) {
                       indexOfNumber--;
                       LcdPrintStringS(1,indexOfNumber," ");
                   }
                }
                else {
                    LcdPrintStringS(1,indexOfNumber,"*");
                    arrayPassword [indexOfNumber] = numberValue;
                    indexOfNumber++;
                }
                timeDelay = 0;
            }
            if (indexOfNumber >= PASSWORD_LENGTH)
                statusPassword = CHECK_PASSWORD;
            if (timeDelay >= 100)
                statusPassword = INIT_SYSTEM;
            if (isButtonBack())
                statusPassword = INIT_SYSTEM;
            break;
        case CHECK_PASSWORD:
            
            timeDelay = 0;
            if (CheckPassword())
                statusPassword = UNLOCK_DOOR;
            else
                statusPassword = WRONG_PASSWORD;
            break;
        case UNLOCK_DOOR:
            timeDelay++;
            LcdPrintStringS(0,0,"OPENING DOOR    ");
            UnlockDoor();
            if (timeDelay >= 100)
                statusPassword = INIT_SYSTEM;
            break;
        case WRONG_PASSWORD:
            timeDelay++;
            LcdPrintStringS(0,0,"PASSWORD WRONG  ");
            if (timeDelay >= 40)
                statusPassword = INIT_SYSTEM;
            break;
        default:
            break;

    }
}
unsigned char CheckPassword()
{
    unsigned char i,j;
    for (i = 0; i < PASSWORD_LENGTH; i++) {
        if (account[current_user].password[i] != arrayPassword[i]) return 0;
    }
    return 1;

}
unsigned char isButtonNumber()
{
    unsigned char i;
    for (i = 0; i<=15; i++)
        if (key_code[i] == 1)
        {
            numberValue = arrayMapOfNumber[i];
            return 1;
        }
    return 0;
}

unsigned char isButtonEnter()
{
    if (key_code[14] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonBack()
{
    if (key_code[12] == 1)
        return 1;
    else
        return 0;
}
void UnlockDoor()
{
    OpenOutput(0);
    //OpenOutput(4);
}
void LockDoor()
{
    CloseOutput(0);
    //CloseOutput(4);
}

void displayID(int x, int y, unsigned int value, unsigned char indexOfID) {
    if (value < 10 && indexOfID == 3) {
        LcdPrintStringS(x,y,"00");
        LcdPrintNumS(x,y+2,value);
    } else if (value < 10 && indexOfID == 2) {
        LcdPrintStringS(x,y,"0");
        LcdPrintNumS(x,y+1,value);
    } else if (value < 100 && indexOfID == 3) {
        LcdPrintStringS(x,y,"0");
        LcdPrintNumS(x,y+1,value);
    } else {
        LcdPrintNumS(x,y,value);
    }
}
