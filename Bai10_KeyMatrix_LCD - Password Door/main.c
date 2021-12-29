#include "main.h"
#include <ctype.h>
#include <string.h>
// Noi khai bao hang so



// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char statusOutput[8] = {0,0,0,0,0,0,0,0};
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);

void OpenOutput(int index);

void CloseOutput(int index);

void ReverseOutput(int index);

void Test_KeyMatrix();

////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	unsigned int k = 0;
	init_system();
    SetupForFirstProgram();
    delay_ms(1000);
	while (1)
	{
            while (!flag_timer3);
            flag_timer3 = 0;
//            uncomment 2 dong nay de su dung simulator
//            k = (k + 1) % 20;
//            if (k == 0) clock_run();
            
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
        TRISD = 0x00;       //PORTD is output, too
        init_lcd();
        LcdClearS();
        LED = 0x00;
        init_interrupt();
        delay_ms(1000);
        init_timer0(4695);//dinh thoi 1ms sai so 1%
        init_timer1(9390);//dinh thoi 2ms
        init_timer3(46950);//dinh thoi 10ms
        SetTimer0_ms(2);
        SetTimer1_ms(10);
        SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
        init_key_matrix();
//        disable_uart();
//        init_adc();
        
        init_i2c();     // disable this if you simulator:
        init_user_defined_char();
        

        
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








