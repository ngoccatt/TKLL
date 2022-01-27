
#include "traffic_light.h"
#include "lcd\lcd.h"
#include "button_matrix\button.h"
#include "display_7seg_spi.h"

unsigned char arrayMapOfOutput [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char statusOutput[8] = {0,0,0,0,0,0,0,0};

#define     ON_T      1
#define     OFF_T     0
    
#define PORTD_OUT_T   PORTD
#define TRISD_OUT_T   TRISD


void OpenOutput(int index);
void CloseOutput(int index);
void TestOutput(void);
void ReverseOutput(int index);



void OpenOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		PORTD_OUT_T = PORTD_OUT_T | arrayMapOfOutput[index];
	}
}

void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		PORTD_OUT_T = PORTD_OUT_T & ~arrayMapOfOutput[index];
	}
}


void ReverseOutput(int index)
{
    if (statusOutput[index]  == ON_T)
    {
        CloseOutput(index);
        statusOutput[index] = OFF_T;
    }
    else
    {
        OpenOutput(index);
        statusOutput[index] = ON_T;
    }
}

void TestOutput(void)
{
	int k;
	for (k=0;k<14 ;k++ )
	{
		OpenOutput(k);
		delay_ms(500);
		CloseOutput(k);
		delay_ms(500);
	}
}

// main fsm traffic light:
#define IDLE                0  
#define FUNCTION_SELECT     1
#define FUNCTION_MODE       2

unsigned char traffic_light_state = IDLE;
int time_delay = 0;

int isButtonEnter();
int isButtonEnterHold3Sec();
int isButtonInc();
int isButtonIncHold();
int isButtonDec();
int isButtonDecHold();
int isButtonPress();

//display traffic light

#define     INIT_SYSTEM         255
#define     PHASE1_GREEN        0
#define     PHASE1_YELLOW       1
#define     PHASE2_GREEN        2
#define     PHASE2_YELLOW       3
#define     WAIT                4

unsigned char statusOfLight = PHASE1_GREEN;
unsigned char timeOfGreenPhase1 = 25;
unsigned char timeOfYellowPhase1 = 3;
unsigned char timeOfGreenPhase2 = 20;
unsigned char timeOfYellowPhase2 = 3;
unsigned char timeOfLight = 0;
unsigned char cntOfLight = 0;
unsigned char cntOfSlow = 0;
unsigned char tp;
unsigned char k;
void lightDisplay();
void lightDisplayNoLCD();
void traffic_light_fsm();
void yellow_light_slow_mode();
void UartDataReceiveProcess();
void UartDataReceiveProcess_ElectronicDeviceControl();
void onlyRed();
void onlyYellow();
void onlyGreen();

void Phase1_Green(char on_off);
void Phase1_Yellow(char on_off);
void Phase1_Red(char on_off);


void Phase2_Green(char on_off);
void Phase2_Yellow(char on_off);
void Phase2_Red(char on_off);

#define NUM_OF_FUNCTION 5

#define CHANGE_TRAFFIC_LIGHT_TIME   0
#define SLOW_MODE                   1
#define MANUAL_SELECTION            2
#define FUNCTION_0                  3
#define FUNCTION_1                  4
#define MANUAL_MODE                 5

int function_selection = 0;
unsigned char cursor_line = 0;
int timeof = 0;
unsigned char slow_mode_enable = 0;
int blinking = 0;
enum Manual_mode {MANUAL_RED, MANUAL_GREEN, MANUAL_YELLOW};
enum Manual_mode manual_mode_val = MANUAL_RED;

void display_function(int function, unsigned char lcd_line);
void change_time_val(int timeOf);

void function_fsm(int* function_selection);


unsigned char function_array[NUM_OF_FUNCTION] = {
    CHANGE_TRAFFIC_LIGHT_TIME, SLOW_MODE, MANUAL_SELECTION,
    FUNCTION_0, FUNCTION_1
};


int isButtonEnter() {
    if (key_code[4] == 1) return 1;
    return 0;
}

int isButtonEnterHold3Sec() {
    if (key_code[4] > 60) return 1;
    return 0;
}

int isButtonInc() {
    if(key_code[5] == 1) return 1;
    return 0;
}

int isButtonIncHold() {
    if(key_code[5] > 20 && key_code[5] % 4 == 0) return 1;
    return 0;
}

int isButtonDec() {
    if (key_code[6] == 1) return 1;
    return 0;
}

