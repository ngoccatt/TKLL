/* 
 * File:   simu_clock.h
 * Author: ngocc
 *
 * Created on December 13, 2021, 9:21 PM
 */

#ifndef SIMU_CLOCK_H
#define	SIMU_CLOCK_H

//CLOCK SECTION

#define INIT_CLOCK      1
#define GET_TIME        2
#define CHANGE_HOUR     3
#define CHANGE_MINUTE   4
#define CHANGE_SECOND   5
#define SET_TIME        6


extern unsigned char clockState;

void SetupTimeForRealTime();
void SetupForFirstProgram(void);
void DisplayRealTime();

void DisplayTempTime(unsigned char hour, unsigned char minute, unsigned char second);
void smolClock();
void smolLateClock();
unsigned char isButtonIncrease();
unsigned char isButtonDecrease();
unsigned char isButtonMode();
void clock_run();


extern unsigned char second;
extern unsigned char minute;
extern unsigned char hour;

extern unsigned char temp_hour;
extern unsigned char temp_minute;
extern unsigned char temp_second;

extern unsigned char late_hour;
extern unsigned char late_minute;
extern unsigned char late_second;

#endif	/* SIMU_CLOCK_H */

