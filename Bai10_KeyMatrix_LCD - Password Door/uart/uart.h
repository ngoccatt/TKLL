/* 
 * File:   uart.h
 * Author: ngocc
 *
 * Created on November 26, 2021, 3:57 PM
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <p18f4620.h>

extern unsigned char dataReceive;

void init_uart();
void disable_uart();
void uart_putchar(unsigned char data);
void uart_send_str(const char *str);
void UartSendString(const rom char *str);
void UartSendNum(long num);
void UartSendNumPercent(int num); //dinh dang so ab.cd
void uart_isr();


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

