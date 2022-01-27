#include "spi_stimulate.h"

//init chay SPI thu cong, set cac cong output o port D va port A
void init_SPI_manual() {
    TRIS_DATA_D &= 0b00111111;        //MAKE RD6 AND RD7 OUTPUT
    TRIS_LATCH_A &= 0b11111011;         //Make RA2 output
    DATA_PIN = 0;
    SHIFT_CLOCK = 0;
    LATCH = 0;
}

/*this function tranfer data to the shift register (not yet print to output)
 *de dua data vao 74HC595, minh dua data vao chan DATA_PIN, sau do dua chan SHIFT_CLOCK 
 *len cao. noi dung hon la tin hieu se duoc dua vao 74HC595 moi khi co xung
 * tu low -> high. data nay se duoc dua vao shift register, chua toi da 8 bit.
 * Neu dua nhieu hon 8bit, no bat dau day ra ngoai thong qua chan Q7' (proteus)
 * (chan Q7' chinh la logic cua bit thu 8 trong shift register)
 * Thong qua viec day ra ngoai nhu vay, minh co the xep chong 74HC595 de truyen du lieu.
 * 
 * Luu y, du lieu chi duoc dua vao shift register, du lieu moi duoc them vao se 
 * shift may thang phia truoc. Du lieu nam trong shift register khong duoc show ra ngoai
 * Muon dua du lieu tu shift register ra ngoai, phai day no ra latch thi du lieu
 * moi the hien ra ngoai o 8 cong.
 */ 
void transfer_data_SPI(unsigned char byte) {
    int i = 0;
    for (i = 0; i < 8; i++) {
        SHIFT_CLOCK = DOWN;
        if (byte % 2 == 0) {
            DATA_PIN = 0;
        } else {
            DATA_PIN = 1;
        }
        SHIFT_CLOCK = UP;
        byte = byte >> 1;
    }
    SHIFT_CLOCK = DOWN;
}

void delay_smol(int loop) {
    int i;
    for (i = 0; i < loop; i++) {
    }
}

/* Ham nay dung de day du lieu tu shift register ra latch
 * Du lieu se duoc day ra neu co 1 xung len (tu low -> high) cua tin hieu LATCH.
 * Nghia la, du lieu duoc shift vao trong shift register chua duoc dua ra. can
 * phai day tin hieu ra latch thi no moi hien dien o cac cong output.
 */
void data_to_output_SPI() {
    LATCH = 0;
    LATCH = 1;
    delay_smol(100);
    LATCH = 0;
}
