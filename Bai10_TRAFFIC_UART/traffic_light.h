/* 
 * File:   traffic_light.h
 * Author: ngocc
 *
 * Created on January 16, 2022, 11:39 AM
 */

#ifndef TRAFFIC_LIGHT_H
#define	TRAFFIC_LIGHT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <p18f4620.h>
    
void lightDisplay();
void lightDisplayNoLCD();
void traffic_light_fsm();

#ifdef	__cplusplus
}
#endif

#endif	/* TRAFFIC_LIGHT_H */

