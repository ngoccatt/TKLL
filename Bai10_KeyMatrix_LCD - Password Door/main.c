#include "main.h"
#include <ctype.h>
#include <string.h>
// Noi khai bao hang so
#define     LED     PORTD
#define     ON      1
#define     OFF     0

#define     ADDRESS_FIRST_PROGRAM   0x20

#define     INIT_SYSTEM        0
#define     ENTER_PASSWORD     1
#define     CHECK_PASSWORD     2
#define     UNLOCK_DOOR        3
#define     WRONG_PASSWORD     4
#define     ENTER_ID           5
#define     CHECK_ID           6
#define     INVALID_ID         7
#define     ADMIN_DASHBOARD         8
#define     ADMIN_CHANGE_PASS       9
#define     ADMIN_MEMBER_MANAGER    10
#define     ADMIN_LOG               11
#define     APPLY_NEW_PASS          12
#define     ADMIN_ADD_MEMBER        13
#define     ADMIN_REMOVE_MEMBER     14
#define     ADMIN_CHANGE_MEMBER     15
#define     NEW_MEMBER_CREATED      16
#define     CONFIRM_REMOVE_MEMBER   17
#define     REMOVE_COMPLETE         18
#define     CHANGE_MEMBER_PASSWORD  19
#define     APPLY_MEM_NEW_PASS      20
#define     USER_DASHBOARD          21
#define     USER_CHANGE_PASS        22
#define     RESET_CHECKIN           23

// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char statusOutput[8] = {0,0,0,0,0,0,0,0};
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void OpenOutput(int index);
void CloseOutput(int index);
void TestOutput(void);
void ReverseOutput(int index);
void Test_KeyMatrix();
void createlog();

void SetupTimeForRealTime();
void SetupForFirstProgram(void);
void DisplayTime();

//Chuong trinh Password Door
#define PASSWORD_LENGTH 6
#define MAX_ACCOUNT     10
#define ERROR_RETURN    0xffff
#define CHAR_ERROR_RETURN 0xf
#define MAX_ID_LENGTH   3
#define ADMIN_NUM_OF_PAGES     3
#define MANAGE_NUM_OF_PAGES     2


unsigned char second = 0,minute = 0,hour = 0;
unsigned char day = 0;
unsigned char date = 0,month = 0,year = 0;

enum Check_in {HIEN_DIEN, TRE, VANG};
unsigned int hien_dien = 0;
unsigned int vang = 0;
unsigned int tre = 0;

unsigned char arrayMapOfNumber [16] = {1,2,3,'A',4,5,6,'B',
                                       7,8,9,'C','*',0,'E','D'};
unsigned char arrayMapOfPassword [5][PASSWORD_LENGTH]= {
  {1,2,3,4,5,6},
  {2,7,8,9,7,8},
  {3,3,3,3,3,3},
  {4,8,6,8,2,1},
  {5,'A','B','C','D', "F"},
};

typedef struct user_account {
    unsigned int ID;
    unsigned char password[PASSWORD_LENGTH];
    enum Check_in checkin;
} user_account;

user_account account[MAX_ACCOUNT] = {
    {0, {1,2,3,4,5,6}, VANG},
    {1, {2,7,8,9,7,8}, VANG},
    {2, {3,3,3,3,3,3}, VANG},
    {3, {4,8,6,8,2,1}, VANG}
};

unsigned char arrayPassword[PASSWORD_LENGTH];
unsigned int ID_value = 0;

unsigned char statusPassword = INIT_SYSTEM;

unsigned char indexOfNumber = 0;
unsigned char indexOfID = 0;
unsigned char timeDelay = 0;

unsigned int current_user = 0;
unsigned int num_of_user = 4;
unsigned int index_user = 4;

//Variable use for admin section:

unsigned char admin_page[ADMIN_NUM_OF_PAGES][2] = {
    {UNLOCK_DOOR, ADMIN_CHANGE_PASS},
    {ADMIN_MEMBER_MANAGER, ADMIN_LOG},
    {RESET_CHECKIN, ADMIN_DASHBOARD}
};
unsigned char ad_current_page = 0;
unsigned char manage_page[MANAGE_NUM_OF_PAGES][2] = {
    {ADMIN_ADD_MEMBER, ADMIN_REMOVE_MEMBER},
    {ADMIN_CHANGE_MEMBER, ADMIN_MEMBER_MANAGER}
};
unsigned char ad_mem_cur_page = 0;

