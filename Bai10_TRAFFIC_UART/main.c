#include "main.h"

// Noi khai bao hang so
#define     LED     PORTD
#define     ON      1
#define     OFF     0
    
#define PORTD_OUT   PORTD
#define TRISD_OUT   TRISD

#define     INIT_SYSTEM        255
#define     ENTER_PASSWORD     1
#define     CHECK_PASSWORD     2
#define     UNLOCK_DOOR        3
#define     WRONG_PASSWORD     4



void init_output(void);
// Noi khai bao bien toan cuc
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);


void Test_KeyMatrix();
//Chuong trinh Password Door
//unsigned char arrayMapOfNumber [16] = {1,2,3,'A',4,5,6,'B',
//                                       7,8,9,'C','*',0,'E','D'};
//unsigned char arrayMapOfPassword [5][4]= {
//  {1,2,3,4},
//  {2,7,8,9},
//  {3,3,3,3},
//  {4,8,6,8},
//  {5,'A','B','C'},
//};
//unsigned char arrayPassword[4];s
//unsigned char statusPassword = INIT_SYSTEM;
//unsigned char indexOfNumber = 0;
//unsigned char timeDelay = 0;
//
//void App_PasswordDoor();
//unsigned char CheckPassword();
//unsigned char isButtonNumber();
//unsigned char numberValue;
//unsigned char isButtonEnter();
//void UnlockDoor();
//void LockDoor();


// Den giao thong
//void traffic_light_fsm();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	unsigned int k = 0;
	init_system();
        delay_ms(1000);
//        transfer_data_SPI(0b10011010);
//        transfer_data_SPI(0b11110000);
//        data_to_output_SPI();
//        display7Seg_SPI(7, 0);
	while (1)
	{
            while (!flag_timer3);
            flag_timer3 = 0;
            scan_key_matrix_with_uart_i2c(); // 8 button
//              scan_key_matrix();
//              DisplayDataReceive();
//              UartDataReceiveProcess();
            traffic_light_fsm();
//              Test_KeyMatrix();
            DisplayLcdScreen(); //Output process 14ms vs 10ms with no timer
            
            
	}
}
// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void init_output(void)
{
    TRISD_OUT = 0x00;
    PORTD_OUT = 0x00;
}

void init_system(void)
{
    init_output();
    init_SPI_manual();
    lcd_init();
    init_user_defined_char();
    init_key_matrix_with_uart_i2c();
    init_interrupt();
    init_i2c();
    init_uart();
    lcd_clear();
    LcdClearS();
    delay_ms(500);
    init_timer0(4695);//dinh thoi 1ms sai so 1%
    init_timer3(46950);//dinh thoi 10ms
    SetTimer0_ms(2);
    SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
    //PORTAbits.RA0 = 1;
}


void Test_KeyMatrix() {
    int i = 0;
    for (i = 0; i < 16; i++) {
        if (key_code[i]) {
            LcdPrintCharS(0, i, 'a');
        } else {
            LcdPrintCharS(0, i, ' ');
        }
    }
}


//unsigned char isButtonNumber()
//{
//    unsigned char i;
//    for (i = 0; i<=15; i++)
//        if (key_code[i] == 10)
//        {
//            numberValue = arrayMapOfNumber[i];
//            return 1;
//        }
//    return 0;
//}
//
//unsigned char isButtonEnter()
//{
//    if (key_code[14] == 10)
//        return 1;
//    else
//        return 0;
//}
//void UnlockDoor()
//{
//    OpenOutput(0);
//}
//void LockDoor()
//{
//    CloseOutput(0);
//}





