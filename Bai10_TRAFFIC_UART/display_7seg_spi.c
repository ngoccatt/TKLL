
#include "display_7seg_spi.h"


#include "spi_stimulate.h"


#define NUM_OF_LED 4

unsigned char ledBuffer[NUM_OF_LED] = {1,2,3,4};
unsigned char numberData[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D
								,0x7D, 0X07, 0X7F, 0X6F};
unsigned char ledIndex[NUM_OF_LED] = {0x0e, 0x0d, 0x0b, 0x07};
static int led_index = 0;


/* Ham nay dung de truyen du lieu ra 2 con 74HC595 xep chong.
 * Dau tien, truyen truoc du lieu de xac dinh led nao duoc bat
 * O lan truyen dau tien nay, du lieu se nam trong con 74HC595 thu 1. 
 * Tuy nhien, minh muon dung con 74HC595 thu 1 de dieu khien hien thi so tren 
 * led 7 doan, con viec xac dinh led nao duoc bat thi su dung con 74HC595 thu 2
 * Vi the, minh truyen vao them 1 byte nua, chua du lieu hien thi so led 7 doan.
 * Khi nay, du lieu hien thi so se nam o 74HC595 thu 1, con du lieu xac dinh led
 * nao duoc bat da~ bi day sang con 74HC595 thu 2 thong qua chan Q7'.
 * Truyen du lieu xong, minh bat cai LATCH (low -> high)
 * len de dem du lieu tu shift register
 * ra ngoai output voi ham data_to_output_SPI().
 * 
 * Co the noi, khi dung 74HC595 xep chong 2 lop, du lieu truyen se di tu con
 * 74HC595 thu nhat chay tu tu sang con thu 2. Vay nen, neu gui hai lan 8 bit, 
 * thi 8 bit dau tien se nam o con 74HC595 thu 2, con 8 bit lan 2 se nam o 74HC595
 * thu 1.
 * 
 * Luu y, khi su dung xep chong, 2 con 74HC595 co the xai chung tin hieu LATCH 
 * va SHIFT CLOCK.
 */
void display7Seg_SPI(int value, int led_index) {
//    first, choose the led index:
    transfer_data_SPI(ledIndex[led_index]);
    transfer_data_SPI(numberData[value]);
    data_to_output_SPI();
}

/* Ham nay tat tat ca cac led
 */

void turnOffAll7Seg() {
    transfer_data_SPI(0xff);
    transfer_data_SPI(0x00);
    data_to_output_SPI();
}


void scan7Seg_SPI() {
    display7Seg_SPI(ledBuffer[led_index], led_index);
    led_index = (led_index + 1) % NUM_OF_LED;
}


void updateLedBuffer_SPI(int value1, int value2) {
    ledBuffer[0] = value1 / 10;
    ledBuffer[1] = value1 % 10;
    ledBuffer[2] = value2 / 10;
    ledBuffer[3] = value2 % 10;
}