int isButtonDecHold() {
    if (key_code[6] > 20 && key_code[6] % 4 == 0) return 1;
    return 0;
}

int isButtonPress() {
    if (key_code[4] == 1 || key_code[5] == 1 || key_code[6] == 1) return 1;
    return 0;
}


void Phase1_Green(char on_off)
{
    if (on_off == 1) {
        OpenOutput(4);
    } else {
        CloseOutput(4);
    }
}


void Phase1_Yellow(char on_off)
{
    if (on_off == 1) {
        OpenOutput(2);
    } else {
        CloseOutput(2);
    }
}


void Phase1_Red(char on_off)
{
    if (on_off == 1) {
        OpenOutput(0);
    } else {
        CloseOutput(0);
    }
}


void Phase2_Green(char on_off)
{
    if (on_off == 1) {
        OpenOutput(5);
    } else {
        CloseOutput(5);
    }
}


void Phase2_Yellow(char on_off)
{
    if (on_off == 1) {
        OpenOutput(3);
    } else {
        CloseOutput(3);
    }
}


void Phase2_Red(char on_off)
{
    if (on_off == 1) {
        OpenOutput(1);
    } else {
        CloseOutput(1);
    }
}

void display_function(int function, unsigned char lcd_line) {
    
    switch(function) {
        case CHANGE_TRAFFIC_LIGHT_TIME:
            LcdPrintLineS(lcd_line, "1.CHANGE TIME");
        break;
        case SLOW_MODE:
            LcdPrintLineS(lcd_line, "2.SLOW MODE");
        break;
        case MANUAL_SELECTION:
            LcdPrintLineS(lcd_line, "3.MANUAL MODE");
        break;
        case FUNCTION_0:
            LcdPrintLineS(lcd_line, "4.FUNCTION_0");
        break;
        case FUNCTION_1:
            LcdPrintLineS(lcd_line, "5.FUNCTION_1");
        break;
    }
}

void change_time_val(int timeOf) {
    blinking = (blinking + 1) % 40;
    switch(timeOf) {
        case 0:
            blinking = 0;
        break;
        case 1:
            if (blinking < 10 && key_code[5] == 0 && key_code[6] == 0) {
                LcdPrintStringS(0, 5, "   ");
            } else {
                LcdPrintNumS(0, 5, timeOfGreenPhase1);
            }
            
            if ((isButtonInc() || isButtonIncHold()) && timeOfGreenPhase1 < 99) 
                timeOfGreenPhase1++;
            if ((isButtonDec() || isButtonDecHold()) && timeOfGreenPhase1 > 0) 
                timeOfGreenPhase1--;
        break;
        case 2:
            if (blinking < 10 && key_code[5] == 0 && key_code[6] == 0) {
                LcdPrintStringS(0, 13, "   ");
            } else {
                LcdPrintNumS(0, 13, timeOfYellowPhase1);
            }
            if ((isButtonInc() || isButtonIncHold()) && timeOfYellowPhase1 < 99) 
                timeOfYellowPhase1++;
            if ((isButtonDec() || isButtonDecHold()) && timeOfYellowPhase1 > 0) 
                timeOfYellowPhase1--;
        break;
        case 3:
            if (blinking < 10 && key_code[5] == 0 && key_code[6] == 0) {
                LcdPrintStringS(1, 5, "   ");
            } else {
                LcdPrintNumS(1, 5, timeOfGreenPhase2);
            }
            if ((isButtonInc() || isButtonIncHold()) && timeOfGreenPhase2 < 99) 
                timeOfGreenPhase2++;
            if ((isButtonDec() || isButtonDecHold()) && timeOfGreenPhase2 > 0) 
                timeOfGreenPhase2--;
        break;
        case 4:
            if (blinking < 10 && key_code[5] == 0 && key_code[6] == 0) {
                LcdPrintStringS(1, 13, "   ");
            } else {
                LcdPrintNumS(1, 13, timeOfYellowPhase2);
            }
            if ((isButtonInc() || isButtonIncHold()) && timeOfYellowPhase2 < 99) 
                timeOfYellowPhase2++;
            if ((isButtonDec() || isButtonDecHold()) && timeOfYellowPhase2 > 0) 
                timeOfYellowPhase2--;
        break;
    }
}

