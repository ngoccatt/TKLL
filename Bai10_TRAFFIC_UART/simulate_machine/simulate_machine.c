#include "simulate_machine.h"
#include "..\button_matrix\button.h"
#include "..\lcd\lcd.h"

#define INIT_RECEIVE		0
#define WAIT_HEADER_1		1
#define WAIT_HEADER_2		2
#define RECEIVE_DATA		3
#define END_OF_RECEIVE_1	4
#define END_OF_RECEIVE_2	5
#define     INIT_SYSTEM     0
#define     CALL            1
#define     MESSAGE         2
#define     WAITING         3
#define     MESSAGE_GPRS         4
#define     WAITING_GPRS         5
// Noi khai bao bien toan cuc
unsigned char statusSim900 = INIT_SYSTEM;
unsigned char WaitResponse = 0;

unsigned char i;
unsigned char statusReceive = INIT_RECEIVE;
unsigned char flagOfDataReceiveComplete = 0;
unsigned char indexOfMachine = 0;
unsigned char indexOfDataReceive = 0;
unsigned char indexOfdataSend = 0;
unsigned char numberOfDataReceive = 5;
unsigned char dataReceive [50];
unsigned char numberOfDataSend = 5;
unsigned char dataSend [50];

unsigned char tempCount = 0, TEMP_value = 0, pH_value = 0, FLOW_value = 0;
unsigned char COD_value = 0, TSS_value = 0, NO3_value = 0, NH4_value = 0;
unsigned int timeChangedataSend = 0;
unsigned char isButtonMessage();
unsigned char isButtonCall();
unsigned char isButtonGPRS();
unsigned char isButtonClear();

void uart_isr_simulate_machine()
{
    unsigned char tempReceive;
    tempReceive = RCREG;
    uart_putchar(tempReceive);
    switch(statusReceive)
    {
        case INIT_RECEIVE:
        case WAIT_HEADER_1:
            if (tempReceive == 0xff)
            {
                indexOfDataReceive = 0;
                statusReceive = WAIT_HEADER_2;
            }
            break;
        case WAIT_HEADER_2:
            if (tempReceive == 0xff)
                statusReceive = RECEIVE_DATA;
            else
                statusReceive = WAIT_HEADER_1;
            break;
        case RECEIVE_DATA:
            dataReceive [indexOfDataReceive] = tempReceive;
            indexOfDataReceive ++;
            if (indexOfDataReceive >= numberOfDataReceive)
                statusReceive = END_OF_RECEIVE_1;

            break;
        case END_OF_RECEIVE_1:
            if (tempReceive == 0xfe)
                statusReceive = END_OF_RECEIVE_2;
            else
                statusReceive = WAIT_HEADER_1;
            break;
        case END_OF_RECEIVE_2:
            if (tempReceive == 0xfe)
            {
                flagOfDataReceiveComplete = 1;
                statusReceive = INIT_RECEIVE;
            }
            else
                statusReceive = WAIT_HEADER_1;
            break;
        default:
            statusReceive = INIT_RECEIVE;
            break;
    }
}

void SimulatedataSend()
{
   
}

void DisplayDataReceive()
{
    if(flagOfDataReceiveComplete == 1)
    {
        //flagOfDataReceiveComplete = 0;
        LcdPrintStringS(0,0,"                ");
        for (i = 0; i<numberOfDataReceive;i++)
            LcdPrintNumS(0,3*i,dataReceive[i]);
//        LcdPrintNumS(0,15,statusReceive);
        LcdPrintStringS(0,14,"OK");
    }

}
