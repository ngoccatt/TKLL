#include "button.h"

unsigned int key_code[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//unsigned char arrayMaskOutputOfKey [8] = {0x80,0x40,0x20,0x10,0x01,0x02,0x04,0x08};
//unsigned char arrayMaskInputOfKey [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char arrayMaskOutputOfKey [4] = {0x10,0x20,0x40,0x80};
unsigned char arrayMaskInputOfKey [4] = {0x1,0x02,0x04,0x08};

void init_key_matrix()
{
	TRIS_BUTTON = 0x0f; 
	PORT_BUTTON = 0xff;
}
void init_key_matrix_with_uart_i2c()
{
        TRIS_BUTTON = TRIS_BUTTON | 0x07;
        TRIS_BUTTON = TRIS_BUTTON & 0b11011111; //RC5 Output
	PORT_BUTTON = 0xff;
}
void scan_key_matrix()
{
	int i,j;
	for(i=0;i<MAX_ROW;i++)     
	{
		PORT_BUTTON = ~arrayMaskOutputOfKey[i];
		for(j=0;j<MAX_COL;j++)
		{ 
			if((PORT_BUTTON & arrayMaskInputOfKey[j]) == 0)  
				key_code[i*MAX_ROW+j] = key_code[i*MAX_ROW+j] + 1;
			else
				key_code[i*MAX_ROW+j] = 0;   
		}
	}
}

void scan_key_matrix_with_uart()
{
	int i,j;
	for(i=0;i<2;i++)
	{
		PORT_BUTTON = PORT_BUTTON & ~arrayMaskOutputOfKey[i];
		for(j=0;j<MAX_COL;j++)
		{
			if((PORT_BUTTON & arrayMaskInputOfKey[j]) == 0)
				key_code[i*MAX_ROW+j] = key_code[i*MAX_ROW+j] + 1;
			else
				key_code[i*MAX_ROW+j] = 0;
		}
	}
}

void scan_key_matrix_1_line()
{
	int i=0,j=0;
	//for(i=3;i<MAX_ROW;i++)
	{
		PORT_BUTTON = PORT_BUTTON & ~arrayMaskOutputOfKey[0];
		for(j=0;j<MAX_COL;j++)
		{
			if((PORT_BUTTON & arrayMaskInputOfKey[j]) == 0)
				key_code[i*MAX_ROW+j] = key_code[i*MAX_ROW+j] + 1;
			else
				key_code[i*MAX_ROW+j] = 0;
		}
	}
}

void scan_key_matrix_with_uart_i2c()
{
	int i=0,j=0;
        i=1;
	{
		PORT_BUTTON = PORT_BUTTON & ~arrayMaskOutputOfKey[i];
		for(j=0;j<3;j++)
		{
			if((PORT_BUTTON & arrayMaskInputOfKey[j]) == 0)
				key_code[i*MAX_ROW+j] = key_code[i*MAX_ROW+j] + 1;
			else
				key_code[i*MAX_ROW+j] = 0;
		}
	}
}

//ham nay de giup cac ban hieu ro viec quet ma tran phim
//Sau khi da hieu ve cach quet ma tran phim thi xoa di, hoac luu lai o mot file khac
void button_delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void scan_key_matrix_demo() 
{
	int i,j;
	for(i=0;i<MAX_ROW;i++)     
	{
		PORT_BUTTON = ~arrayMaskOutputOfKey[i];
		for(j=0;j<MAX_COL;j++)
		{ 
			key_code[i*MAX_ROW+j] = 0;   
			if((PORT_BUTTON & arrayMaskInputOfKey[j]) == 0)  
			{
				key_code[i*MAX_ROW+j] = 1;
			}
		}
		PORTB = PORT_BUTTON;
		button_delay_ms(1000);
	}
}

void scan_key_matrix_new_board()
{
	int i,j;
    if(PORTEbits.RE0 == 0)
        key_code[0] ++;
    else
        key_code[0] = 0;
    if(PORTEbits.RE1 == 0)
        key_code[1] ++;
    else
        key_code[1] = 0;
    if(PORTEbits.RE2 == 0)
        key_code[2] ++;
    else
        key_code[2] = 0;
	if(PORTCbits.RC0 == 0)
        key_code[3] ++;
    else
        key_code[3] = 0;
    if(PORTCbits.RC1 == 0)
        key_code[4] ++;
    else
        key_code[4] = 0;
    if(PORTCbits.RC2 == 0)
        key_code[5] ++;
    else
        key_code[5] = 0;
    if(PORTCbits.RC5 == 0)
        key_code[6] ++;
    else
        key_code[6] = 0;
    if(PORTBbits.RB0 == 0)
        key_code[7] ++;
    else
        key_code[7] = 0;
}

void init_key_matrix_new_board()
{
    TRISE = TRISE | 0x07;
    TRISC = TRISC | 0x27;
    TRISB = TRISB | 0x01;
}