void function_fsm(int *function_selection) {
    switch(*function_selection) {
        case CHANGE_TRAFFIC_LIGHT_TIME:
            LcdPrintStringS(0, 0, "GRN1:   ");
            LcdPrintNumS(0, 5, timeOfGreenPhase1);
            LcdPrintStringS(0, 8, "YLW1:   ");
            LcdPrintNumS(0, 13, timeOfYellowPhase1);
            LcdPrintStringS(1, 0, "GRN2:   ");
            LcdPrintNumS(1, 5, timeOfGreenPhase2);
            LcdPrintStringS(1, 8, "YLW2:   ");
            LcdPrintNumS(1, 13, timeOfYellowPhase2);
            
            if (isButtonEnter()) {
                timeof = (timeof + 1) % 5;
            }
            
            change_time_val(timeof);
//            notice me senpai
            lightDisplayNoLCD();
        break;
        case SLOW_MODE:
            LcdPrintLineS(0, "    SLOW MODE");
            if (slow_mode_enable) {
                //some code to flash the yellow led
                yellow_light_slow_mode();
                LcdPrintLineS(1, "       ON");
            } else {
//                neu khong, chay led binh thuong.
                lightDisplayNoLCD();
                LcdPrintLineS(1, "       OFF");
            }
            
            if (isButtonDec()) {
                slow_mode_enable = (slow_mode_enable + 1) % 2;
            }
        break;
        case MANUAL_SELECTION:
            LcdPrintLineS(0, "   MANUAL MODE");
            
            switch(manual_mode_val) {
                case MANUAL_RED:
                    LcdPrintLineS(1, "RED");
                    break;
                case MANUAL_GREEN:
                    LcdPrintLineS(1, "GREEN");
                    break;
                case MANUAL_YELLOW:
                    LcdPrintLineS(1, "YELLOW");
                    break;
            }
            if (isButtonInc()) {
                manual_mode_val = (manual_mode_val + 1) % 3;
            }
            LcdPrintStringS(1, 10, "OFF");
            if (isButtonDec()) {
                *function_selection = MANUAL_MODE;
                turnOffAll7Seg();
            }
            lightDisplayNoLCD();
        break;
        case MANUAL_MODE:
            LcdPrintStringS(1, 10, "ON     ");
            switch(manual_mode_val) {
                case MANUAL_RED:
                    //code to make the led red light only
                    onlyRed();
                    break;
                case MANUAL_GREEN:
                    //code to make the led green light only
                    onlyGreen();
                    break;
                case MANUAL_YELLOW:
                    //code to make the led yewllo light only
                    onlyYellow();
                    break;
            }
            if (isButtonDec()) {
                LcdClearS();
                *function_selection = MANUAL_SELECTION;
            }
        break;
        case FUNCTION_0:
            LcdPrintLineS(1, "4.FUNCTION_0");
        break;
        case FUNCTION_1:
            LcdPrintLineS(1, "5.FUNCTION_1");
        break;
        default:
            LcdPrintStringS(1,0,"wrong");
            LcdPrintNumS(1, 12, *function_selection);
            break;
    }
}


void traffic_light_fsm() {
    switch(traffic_light_state) {
        case IDLE:
            lightDisplay();
            if (isButtonEnter()) {
                LcdClearS();
                traffic_light_state = FUNCTION_SELECT;
                function_selection = 0;
                cursor_line = 0;
                display_function(0, 0);
                display_function(1, 1);
                time_delay = 0;
            }
        break;
        case FUNCTION_SELECT:
            time_delay++;
            
            LcdPrintCharS(cursor_line, 15, LEFT_ARROW);
           
            
            if (isButtonDec() && (function_selection < NUM_OF_FUNCTION - 1)) {
                function_selection++;
                display_function(function_selection - 1, 0);
                display_function(function_selection, 1);
                if (cursor_line < 1) cursor_line++;
                time_delay = 0;
            }
            
            if (isButtonInc() && (function_selection > 0)) {
                function_selection--;
                display_function(function_selection, 0);
                display_function(function_selection + 1, 1);
                if (cursor_line > 0) cursor_line--;
                time_delay = 0;
            }
            
            if (isButtonEnter()) {
                LcdClearS();
                time_delay = 0;
                timeof = 0;
                slow_mode_enable = 0;
                cntOfSlow = 0;
                manual_mode_val = MANUAL_RED;
                traffic_light_state = FUNCTION_MODE;
            }
            
            if (time_delay > 300) {
                traffic_light_state = IDLE;
                LcdClearS();
            }
            
            lightDisplayNoLCD();
        break;
        case FUNCTION_MODE:
            
            
            function_fsm(&function_selection);
            
            
            if (isButtonEnterHold3Sec()) {
                LcdClearS();
                traffic_light_state = FUNCTION_SELECT;
                function_selection = 0;
                cursor_line = 0;
                display_function(0, 0);
                display_function(1, 1);
                time_delay = 0;
            }
        break;
    }
}



