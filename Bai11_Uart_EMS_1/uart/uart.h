#ifndef _UART_H_
#define _UART_H_

#include <p18f4620.h>

extern unsigned char dataReceive;

void init_uart();
void uart_putchar(unsigned char data);
void uart_send_str(const char *str);
void UartSendString(const rom char *str);
void UartSendNum(long num);
void UartSendNumPercent(int num); //dinh dang so ab.cd
void uart_isr();

#endif