unsigned char ad_num_member_list = 0;
unsigned char ad_cur_mem_list = 0;
unsigned char ad_current_member = 0;

//Variable use for user section:

unsigned char user_page[2] = {UNLOCK_DOOR, USER_CHANGE_PASS};


void App_PasswordDoor();
unsigned char CheckPassword();
unsigned char isButtonNumber();
unsigned char numberValue;
unsigned char isButtonEnter();
unsigned char isButtonBack();
void UnlockDoor();
void LockDoor();
void DoorClosed();
void displayID(int x, int y, unsigned int value, unsigned char indexOfID);
void reset_package();
void resetCheckin();

#define     LIGHT_ON      1
#define     LIGHT_OFF     0
void BaiTap_Den();
unsigned char isButtonLight();
unsigned char statusLight = LIGHT_OFF;
void LightOn();
void LightOff();
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	unsigned int k = 0;
	init_system();
        //TestOutput();
	while (1)
	{
            while (!flag_timer3);
            flag_timer3 = 0;
            // thuc hien cac cong viec duoi day
            scan_key_matrix();
            App_PasswordDoor();
            DisplayLcdScreen();
	}
}
// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void init_system(void)
{
        TRISB = 0x00;		//setup PORTB is output
        TRISD = 0x00;       //PORTD is output, too
        init_lcd();
        LcdClearS();
//        LED = 0x00;
        init_interrupt();
        delay_ms(1000);
        init_timer0(4695);//dinh thoi 1ms sai so 1%
        init_timer1(9390);//dinh thoi 2ms
        init_timer3(46950);//dinh thoi 10ms
        SetTimer0_ms(2);
        SetTimer1_ms(10);
        SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
        init_key_matrix();
//        disable_uart();
//        init_adc();
//        init_i2c();
}

void OpenOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED | arrayMapOfOutput[index];
	}

}

void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED & ~arrayMapOfOutput[index];
	}
}

void ReverseOutput(int index)
{
    if (statusOutput[index]  == ON)
    {
        CloseOutput(index);
        statusOutput[index] = OFF;
    }
    else
    {
        OpenOutput(index);
        statusOutput[index] = ON;
    }
}

void TestOutput(void)
{
	int k;
	for (k=0;k<=7 ;k++ )
	{
		OpenOutput(k);
		delay_ms(500);
		CloseOutput(k);
		delay_ms(500);
	}
}

unsigned int CheckID(unsigned int id) {
    int i = 0;
    for(i = 0; i < num_of_user; i++) {
        if (account[i].ID == id) return i;
    }
    return ERROR_RETURN;
}

