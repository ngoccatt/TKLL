#include "password_door.h"

#include "..\i2c\i2c.h"
#include "..\lcd\lcd.h"
#include "..\button_matrix\button.h"
#include "..\clock\clock.h"         //comment this if you simulator
//#include "..\clock\simu_clock.h"        //use this for simulator
#include <stdlib.h>

//get current time
unsigned char current_hour = 0;
unsigned char current_minute = 0;
unsigned char current_second = 0;


//present state
enum Check_in {HIEN_DIEN, TRE, VANG};
unsigned int hien_dien = 0;
unsigned int vang = 0;
unsigned int tre = 0;

//door state
enum Door_state {OPENED, CLOSED};
enum Door_state doorState = CLOSED;

//button value
unsigned char arrayMapOfNumber [16] = {1,2,3,'A',4,5,6,'B',
                                       7,8,9,'C','*',0,'E','D'};
unsigned char numberValue;

//user info
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

//an array hold input password
unsigned char arrayPassword[PASSWORD_LENGTH];

//ID_value hold ID input by user
unsigned int ID_value = 0;

//index, keep track of how many password character entered
unsigned char indexOfNumber = 0;

//index, keep track of how many ID character entered
unsigned char indexOfID = 0;

//Current User trying to login 
unsigned int current_user = 0;

//amount of User currently in database
unsigned int num_of_user = 4;

//an unique value, always increase every time we add a new user
//this value can serve as ID for each user
unsigned int index_user = 4;

//FSM state
unsigned char statusPassword = INIT_SYSTEM;

//Delay counter
unsigned int timeDelay = 0;

//Variable use for admin section:

//admin page
unsigned char admin_page[ADMIN_NUM_OF_PAGES] = {
    UNLOCK_DOOR, 
    ADMIN_CHANGE_PASS,
    ADMIN_MEMBER_MANAGER, 
    CHECK_IN,
    TIME,
};
unsigned char ad_current_page = 0;

//manage member page
unsigned char manage_page[MANAGE_NUM_OF_PAGES] = {
    ADMIN_ADD_MEMBER,
    ADMIN_REMOVE_MEMBER,
    ADMIN_CHANGE_MEMBER, 
};
unsigned char ad_mem_cur_page = 0;

//checkin page
unsigned char checkin_page[CHECKIN_NUM_OF_PAGES] = {
    ADMIN_LOG,
    LIST_PRESENCE,
    LIST_LATE,
    LIST_ABSENT,
    RESET_CHECKIN
};
unsigned char checkin_cur_page = 0;

//time page
unsigned char time_page[TIME_NUM_OF_PAGES] = {
    CHANGE_TIME,
    CHANGE_LATE_TIME
};
unsigned char time_cur_page = 0;



//how many lists needed to show all the user (member)
unsigned char ad_num_member_list = 0;

//current member list page
unsigned char ad_cur_mem_list = 0;

//current member selected for changing password or delete
unsigned char ad_current_member = 0;

//Variable use for user section:
unsigned char user_page[USER_NUM_OF_PAGES] = {
    UNLOCK_DOOR, 
    USER_CHANGE_PASS
};
unsigned char user_cur_page = 0;

//get member presence status: HIEN_DIEN, TRE or VANG?
int list_member_presence_status[8];
unsigned int index_list_of_presence = 0;
unsigned int index_list_of_late = 0;
unsigned int index_list_of_absent = 0;

//function prototype
unsigned int CheckID(unsigned int id);
void App_PasswordDoor();
unsigned char CheckPassword();
void displayID(int x, int y, unsigned int value, unsigned char indexOfID);
void reset_package();
void reset_smol_package();
void createlog();
void resetCheckin();
void UnlockDoor();
void LockDoor();
void DoorStop();
unsigned char isButtonNumber();
unsigned char isButtonNext();
unsigned char isButtonEnter();
unsigned char isButtonBack();

void get_list_member_presence_status(enum Check_in check, unsigned int *page);
void display_list_member_presence_status();


