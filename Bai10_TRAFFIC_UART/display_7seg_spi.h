/* 
 * File:   display_7seg_spi.h
 * Author: ngocc
 *
 * Created on January 23, 2022, 6:29 PM
 */

#ifndef DISPLAY_7SEG_SPI_H
#define	DISPLAY_7SEG_SPI_H

#ifdef	__cplusplus
extern "C" {
#endif


#include <p18f4620.h>

#define NUM_OF_LED 4

void display7Seg_SPI(int value, int led_index);
void scan7Seg_SPI();
void turnOffAll7Seg();
void updateLedBuffer_SPI(int value1, int value2);

#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_7SEG_SPI_H */