void App_PasswordDoor()
{
    switch (statusPassword)
    {
        case INIT_SYSTEM:
            LcdPrintStringS(0,0,"PRESS # FOR PASS");
//            LcdPrintStringS(1,0,"                ");
//            DisplayTime();
            DoorClosed();
            if (isButtonEnter())
            {
                reset_package();
                ID_value = 0;
                statusPassword = ENTER_ID;
            }
            break;
        case ENTER_ID:
            timeDelay++;
            LcdPrintStringS(0, 0, "ENTER ID        ");
            if (isButtonNumber()) {
                //phim A dung de xoa khi bam sai.
                if (numberValue == 'A') {
                    if (indexOfID > 0) {
                        ID_value = ID_value / 10;
                        indexOfID--;
                    }
                }
                else if (numberValue < 10){
                    ID_value = ID_value * 10 + numberValue;
                    indexOfID++;
                }
                //everytime we press a button, it mean that we're active, so reset this
                //timeDelay pls
                timeDelay = 0;
            }
            
            displayID(1,0,ID_value,indexOfID);
            LcdPrintStringS(1,3, "             ");
            if (indexOfID >= MAX_ID_LENGTH || isButtonEnter()) {
                reset_package();
                statusPassword = CHECK_ID;
            }
            if (isButtonBack()) {
                statusPassword = INIT_SYSTEM;
            }
            if (timeDelay >= 600) {
                statusPassword = INIT_SYSTEM;
            }
            break;
        case CHECK_ID:
            timeDelay = 0;
            current_user = CheckID(ID_value);
            if (current_user != ERROR_RETURN) {
                statusPassword = ENTER_PASSWORD;
                LcdClearS();
            }
            else
                statusPassword = INVALID_ID;
            break;
        case INVALID_ID: 
            timeDelay++;
            LcdPrintStringS(0,0,"Invalid ID      ");
            if (timeDelay >= 40)
                statusPassword = INIT_SYSTEM;
            break;
        case ENTER_PASSWORD:
            LcdPrintStringS(0,0,"PASS FOR ID ");
            LcdPrintNumS(0, 13, account[current_user].ID);
            timeDelay++;
            if (isButtonNumber())
            {
                if (numberValue == 'A') {
                   if (indexOfNumber > 0) {
                       indexOfNumber--;
                       LcdPrintStringS(1,indexOfNumber," ");
                   }
                }
                else if (numberValue < 10){
                    LcdPrintStringS(1,indexOfNumber,"*");
                    arrayPassword [indexOfNumber] = numberValue;
                    indexOfNumber++;
                }
                timeDelay = 0;
            }
            if (indexOfNumber >= PASSWORD_LENGTH)
                statusPassword = CHECK_PASSWORD;
            if (timeDelay >= 600)
                statusPassword = INIT_SYSTEM;
            if (isButtonBack())
                statusPassword = INIT_SYSTEM;
            break;
        case CHECK_PASSWORD:
            timeDelay = 0;
            indexOfNumber = 0;
            if (CheckPassword()) 
                if (current_user == 0) {
                    statusPassword = ADMIN_DASHBOARD;
                } else {       
                    statusPassword = USER_DASHBOARD;
                }
            else
                statusPassword = WRONG_PASSWORD;
            break;
        case USER_DASHBOARD:
            timeDelay++;
            LcdPrintLineS(0, "1.OPEN DOOR");
            LcdPrintLineS(1, "2.CHANGE PASS");
            
            if (isButtonPress == 1) timeDelay = 0;
            
            if (isButtonBack()){
                statusPassword = INIT_SYSTEM;
            }
            if (key_code[0] == 1) {
                statusPassword = user_page[0];
                reset_package();
            }
            if (key_code[1] == 1) {
                statusPassword = user_page[1];
                reset_package();
            }
            if (timeDelay > 600) {      
                statusPassword = INIT_SYSTEM;
            }
            break;
        case USER_CHANGE_PASS:
            timeDelay++;
            LcdPrintLineS(0, "CHANGE USER PASS");
            if (isButtonNumber())
            {
                if (numberValue == 'A') {
                   if (indexOfNumber > 0) {
                       indexOfNumber--;
                       LcdPrintStringS(1,indexOfNumber," ");
                   }
                }
                else if (numberValue < 10){
                    if (indexOfNumber < PASSWORD_LENGTH) {
                        LcdPrintStringS(1,indexOfNumber,"*");
                        arrayPassword [indexOfNumber] = numberValue;
                        indexOfNumber++;
                    }
                }
                timeDelay = 0;
            }
            if (isButtonEnter() && indexOfNumber >= PASSWORD_LENGTH) {
                statusPassword = APPLY_NEW_PASS;
                reset_package();
            }
            if (isButtonBack()) {
                statusPassword = USER_DASHBOARD;
                reset_package();
            }
            if (timeDelay > 600) {
                statusPassword = INIT_SYSTEM;
            }
            break;
        case ADMIN_DASHBOARD:
            timeDelay++;
            switch(ad_current_page) {
                case 0:
                    LcdPrintLineS(0, "1.OPEN DOOR");
                    LcdPrintLineS(1, "2.CHANGE PASS");
                    break;
                case 1:
                    LcdPrintLineS(0, "1.MANAGE USER");
                    LcdPrintLineS(1, "2.CREATE LOG");
                    break;
                case 2:
                    LcdPrintLineS(0, "1.CLEAR CHECK IN");
                    LcdPrintLineS(1, "");
                    break;
                default:
                    break;
            } 
            
            if (isButtonPress == 1) timeDelay = 0;
            
            if (isButtonEnter()) {
                ad_current_page = (ad_current_page + 1) % ADMIN_NUM_OF_PAGES;
                timeDelay = 0;
            }
            if (isButtonBack()) {
               statusPassword = INIT_SYSTEM;
            }
            if (key_code[0] == 1) {
                statusPassword = admin_page[ad_current_page][0];
                reset_package();
            }
            if (key_code[1] == 1) {
                statusPassword = admin_page[ad_current_page][1];
                reset_package();
            }
            if (timeDelay > 100) {      
                statusPassword = INIT_SYSTEM;
            }
            break;
        case ADMIN_CHANGE_PASS:
            timeDelay++;
            LcdPrintLineS(0, "CHANGE AD PASS");
            if (isButtonNumber())
            {
                if (numberValue == 'A') {
                   if (indexOfNumber > 0) {
                       indexOfNumber--;
                       LcdPrintStringS(1,indexOfNumber," ");
                   }
                }
                else if (numberValue < 10){
                    if (indexOfNumber < PASSWORD_LENGTH) {
                        LcdPrintStringS(1,indexOfNumber,"*");
                        arrayPassword [indexOfNumber] = numberValue;
                        indexOfNumber++;
                    }
                }
                timeDelay = 0;
            }
            if (isButtonEnter() && indexOfNumber >= PASSWORD_LENGTH) {
                statusPassword = APPLY_NEW_PASS;
                reset_package();
            }
            if (isButtonBack()) {
                statusPassword = ADMIN_DASHBOARD;
                reset_package();
            }
            if (timeDelay > 600) {
                statusPassword = INIT_SYSTEM;
            }
            break;
        case APPLY_NEW_PASS:        
            timeDelay++;
            LcdPrintLineS(0,"CHANGE SUCCESS");
            if (timeDelay == 1) {
                memmove(account[current_user].password, arrayPassword, sizeof(arrayPassword));
            }
            if (timeDelay > 40) {
                statusPassword = INIT_SYSTEM;
            }
            break;
        case ADMIN_MEMBER_MANAGER:
            timeDelay++;
            switch(ad_mem_cur_page) {
                case 0:
                    LcdPrintLineS(0, "1.ADD MEMBER");
                    LcdPrintLineS(1, "2.REMOVE MEMBER");
                    break;
                case 1:
                    LcdPrintLineS(0, "1.CHANGE MEM PW");
                    LcdPrintLineS(1, " ");
                    break;
                default:
                    break;
            } 
            if (isButtonPress == 1) timeDelay = 0;
            if (isButtonEnter()) {
                ad_mem_cur_page = (ad_mem_cur_page + 1) % MANAGE_NUM_OF_PAGES;
                timeDelay = 0;
            }
            if (isButtonBack()) {
                statusPassword = ADMIN_DASHBOARD;
                reset_package();
            }
            if (key_code[0] == 1) {
                statusPassword = manage_page[ad_mem_cur_page][0];
                reset_package();       
                //cap nhat lai so trang cua member khi vao ham REMOVE_MEMBER hay CHANGE_MEMBER
                ad_num_member_list = (((num_of_user - 1) % 4 == 0) ? ((num_of_user - 1) / 4) : ((num_of_user - 1) / 4) + 1);
            }
            if (key_code[1] == 1) {
                statusPassword = manage_page[ad_mem_cur_page][1];
                reset_package();  
                //cap nhat lai so trang cua member khi vao ham REMOVE_MEMBER hay CHANGE_MEMBER
                ad_num_member_list = (((num_of_user - 1) % 4 == 0) ? ((num_of_user - 1) / 4) : ((num_of_user - 1) / 4) + 1);
            }
            if (timeDelay > 600) {      
                statusPassword = ADMIN_DASHBOARD;
            }
            break;
        case ADMIN_ADD_MEMBER:
            timeDelay++;
            LcdPrintStringS(0,0,"PW FOR ID ");
            LcdPrintNumS(0,10,index_user);
            if (isButtonNumber())
            {
                if (numberValue == 'A') {
                   if (indexOfNumber > 0) {
                       indexOfNumber--;
                       LcdPrintStringS(1,indexOfNumber," ");
                   }
                }
                else if (numberValue < 10){
                    if (indexOfNumber < PASSWORD_LENGTH) {
                        LcdPrintStringS(1,indexOfNumber,"*");
                        arrayPassword [indexOfNumber] = numberValue;
                        indexOfNumber++;
                    }
                }
                timeDelay = 0;
            }
            if (isButtonEnter() && indexOfNumber >= PASSWORD_LENGTH) {
                statusPassword = NEW_MEMBER_CREATED;
                reset_package();
            }
            if (isButtonBack()) {
                statusPassword = ADMIN_MEMBER_MANAGER;
                reset_package();
            }
            if (timeDelay > 600) {      
                statusPassword = ADMIN_DASHBOARD;
                reset_package();
            }
            break;
        case NEW_MEMBER_CREATED:
            timeDelay++;
            LcdPrintStringS(0,0,"NEW ID : ");
            LcdPrintStringS(1,0,"PASSWORD: ");
            if (timeDelay == 1) {
                int i = 0;
                account[num_of_user].ID = index_user;
                for(i = 0; i < PASSWORD_LENGTH; i++) {
                    account[num_of_user].password[i] = arrayPassword[i];
                    LcdPrintCharS(1,10 + i, arrayPassword[i] + '0');
                }
                account[num_of_user].checkin = VANG;
                index_user++;
                num_of_user++;
            }
            LcdPrintNumS(0,9,index_user - 1);
            
            if (timeDelay > 40) {
                statusPassword = ADMIN_MEMBER_MANAGER;
                reset_package();
            }
            break;
        case ADMIN_REMOVE_MEMBER:
            //SHOW MEMBER ONLY, DO NOT SHOW ADMIN!
            timeDelay++;
            if ((ad_cur_mem_list * 4 + 1) < num_of_user) {
                LcdPrintStringS(0,0,"1. ");
                LcdPrintNumS(0,3,account[ad_cur_mem_list * 4 + 1].ID);
            }
            if ((ad_cur_mem_list * 4 + 2) < num_of_user) {
                LcdPrintStringS(0,8,"2. ");
                LcdPrintNumS(0,11,account[ad_cur_mem_list * 4 + 2].ID);
            }
            
            if ((ad_cur_mem_list * 4 + 3) < num_of_user) {
                LcdPrintStringS(1,0,"3. ");
                LcdPrintNumS(1,3,account[ad_cur_mem_list * 4 + 3].ID);
            }
            if ((ad_cur_mem_list * 4 + 4) < num_of_user) {
                LcdPrintStringS(1,8,"4. ");
                LcdPrintNumS(1,11,account[ad_cur_mem_list * 4 + 4].ID);
            }
            
            if (isButtonEnter()) {
                ad_cur_mem_list = (ad_cur_mem_list + 1) % ad_num_member_list;
                LcdClearS();
                timeDelay = 0;
            }
            if (isButtonBack()) {
                statusPassword = ADMIN_MEMBER_MANAGER;
                reset_package();
            }
            if (isButtonNumber()) {
                if (numberValue == 1 && ((ad_cur_mem_list * 4 + 1) < num_of_user)) {
                    ad_current_member = ad_cur_mem_list * 4 + 1;
                }
                else if (numberValue == 2 && ((ad_cur_mem_list * 4 + 2) < num_of_user)) {
                    ad_current_member = ad_cur_mem_list * 4 + 2;
                }
                else if (numberValue == 3 && ((ad_cur_mem_list * 4 + 3) < num_of_user)) {
                    ad_current_member = ad_cur_mem_list * 4 + 3;
                }
                else if (numberValue == 4 && ((ad_cur_mem_list * 4 + 4) < num_of_user)) {
                    ad_current_member = ad_cur_mem_list * 4 + 4;
                } else {
                    ad_current_member = CHAR_ERROR_RETURN; 
                }
                if (ad_current_member != CHAR_ERROR_RETURN ) {
                    statusPassword = CONFIRM_REMOVE_MEMBER;
                    reset_package();
                }
                timeDelay = 0;
            }
            if (timeDelay > 600) {
                statusPassword = ADMIN_MEMBER_MANAGER;
                reset_package();
            }
            break;
        case CONFIRM_REMOVE_MEMBER:
            timeDelay++;
            LcdPrintStringS(0, 0, "REMOVE ID ");
            LcdPrintNumS(0, 10, account[ad_current_member].ID);
            LcdPrintCharS(0, 15, '?');
            LcdPrintStringS(1,0,"N:BACK");
            LcdPrintStringS(1,8,"Y:ENTER");
            
            if(isButtonPress == 1) timeDelay = 0;
            
            if (isButtonEnter()) {
                reset_package();
                statusPassword = REMOVE_COMPLETE;
            } 
            if (isButtonBack()) {
                reset_package();
                statusPassword = ADMIN_REMOVE_MEMBER;
            }
            if (timeDelay > 600) {
                reset_package();
                statusPassword = ADMIN_MEMBER_MANAGER;
            }
            
            break;
        case REMOVE_COMPLETE:
            timeDelay++;
            LcdPrintStringS(0,0,"REMOVE COMPLETE");
            if (timeDelay == 1) {
                memmove(account + ad_current_member, account + ad_current_member + 1,
                        (num_of_user - ad_current_member - 1) * sizeof(user_account));
                if (num_of_user > 0) {
                    num_of_user--;
                }
                
            }
            if (timeDelay > 40) {
                reset_package();
                statusPassword = ADMIN_MEMBER_MANAGER;
            }
            break;
        case ADMIN_CHANGE_MEMBER:
            timeDelay++;
            if ((ad_cur_mem_list * 4 + 1) < num_of_user) {
                LcdPrintStringS(0,0,"1. ");
                LcdPrintNumS(0,3,account[ad_cur_mem_list * 4 + 1].ID);
            }
            if ((ad_cur_mem_list * 4 + 2) < num_of_user) {
                LcdPrintStringS(0,8,"2. ");
                LcdPrintNumS(0,11,account[ad_cur_mem_list * 4 + 2].ID);
            }
            
            if ((ad_cur_mem_list * 4 + 3) < num_of_user) {
                LcdPrintStringS(1,0,"3. ");
                LcdPrintNumS(1,3,account[ad_cur_mem_list * 4 + 3].ID);
            }
            if ((ad_cur_mem_list * 4 + 4) < num_of_user) {
                LcdPrintStringS(1,8,"4. ");
                LcdPrintNumS(1,11,account[ad_cur_mem_list * 4 + 4].ID);
            }
            
            if (isButtonEnter()) {
                ad_cur_mem_list = (ad_cur_mem_list + 1) % ad_num_member_list;
                LcdClearS();
                timeDelay = 0;
            }
            if (isButtonBack()) {
                statusPassword = ADMIN_MEMBER_MANAGER;
                reset_package();
            }
            if (isButtonNumber()) {
                if (numberValue == 1 && ((ad_cur_mem_list * 4 + 1) < num_of_user)) {
                    ad_current_member = ad_cur_mem_list * 4 + 1;
                }
                else if (numberValue == 2 && ((ad_cur_mem_list * 4 + 2) < num_of_user)) {
                    ad_current_member = ad_cur_mem_list * 4 + 2;
                }
                else if (numberValue == 3 && ((ad_cur_mem_list * 4 + 3) < num_of_user)) {
                    ad_current_member = ad_cur_mem_list * 4 + 3;
                }
                else if (numberValue == 4 && ((ad_cur_mem_list * 4 + 4) < num_of_user)) {
                    ad_current_member = ad_cur_mem_list * 4 + 4;
                } else {
                    ad_current_member = CHAR_ERROR_RETURN;
                }
                if (ad_current_member != CHAR_ERROR_RETURN ) {
                    statusPassword = CHANGE_MEMBER_PASSWORD;
                    reset_package();
                }
                timeDelay = 0;
            }
            if (timeDelay > 600) {
                statusPassword = ADMIN_MEMBER_MANAGER;
                reset_package();
            }
            break;
        case CHANGE_MEMBER_PASSWORD:
            timeDelay++;
            LcdPrintStringS(0,0,"NEW PW ID ");
            LcdPrintNumS(0, 11, account[ad_current_member].ID);
            if (isButtonNumber())
            {
                if (numberValue == 'A') {
                   if (indexOfNumber > 0) {
                       indexOfNumber--;
                       LcdPrintStringS(1,indexOfNumber," ");
                   }
                }
                else if (numberValue < 10){
                    if (indexOfNumber < PASSWORD_LENGTH) {
                        LcdPrintStringS(1,indexOfNumber,"*");
                        arrayPassword [indexOfNumber] = numberValue;
                        indexOfNumber++;
                    }
                }
                timeDelay = 0;
            }
            if (isButtonEnter() && indexOfNumber >= PASSWORD_LENGTH) {
                statusPassword = APPLY_MEM_NEW_PASS;
                reset_package();
            }
            if (isButtonBack()) {
                statusPassword = ADMIN_CHANGE_MEMBER;
                reset_package();
            }        
            if (timeDelay > 600) {
                statusPassword = ADMIN_MEMBER_MANAGER;
                reset_package();
            }
            break;
        case APPLY_MEM_NEW_PASS:
            timeDelay++;
            LcdPrintLineS(0,"CHANGE SUCCESS");
            if (timeDelay == 1) {
                memmove(account[ad_current_member].password, arrayPassword, sizeof(arrayPassword));
            }
            if (timeDelay > 40) {
                statusPassword = ADMIN_MEMBER_MANAGER;
                reset_package();
            }
            break;
        case ADMIN_LOG:
            timeDelay++;
            if (timeDelay == 1) {
                createlog();
            }
            LcdPrintStringS(0,0 ,"LOG     ");
            LcdPrintStringS(0,8 ,"HD     ");
            LcdPrintNumS(0,12, hien_dien);
            LcdPrintStringS(1,0 ,"T     ");
            LcdPrintNumS(1,3, tre);
            LcdPrintStringS(1,8 ,"V     ");
            LcdPrintNumS(1,12, vang);
            if (isButtonBack()) {
                statusPassword = ADMIN_DASHBOARD;
                reset_package();
            }
            if (timeDelay > 600) {
                statusPassword = INIT_SYSTEM;
            }
            break;
        case UNLOCK_DOOR:
            
            timeDelay++;
            if (timeDelay == 1) {
                account[current_user].checkin = HIEN_DIEN;
            }
            LcdPrintStringS(0,0,"OPENING DOOR    ");
            if (timeDelay < 50) {
                UnlockDoor();
            } else {
                LockDoor();
            }
            
            if (timeDelay >= 100) {
                statusPassword = INIT_SYSTEM;
//                disable_uart();
            }
            break;
        case RESET_CHECKIN:
            timeDelay++;
            LcdPrintLineS(0,"CLEAR CHECK IN");
            if (timeDelay == 1) {
                resetCheckin();
            }
            if (timeDelay >= 40) {
                statusPassword = ADMIN_DASHBOARD;
                reset_package();
            }
            break;
        case WRONG_PASSWORD:
            timeDelay++;
            LcdPrintStringS(0,0,"PASSWORD WRONG  ");
            if (timeDelay >= 40)
                statusPassword = INIT_SYSTEM;
            break;
        default:
            break;

    }
}
unsigned char CheckPassword()
{
    unsigned char i,j;
    for (i = 0; i < PASSWORD_LENGTH; i++) {
        if (account[current_user].password[i] != arrayPassword[i]) return 0;
    }
    return 1;

}
unsigned char isButtonNumber()
{
    unsigned char i;
    for (i = 0; i<=15; i++)
        if (key_code[i] == 1)
        {
            numberValue = arrayMapOfNumber[i];
            return 1;
        }
    return 0;
}