void get_list_member_presence_status(enum Check_in check, unsigned int *page) {
    int i = 0;
    int count_mem = 0;
    int array_index = 0;
    for(i = 0; i < 8; i++) {
        list_member_presence_status[i] = -1;
    }
    
    for(i = 0; i < num_of_user; i++) {
        if (account[i].checkin == check) {
            count_mem++;
            if (count_mem > ((*page) * 8)) {
                list_member_presence_status[array_index] = account[i].ID;
                array_index++;
                if (array_index >= 8) return;
            }
        }  
    }
         
    if ((*page) > 0 && count_mem <= ((*page) * 8)) {
        (*page) = (*page) - 1;
        array_index = 0;
        count_mem = 0;
        for(i = 0; i < num_of_user; i++) {
        if (account[i].checkin == check) {
            count_mem++;
            if (count_mem > ((*page) * 8)) {
                list_member_presence_status[array_index] = account[i].ID;
                array_index++;
                if (array_index >= 8) return;
                }
            } 
        }
    }  
}

void display_list_member_presence_status() {
    int i = 0;
    LcdClearS();
    if (list_member_presence_status[0] == -1) {
        LcdPrintLineS(0, "EMPTY");
        return;
    }
    for (i = 0; i < 8; i++) {
        if (i < 4) {
            if (list_member_presence_status[i] == -1) {
                LcdPrintStringS(0, i * 4, "    ");
            } else 
            {
                LcdPrintNumS(0, i * 4, list_member_presence_status[i]);
            }
        } else {
            if (list_member_presence_status[i] == -1) {
                LcdPrintStringS(1, (i - 4) * 4, "    ");
            } else 
            {
                LcdPrintNumS(1, (i - 4) * 4, list_member_presence_status[i]);
            }
        }
    }
}


unsigned int CheckID(unsigned int id) {
    int i = 0;
    for(i = 0; i < num_of_user; i++) {
        if (account[i].ID == id) return i;
    }
    return ERROR_RETURN;
}

unsigned char CheckPassword()
{
    unsigned char i,j;
    for (i = 0; i < PASSWORD_LENGTH; i++) {
        if (account[current_user].password[i] != arrayPassword[i]) return 0;
    }
    return 1;

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
    checkin_cur_page = 0;
    time_cur_page = 0;
    indexOfNumber = 0;
    indexOfID = 0;
}