void lightDisplay()
{
    cntOfLight = (cntOfLight + 1)%20;
    if (cntOfLight == 0)
        timeOfLight --;
//    LcdPrintStringS(0,1,"LINE 1");
//    LcdPrintStringS(0,9,"LINE 2");
    switch (statusOfLight)
    {
        case PHASE1_GREEN:
            Phase1_Green(ON_T);
            Phase1_Yellow(OFF_T);
            Phase1_Red(OFF_T);
            Phase2_Green(OFF_T);
            Phase2_Yellow(OFF_T);
            Phase2_Red(ON_T);
            
            LcdPrintStringS(1,0,"GRN     ");
            LcdPrintNumS(1,4,timeOfLight);
            LcdPrintStringS(1,8,"RED     ");
            LcdPrintNumS(1,12,timeOfLight + timeOfYellowPhase1);

            if (timeOfLight == 0)
            {
                statusOfLight = PHASE1_YELLOW;
                timeOfLight = timeOfYellowPhase1;
            }
            updateLedBuffer_SPI(timeOfLight, timeOfLight + timeOfYellowPhase1);
            break;
        case PHASE1_YELLOW:
            Phase1_Green(OFF_T);
            Phase1_Yellow(ON_T);
            Phase1_Red(OFF_T);
            Phase2_Green(OFF_T);
            Phase2_Yellow(OFF_T);
            Phase2_Red(ON_T);
            
            
            LcdPrintStringS(1,0,"YLW     ");
            LcdPrintNumS(1,4,timeOfLight);
            LcdPrintStringS(1,8,"RED     ");
            LcdPrintNumS(1,12,timeOfLight);

            if (timeOfLight == 0)
            {
                statusOfLight = PHASE2_GREEN;
                timeOfLight = timeOfGreenPhase2;
            }
            updateLedBuffer_SPI(timeOfLight, timeOfLight);
            break;
        case PHASE2_GREEN:
            Phase1_Green(OFF_T);
            Phase1_Yellow(OFF_T);
            Phase1_Red(ON_T);
            Phase2_Green(ON_T);
            Phase2_Yellow(OFF_T);
            Phase2_Red(OFF_T);
            
            LcdPrintStringS(1,0,"RED     ");
            LcdPrintNumS(1,4,timeOfLight + timeOfYellowPhase2);
            LcdPrintStringS(1,8,"GRN     ");
            LcdPrintNumS(1,12,timeOfLight);

            if (timeOfLight == 0)
            {
                statusOfLight = PHASE2_YELLOW;
                timeOfLight = timeOfYellowPhase2;
            }
            updateLedBuffer_SPI(timeOfLight + timeOfYellowPhase2, timeOfLight);
            break;
        case PHASE2_YELLOW:
            Phase1_Green(OFF_T);
            Phase1_Yellow(OFF_T);
            Phase1_Red(ON_T);
            Phase2_Green(OFF_T);
            Phase2_Yellow(ON_T);
            Phase2_Red(OFF_T);
            
            
            LcdPrintStringS(1,0,"RED     ");
            LcdPrintNumS(1,4,timeOfLight);
            LcdPrintStringS(1,8,"YLW     ");
            LcdPrintNumS(1,12,timeOfLight);

            if (timeOfLight == 0)
            {
                statusOfLight = PHASE1_GREEN;
                timeOfLight = timeOfGreenPhase1;
            }
            updateLedBuffer_SPI(timeOfLight, timeOfLight);
            break;
//        case WAIT:
//            Phase1_GreenOff();
//            Phase2_GreenOff();
//            Phase1_RedOff();
//            Phase2_RedOff();
//            if (cntOfLight<=12)
//            {
//                Phase1_YellowOn();
//                Phase2_YellowOn();
//            }
//            else
//            {
//                Phase1_YellowOff();
//                Phase2_YellowOff();
//            }
//            break;
        default:
            statusOfLight = PHASE1_GREEN;
            break;
    }
//    notice me senpai
    scan7Seg_SPI();
}


