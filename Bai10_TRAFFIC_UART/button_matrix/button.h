#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <p18f4620.h>

#define PORT_BUTTON		PORTC
#define TRIS_BUTTON		TRISC
#define	MAX_COL			4
#define MAX_ROW			4

extern unsigned int key_code[16];



void init_key_matrix();
void scan_key_matrix();
void scan_key_matrix_1_line();
void scan_key_matrix_demo();
void button_process();
void scan_key_matrix_with_uart();
void scan_key_matrix_with_uart_i2c();
void init_key_matrix_new_board();
void scan_key_matrix_new_board();
#endif