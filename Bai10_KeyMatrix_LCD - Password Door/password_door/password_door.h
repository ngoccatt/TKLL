#ifndef PASSWORD_DOOR_H
#define PASSWORD_DOOR_H



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
#define     WAIT_DOOR               24
#define     CHANGE_TIME             25
#define     CHANGE_LATE_TIME        26

//Chuong trinh Password Door
#define PASSWORD_LENGTH 6
#define MAX_ACCOUNT     10
#define ERROR_RETURN    0xffff
#define CHAR_ERROR_RETURN 0xf
#define MAX_ID_LENGTH   3
#define ADMIN_NUM_OF_PAGES     4
#define MANAGE_NUM_OF_PAGES     2

//function prototype
unsigned int CheckID(unsigned int id);
void App_PasswordDoor();
unsigned char CheckPassword();
void displayID(int x, int y, unsigned int value, unsigned char indexOfID);
void reset_package();
void createlog();
void resetCheckin();
void UnlockDoor();
void LockDoor();
void DoorStop();
unsigned char isButtonNumber();
unsigned char isButtonEnter();
unsigned char isButtonBack();


#endif //PASSWORD_DOOR_H