void lightDisplayNoLCD()
{
    cntOfLight = (cntOfLight + 1)%20;
    if (cntOfLight == 0)
        timeOfLight --;
//    LcdPrintStringS(0,1,"LINE 1");
//    LcdPrintStringS(0,9,"LINE 2");
    switch (statusOfLight)
    {
        case PHASE1_GREEN:
            Phase1_Green(ON_T);
            Phase1_Yellow(OFF_T);
            Phase1_Red(OFF_T);
            Phase2_Green(OFF_T);
            Phase2_Yellow(OFF_T);
            Phase2_Red(ON_T);

            if (timeOfLight == 0)
            {
                statusOfLight = PHASE1_YELLOW;
                timeOfLight = timeOfYellowPhase1;
            }
            updateLedBuffer_SPI(timeOfLight, timeOfLight + timeOfYellowPhase1);
            break;
        case PHASE1_YELLOW:
            Phase1_Green(OFF_T);
            Phase1_Yellow(ON_T);
            Phase1_Red(OFF_T);
            Phase2_Green(OFF_T);
            Phase2_Yellow(OFF_T);
            Phase2_Red(ON_T);
            
            if (timeOfLight == 0)
            {
                statusOfLight = PHASE2_GREEN;
                timeOfLight = timeOfGreenPhase2;
            }
            updateLedBuffer_SPI(timeOfLight, timeOfLight);
            break;
        case PHASE2_GREEN:
            Phase1_Green(OFF_T);
            Phase1_Yellow(OFF_T);
            Phase1_Red(ON_T);
            Phase2_Green(ON_T);
            Phase2_Yellow(OFF_T);
            Phase2_Red(OFF_T);
            
            if (timeOfLight == 0)
            {
                statusOfLight = PHASE2_YELLOW;
                timeOfLight = timeOfYellowPhase2;
            }
            updateLedBuffer_SPI(timeOfLight + timeOfYellowPhase2, timeOfLight);
            break;
        case PHASE2_YELLOW:
            Phase1_Green(OFF_T);
            Phase1_Yellow(OFF_T);
            Phase1_Red(ON_T);
            Phase2_Green(OFF_T);
            Phase2_Yellow(ON_T);
            Phase2_Red(OFF_T);
            
            if (timeOfLight == 0)
            {
                statusOfLight = PHASE1_GREEN;
                timeOfLight = timeOfGreenPhase1;
            }
            updateLedBuffer_SPI(timeOfLight, timeOfLight);
            break;
        default:
            statusOfLight = PHASE1_GREEN;
            break;
    }
//    notice me senpai
    scan7Seg_SPI();
}

void yellow_light_slow_mode() {
    cntOfSlow = (cntOfSlow + 1) % 40;
    if (cntOfSlow == 0) {
        turnOffAll7Seg();
        Phase1_Red(0);
        Phase1_Green(0);
        Phase2_Red(0);
        Phase2_Green(0);
    }
    
    if (cntOfSlow < 20) {
        Phase1_Yellow(1);
        Phase2_Yellow(1);
    } else {
        Phase1_Yellow(0);
        Phase2_Yellow(0);
    } 
}

void onlyRed() {
    Phase1_Red(1);
    Phase1_Green(0);
    Phase1_Yellow(0);
    Phase2_Red(1);
    Phase2_Green(0); 
    Phase2_Yellow(0);
}

void onlyYellow() {
    Phase1_Red(0);
    Phase1_Green(0);
    Phase1_Yellow(1);
    Phase2_Red(0);
    Phase2_Green(0); 
    Phase2_Yellow(1);
}

void onlyGreen() {
    Phase1_Red(0);
    Phase1_Green(1);
    Phase1_Yellow(0);
    Phase2_Red(0);
    Phase2_Green(1); 
    Phase2_Yellow(0);
}


//void UartDataReceiveProcess()
//{
//    if(flagOfDataReceiveComplete == 1)
//    {
//        flagOfDataReceiveComplete = 0;
//        if (dataReceive[0] == 0)
//        {
//            timeOfGreenPhase1 = dataReceive[1];
//            timeOfYellowPhase1 = dataReceive[2];
//            timeOfGreenPhase2 = dataReceive[3];
//            timeOfYellowPhase2 = dataReceive[4];
//        }
//    }
//    LcdPrintNumS(0,15,statusReceive+4);
//}
