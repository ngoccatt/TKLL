/* 
 * File:   spi_stimulate.h
 * Author: ngocc
 *
 * Created on January 23, 2022, 5:04 PM
 */

#ifndef SPI_STIMULATE_H
#define	SPI_STIMULATE_H

#ifdef	__cplusplus
extern "C" {
#endif


#include <p18f4620.h>

#define DATA_PIN    PORTDbits.RD6
#define SHIFT_CLOCK PORTDbits.RD7
#define LATCH       PORTAbits.RA2
#define TRIS_DATA_D   TRISD
#define TRIS_LATCH_A  TRISA

#define UP          1
#define DOWN        0


void init_SPI_manual();

//this function tranfer data to the shift register (not yet print to output)
void transfer_data_SPI(unsigned char byte);

void data_to_output_SPI();


#ifdef	__cplusplus
}
#endif

#endif	/* SPI_STIMULATE_H */