unsigned char isButtonEnter()
{
    if (key_code[14] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonBack()
{
    if (key_code[12] == 1)
        return 1;
    else
        return 0;
}
void UnlockDoor()
{
    OpenOutput(0);
    CloseOutput(1);
}
void LockDoor()
{
    CloseOutput(0);
    OpenOutput(1);
}

void DoorClosed() {
    CloseOutput(0);
    CloseOutput(1);
}

void displayID(int x, int y, unsigned int value, unsigned char indexOfID) {
    if (value < 10 && indexOfID == 3) {
        LcdPrintStringS(x,y,"00");
        LcdPrintNumS(x,y+2,value);
    } else if (value < 10 && indexOfID == 2) {
        LcdPrintStringS(x,y,"0");
        LcdPrintNumS(x,y+1,value);
    } else if (value < 100 && indexOfID == 3) {
        LcdPrintStringS(x,y,"0");
        LcdPrintNumS(x,y+1,value);
    } else {
        LcdPrintLineS(x,"");
        LcdPrintNumS(x,y,value);
    }
}

void reset_package() {
    LcdClearS();
    timeDelay = 0;
    ad_cur_mem_list = 0;        //list of member
    ad_current_page = 0;        //list of admin function
    ad_mem_cur_page = 0;        //list of admin member management function
    indexOfNumber = 0;
    indexOfID = 0;
}

void createlog() {
    int i = 0;
    hien_dien = tre = vang = 0;
    for(i = 0; i < num_of_user; i++) {
        unsigned char state = account[i].checkin;
        switch(state) {
            case HIEN_DIEN:
                hien_dien++;
            break;
            case TRE:
                tre++;
            break;
            case VANG:
                vang++;
            break;    
        }
    }
}

void resetCheckin() {
    int i = 0;
    for (i = 0; i < num_of_user; i++) {
        account[i].checkin = VANG;
    }
}

void SetupForFirstProgram(void)
{
    if(Read_DS1307(ADDRESS_FIRST_PROGRAM) != 0x22)
    {
        SetupTimeForRealTime();
        Write_DS1307(ADDRESS_FIRST_PROGRAM, 0x22);
    }
}

void SetupTimeForRealTime()
{
    second = 50;
    minute = 59;
    hour = 23;
    date = 31;
    month = 12;
    year = 14;
    
    write_ds1307(ADDRESS_SECOND, second);
    write_ds1307(ADDRESS_MINUTE, minute);
    write_ds1307(ADDRESS_HOUR, hour);
    write_ds1307(ADDRESS_DATE, date);
    write_ds1307(ADDRESS_MONTH, month);
    write_ds1307(ADDRESS_YEAR, year);
}

void DisplayTime()
{
    second = Read_DS1307(ADDRESS_SECOND);
    minute = Read_DS1307(ADDRESS_MINUTE);
    hour = Read_DS1307(ADDRESS_HOUR);
//    day = Read_DS1307(ADDRESS_DAY);
    date = Read_DS1307(ADDRESS_DATE);
    month = Read_DS1307(ADDRESS_MONTH);
//    year = Read_DS1307(ADDRESS_YEAR);

    //////day
//    switch(day)
//    {
//        case 1:
//            LcdPrintStringS(0,0,"SUN");
//            break;
//        case 2:
//            LcdPrintStringS(0,0,"MON");
//            break;
//        case 3:
//            LcdPrintStringS(0,0,"TUE");
//            break;
//        case 4:
//            LcdPrintStringS(0,0,"WED");
//            break;
//        case 5:
//            LcdPrintStringS(0,0,"THU");
//            break;
//        case 6:
//            LcdPrintStringS(0,0,"FRI");
//            break;
//        case 7:
//            LcdPrintStringS(0,0,"SAT");
//            break;
//    }
    if(hour < 10)
    {
        LcdPrintStringS(1,0,"0");
        LcdPrintNumS(1,1,hour);
    }
    else
        LcdPrintNumS(1,0,hour);
    
    LcdPrintStringS(1,2,":");
    if(minute < 10)
    {
        LcdPrintStringS(1,3,"0");
        LcdPrintNumS(1,4,minute);
    }
    else
        LcdPrintNumS(1,3,minute);

    LcdPrintStringS(1,5,":");
    if(second < 10)
    {
        LcdPrintStringS(1,6,"0");
        LcdPrintNumS(1,7,second);
    }
    else
        LcdPrintNumS(1,6,second);
    
//    switch(month)
//    {
//        case 1:
//            LcdPrintStringS(1,8,"JAN");
//            break;
//        case 2:
//            LcdPrintStringS(1,8,"FEB");
//            break;
//        case 3:
//            LcdPrintStringS(1,8,"MAR");
//            break;
//        case 4:
//            LcdPrintStringS(1,8,"APR");
//            break;
//        case 5:
//            LcdPrintStringS(1,8,"MAY");
//            break;
//        case 6:
//            LcdPrintStringS(1,8,"JUN");
//            break;
//        case 7:
//            LcdPrintStringS(1,8,"JUL");
//            break;
//        case 8:
//            LcdPrintStringS(1,8,"AUG");
//            break;
//        case 9:
//            LcdPrintStringS(1,8,"SEP");
//            break;
//        case 10:
//            LcdPrintStringS(1,8,"OCT");
//            break;
//        case 11:
//            LcdPrintStringS(1,8,"NOV");
//            break;
//        case 12:
//            LcdPrintStringS(1,8,"DEC");
//            break;
//    }

//    LcdPrintStringS(1,11," ");
    if(date < 10)
    {
        LcdPrintStringS(1,11," ");
        LcdPrintNumS(1,12,date);
    }
    else
        LcdPrintNumS(1,11,date);
    LcdPrintStringS(1,13,"/");
    if (month < 10) {
        LcdPrintStringS(1,14," ");
        LcdPrintNumS(1,15,month);
    } else {
        LcdPrintNumS(1,14,month);
    }
//    LcdPrintNumS(1,9,20);
//    LcdPrintNumS(1,15,year);

}