void reset_smol_package() {
    LcdClearS();
    timeDelay = 0;
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

void DoorStop() {
    CloseOutput(0);
    CloseOutput(1);
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

unsigned char isButtonNext()
{
    if (key_code[14] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonEnter()
{
    if (key_code[13] == 1)
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

void App_PasswordDoor()
{
    switch (statusPassword)
    {
        case INIT_SYSTEM:
            LcdPrintStringS(1,0,"PRESS # FOR PASS");
            LcdPrintStringS(0,0,"                ");
            DisplayRealTime();
            DoorStop();
//            LockDoor();
//            UnlockDoor();
            doorState = CLOSED;
            if (isButtonNext())
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
            if (indexOfID > 0) {
                displayID(1,0,ID_value,indexOfID);
            } else {
                LcdPrintLineS(1," ");
            }
            
            LcdPrintStringS(1,3, "             ");
            if ((indexOfID >= MAX_ID_LENGTH || isButtonNext()) && indexOfID > 0) {
                reset_package();
                statusPassword = CHECK_ID;
            }
            if (isButtonBack()) {
                statusPassword = INIT_SYSTEM;
            }
            if (timeDelay >= 300) {
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
            if (indexOfNumber >= PASSWORD_LENGTH) {
                statusPassword = CHECK_PASSWORD;
                reset_package();
            }
            if (timeDelay >= 300)
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
            switch(user_cur_page) {
                case 0:
                    LcdPrintLineS(0, "  1.OPEN DOOR  ");
//                    LcdPrintLineS(1, "       @      >");
                    LcdPrintCharS(1,0, LONG_LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 1:
                    LcdPrintLineS(0, "  2.CHANGE PW  ");
//                    LcdPrintLineS(1, "<      @       ");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);

                    break;
                default:
                    break;
            } 
            
            if (isButtonNext()) {
//                user_cur_page = (user_cur_page + 1) % USER_NUM_OF_PAGES;
                user_cur_page++;
                if (user_cur_page >= USER_NUM_OF_PAGES) 
                    user_cur_page = USER_NUM_OF_PAGES - 1;
                timeDelay = 0;
                reset_smol_package();
            }
            if (isButtonBack()) {
//                  statusPassword = INIT_SYSTEM;
                if (user_cur_page <= 0) {
                    statusPassword = INIT_SYSTEM;
                }
                else {
                    user_cur_page--;
//                    if (user_cur_page < 0) user_cur_page = 0;
                    reset_smol_package();
                }
            }
            if (isButtonEnter() == 1) {
                statusPassword = user_page[user_cur_page];
                reset_smol_package();
            }
            if (isButtonNumber()) timeDelay = 0;
            
            if (timeDelay > 300) {      
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
            if (isButtonNext() && indexOfNumber >= PASSWORD_LENGTH) {
                statusPassword = APPLY_NEW_PASS;
                reset_package();
            }
            if (isButtonBack()) {
                statusPassword = USER_DASHBOARD;
                reset_smol_package();
            }
            if (timeDelay > 300) {
                statusPassword = INIT_SYSTEM;
            }
            break;
        case ADMIN_DASHBOARD:
            timeDelay++;
            switch(ad_current_page) {
                case 0:
                    LcdPrintLineS(0, "  1.OPEN DOOR  ");
//                    LcdPrintLineS(1, "       @      >");
                    LcdPrintCharS(1,0, LONG_LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 1:
                    LcdPrintLineS(0, "  2.CHANGE PW  ");
//                    LcdPrintLineS(1, "<      @      >");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 2:
                    LcdPrintLineS(0, " 3.MANAGE USER ");
//                    LcdPrintLineS(1, "<      @      >");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 3:
                    LcdPrintLineS(0, "  4.CHECK IN   ");
//                    LcdPrintLineS(1, "<      @      >");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 4:
                    LcdPrintLineS(0, "    5.TIME     ");
//                    LcdPrintLineS(1, "<      @       ");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    break;
                default:
                    break;
            } 
            
            if (isButtonNext()) {
//                ad_current_page = (ad_current_page + 1) % ADMIN_NUM_OF_PAGES;
                ad_current_page++;
                if (ad_current_page >= ADMIN_NUM_OF_PAGES) 
                    ad_current_page = ADMIN_NUM_OF_PAGES - 1;
                reset_smol_package();
            }
            if (isButtonBack()) {
                if (ad_current_page <= 0) {
                    statusPassword = INIT_SYSTEM;
                }
                else {
//                  ad_current_page = (ad_current_page - 1) % ADMIN_NUM_OF_PAGES;
                    ad_current_page--;
//                    if (ad_current_page < 0) ad_current_page = 0;
                    reset_smol_package();
                }
            }
            if (isButtonEnter() == 1) {
                statusPassword = admin_page[ad_current_page];
                reset_smol_package();
                ad_cur_mem_list = 0;        //list of member
                ad_mem_cur_page = 0;        //list of admin member management function
                checkin_cur_page = 0;
                time_cur_page = 0;
            }
//            if (key_code[1] == 1) {
//                statusPassword = admin_page[ad_current_page][1];
//                reset_package();
//            }
            
            if (isButtonNumber()) timeDelay = 0;
            
            if (timeDelay > 300) {      
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
            if (isButtonNext() && indexOfNumber >= PASSWORD_LENGTH) {
                statusPassword = APPLY_NEW_PASS;
                reset_package();
            }
            if (isButtonBack()) {
                statusPassword = ADMIN_DASHBOARD;
                reset_smol_package();
            }
            if (timeDelay > 300) {
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
                    LcdPrintLineS(0, " 1.ADD MEMBER  ");
//                    LcdPrintLineS(1, "       @      >");
                    LcdPrintCharS(1,0, LONG_LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 1:
                    LcdPrintLineS(0, "2.REMOVE MEMBER");
//                    LcdPrintLineS(1, "<      @      >");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 2:
                    LcdPrintLineS(0, "3.CHANGE MEM PW");
//                    LcdPrintLineS(1, "<      @       ");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    break;
                default:
                    break;
            } 
            
            if (isButtonNext()) {
//                ad_mem_cur_page = (ad_mem_cur_page + 1) % MANAGE_NUM_OF_PAGES;
                ad_mem_cur_page++;
                if (ad_mem_cur_page >= MANAGE_NUM_OF_PAGES)
                    ad_mem_cur_page = MANAGE_NUM_OF_PAGES - 1;
                reset_smol_package();
            }
            if (isButtonBack()) {
                if (ad_mem_cur_page <= 0) {
                    statusPassword = ADMIN_DASHBOARD;
                    reset_smol_package();
                }
                else {
                    ad_mem_cur_page--;
                    reset_smol_package();
                }
            }
            if (isButtonEnter() == 1) {
                statusPassword = manage_page[ad_mem_cur_page];
                reset_smol_package();     
                //cap nhat lai so trang cua member khi vao ham REMOVE_MEMBER hay CHANGE_MEMBER
                ad_num_member_list = (((num_of_user - 1) % 4 == 0) ? ((num_of_user - 1) / 4) : ((num_of_user - 1) / 4) + 1);
                ad_cur_mem_list = 0;        //list of member
            }
//            if (key_code[1] == 1) {
//                statusPassword = manage_page[ad_mem_cur_page][1];
//                reset_package();  
//                //cap nhat lai so trang cua member khi vao ham REMOVE_MEMBER hay CHANGE_MEMBER
//                ad_num_member_list = (((num_of_user - 1) % 4 == 0) ? ((num_of_user - 1) / 4) : ((num_of_user - 1) / 4) + 1);
//            }
            if (timeDelay > 300) {      
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
            if (isButtonNext() && indexOfNumber >= PASSWORD_LENGTH) {
                statusPassword = NEW_MEMBER_CREATED;
                reset_package();
            }
            if (isButtonBack()) {
                statusPassword = ADMIN_MEMBER_MANAGER;
                reset_smol_package();
            }
            if (timeDelay > 300) {      
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
            
            if (isButtonNext()) {
                if (ad_cur_mem_list < ad_num_member_list - 1) {
                    ad_cur_mem_list = (ad_cur_mem_list + 1);
                    LcdClearS();
                }
                timeDelay = 0;
            }
            if (isButtonBack()) {
                if (ad_cur_mem_list <= 0) {
                    statusPassword = ADMIN_MEMBER_MANAGER;
                    reset_smol_package();
                }
                else {
                    ad_cur_mem_list--;
                    timeDelay = 0;
                } 
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
            if (timeDelay > 300) {
                statusPassword = ADMIN_MEMBER_MANAGER;
                reset_package();
            }
            break;
        case CONFIRM_REMOVE_MEMBER:
            timeDelay++;
            LcdPrintStringS(0, 0, "REMOVE ID ");
            LcdPrintNumS(0, 10, account[ad_current_member].ID);
            LcdPrintCharS(0, 15, '?');
            LcdPrintStringS(1,0,"*:NO");
            LcdPrintStringS(1,8,"#:YES");
            
                
            
            if (isButtonNext()) {
                reset_package();
                statusPassword = REMOVE_COMPLETE;
            } 
            if (isButtonBack()) {
                reset_smol_package();
                statusPassword = ADMIN_REMOVE_MEMBER;
            }
            
            if (isButtonNumber()) timeDelay = 0;
            
            if (timeDelay > 300) {
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
            
            if (isButtonNext()) {
                if (ad_cur_mem_list < ad_num_member_list - 1) {
                    ad_cur_mem_list = (ad_cur_mem_list + 1);
                    LcdClearS();
                }
                timeDelay = 0;
            }
            if (isButtonBack()) {
                if (ad_cur_mem_list <= 0) {
                    statusPassword = ADMIN_MEMBER_MANAGER;
                    reset_smol_package();
                }
                else {
                    ad_cur_mem_list--;
                    timeDelay = 0;
                } 
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
            if (timeDelay > 300) {
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
            if (isButtonNext() && indexOfNumber >= PASSWORD_LENGTH) {
                statusPassword = APPLY_MEM_NEW_PASS;
                reset_package();
            }
            if (isButtonBack()) {
                statusPassword = ADMIN_CHANGE_MEMBER;
                reset_smol_package();
            }        
            if (timeDelay > 300) {
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
            
        //CHECK_IN session
        case CHECK_IN:
            timeDelay++;
            switch(checkin_cur_page) {
                case 0:
                    LcdPrintLineS(0, "  1.SUMMARY ");
//                    LcdPrintLineS(1, "       @      >");
                    LcdPrintCharS(1,0, LONG_LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 1:
                    LcdPrintLineS(0, " 2.PRESENT LIST");
//                    LcdPrintLineS(1, "<      @      >");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 2:
                    LcdPrintLineS(0, "  3.LATE LIST  ");
//                    LcdPrintLineS(1, "<      @      >");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 3:
                    LcdPrintLineS(0, " 4.ABSENT LIST ");
//                    LcdPrintLineS(1, "<      @      >");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 4:
                    LcdPrintLineS(0, "5.CLEAR CHECK IN");
//                    LcdPrintLineS(1, "<      @       ");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    break;
                default:
                    break;
            } 
            
            if (isButtonNext()) {
                checkin_cur_page++;
                if (checkin_cur_page >= CHECKIN_NUM_OF_PAGES) 
                    checkin_cur_page = CHECKIN_NUM_OF_PAGES - 1;
                reset_smol_package();
            }
            if (isButtonBack()) {
                if (checkin_cur_page <= 0) {
                    statusPassword = ADMIN_DASHBOARD;
                    reset_smol_package();
                }
                else {
                    checkin_cur_page--;
                    reset_smol_package();
                }
            }
            if (isButtonEnter() == 1) {
                statusPassword = checkin_page[checkin_cur_page];
                reset_smol_package();
                index_list_of_presence = 0;
            }
            if (isButtonNumber()) timeDelay = 0;
            
            if (timeDelay > 300) {      
                statusPassword = ADMIN_DASHBOARD;
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
                statusPassword = CHECK_IN;
                reset_smol_package();
            }
            if (timeDelay > 300) {
                statusPassword = INIT_SYSTEM;
            }
            break;
        case LIST_PRESENCE:
            timeDelay++;
            if (timeDelay == 1) {
                get_list_member_presence_status(HIEN_DIEN, &index_list_of_presence);
                display_list_member_presence_status();
            }
            if (isButtonNext()) {
                index_list_of_presence++;
                timeDelay = 0;
            }
            if (isButtonBack()) {
                statusPassword = CHECK_IN;
                reset_smol_package();
                index_list_of_presence = 0;
            }
            if (timeDelay > 300) {
                statusPassword = INIT_SYSTEM;
                reset_package();
                index_list_of_presence = 0;
            }
            break;
        case LIST_LATE:
            timeDelay++;
            if (timeDelay == 1) {
                get_list_member_presence_status(TRE, &index_list_of_late);
                display_list_member_presence_status();
            }
//            if (isButtonNext()) {
//                index_list_of_late++;
//                timeDelay = 0;
//            }
//            if (isButtonBack()) {
//                statusPassword = CHECK_IN;
//                reset_smol_package();
//                index_list_of_late = 0;
//            }
            if (isButtonNext()) {
                index_list_of_late++;
                timeDelay = 0;
            }
            if (isButtonBack()) {
                if (index_list_of_late <= 0) {
                    statusPassword = CHECK_IN;
                    index_list_of_late = 0;
                    reset_smol_package();
                }
                else {
                    index_list_of_late--;
                    timeDelay = 0;
                } 
            }
            if (timeDelay > 300) {
                statusPassword = INIT_SYSTEM;
                reset_package();
                index_list_of_late = 0;
            }
            break;
        case LIST_ABSENT:
            timeDelay++;
            if (timeDelay == 1) {
                get_list_member_presence_status(VANG, &index_list_of_absent);
                display_list_member_presence_status();
            }
            if (isButtonNext()) {
                index_list_of_absent++;
                timeDelay = 0;
            }
            if (isButtonBack()) {
                statusPassword = CHECK_IN;
                reset_smol_package();
                index_list_of_absent = 0;
            }
            if (timeDelay > 300) {
                statusPassword = INIT_SYSTEM;
                reset_package();
                index_list_of_absent = 0;
            }
            break;
        case RESET_CHECKIN:
            timeDelay++;
            LcdPrintLineS(0,"CLEAR CHECK IN");
            if (timeDelay == 1) {
                resetCheckin();
            }
            if (timeDelay >= 40) {
                statusPassword = CHECK_IN;
                reset_package();
            }
            break;
            
        //TIME session
        case TIME:
            timeDelay++;
            switch(time_cur_page) {
                case 0:
                    LcdPrintLineS(0, " 1.CHANGE TIME ");
//                    LcdPrintLineS(1, "       @      >");
                    LcdPrintCharS(1,0, LONG_LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    LcdPrintCharS(1,14,RIGHT_ARROW);
                    break;
                case 1:
                    LcdPrintLineS(0, "2.CHANGE L.TIME");
//                    LcdPrintLineS(1, "<      @       ");
                    LcdPrintCharS(1,0,LEFT_ARROW);
                    LcdPrintCharS(1,7,CENTER_NODE);
                    break;
                default:
                    break;
            } 
            
            if (isButtonNext()) {
                time_cur_page++;
                if (time_cur_page >= TIME_NUM_OF_PAGES) 
                    time_cur_page = TIME_NUM_OF_PAGES - 1;
                reset_smol_package();
            }
            if (isButtonBack()) {
                if (time_cur_page <= 0) {
                    statusPassword = ADMIN_DASHBOARD;
                    reset_smol_package();
                }
                else {
                    time_cur_page--;
                    reset_smol_package();
                }
            }
            if (isButtonEnter() == 1) {
                statusPassword = time_page[time_cur_page];
                reset_smol_package();
            }
            if (isButtonNumber()) timeDelay = 0;
            
            if (timeDelay > 300) {      
                statusPassword = ADMIN_DASHBOARD;
            }
            break;
        case CHANGE_TIME:
            timeDelay++;
            smolClock();
            if (isButtonNumber()) timeDelay = 0;
            
            if (isButtonBack()) {
                statusPassword = TIME;
                clockState = INIT_CLOCK;
                reset_smol_package();
            }
            
            if (timeDelay > 300) {
                statusPassword = INIT_SYSTEM;
                clockState = INIT_CLOCK;
            }
            break;
        case CHANGE_LATE_TIME:
            timeDelay++;
            smolLateClock();
            if (isButtonNumber()) timeDelay = 0;
            
            if (isButtonBack()) {
                statusPassword = TIME;
                clockState = INIT_CLOCK;
                reset_smol_package();
            }
            
            if (timeDelay > 300) {
                statusPassword = INIT_SYSTEM;
                clockState = INIT_CLOCK;
            }
            break;
            
        case UNLOCK_DOOR:
            
            timeDelay++;
            
            if (timeDelay == 1) {
                
                current_second = Read_DS1307(ADDRESS_SECOND);
                current_minute = Read_DS1307(ADDRESS_MINUTE);
                current_hour = Read_DS1307(ADDRESS_HOUR);
                
                if (current_hour > late_hour) {
                    account[current_user].checkin = TRE;
                }
                else if (current_hour == late_hour){
                    if (current_minute > late_minute) {
                        account[current_user].checkin = TRE;
                    }
                    else if (current_minute == late_minute) {
                        if (current_second > late_second) {
                            account[current_user].checkin = TRE;
                        }
                        else {
                            account[current_user].checkin = HIEN_DIEN;
                        }
                    }
                    else {
                        account[current_user].checkin = HIEN_DIEN;
                    }
                }
                else {
                    account[current_user].checkin = HIEN_DIEN;
                }
            }
            
            
//            if (timeDelay == 1) {
//                account[current_user].checkin = HIEN_DIEN;
//            }
            if (doorState == CLOSED) {
                LcdPrintStringS(0,0,"OPENING DOOR    ");
                UnlockDoor();
            }
            if (doorState == OPENED) {
                statusPassword = WAIT_DOOR;
                reset_package();
            }

            if (timeDelay >= 40) {
                statusPassword = WAIT_DOOR;
                reset_package();
//                disable_uart();
            }
            break;

        case WAIT_DOOR:
            timeDelay++;
            LcdPrintLineS(0,"DOOR WAITING");
            LcdPrintLineS(1,"PRESS # TO ENTER ID");
            
            DoorStop();
            doorState = OPENED;
            
            if (isButtonNext()) {
                reset_package();
                statusPassword = ENTER_ID;
                ID_value = 0;
            }
            
            if (timeDelay >= 100) {
                statusPassword = CLOSING_DOOR;
                reset_smol_package();
            }
            break;
        case CLOSING_DOOR:
            timeDelay++;
            LcdPrintLineS(0,"DOOR CLOSING");
            LockDoor();
            if (timeDelay >= 40) {
                statusPassword = INIT_SYSTEM;
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
