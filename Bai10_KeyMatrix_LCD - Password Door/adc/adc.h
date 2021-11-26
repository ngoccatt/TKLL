/* 
 * File:   adc.h
 * Author: ngocc
 *
 * Created on November 26, 2021, 4:00 PM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <p18f4620.h>

#define ADC_CHANNEL 	13

void init_adc(void);
int get_adc_value();

extern unsigned int adc_value[ADC_CHANNEL];


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

