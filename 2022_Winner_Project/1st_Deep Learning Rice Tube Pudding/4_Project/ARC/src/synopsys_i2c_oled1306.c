#include "synopsys_i2c_oled1306.h"

#define USE_SS_IIC_X USE_SS_IIC_0

const uint8_t OledFontTable[] =
{
        0x00, 0x00, 0x00, 0x00, 0x00,   // space
        0x00, 0x00, 0x2f, 0x00, 0x00,   // !
        0x00, 0x07, 0x00, 0x07, 0x00,   // "
        0x14, 0x7f, 0x14, 0x7f, 0x14,   // #
        0x24, 0x2a, 0x7f, 0x2a, 0x12,   // $
        0x23, 0x13, 0x08, 0x64, 0x62,   // %
        0x36, 0x49, 0x55, 0x22, 0x50,   // &
        0x00, 0x05, 0x03, 0x00, 0x00,   // '
        0x00, 0x1c, 0x22, 0x41, 0x00,   // (
        0x00, 0x41, 0x22, 0x1c, 0x00,   // )
        0x14, 0x08, 0x3E, 0x08, 0x14,   // *
        0x08, 0x08, 0x3E, 0x08, 0x08,   // +
        0x00, 0x00, 0xA0, 0x60, 0x00,   // ,
        0x08, 0x08, 0x08, 0x08, 0x08,   // -
        0x00, 0x60, 0x60, 0x00, 0x00,   // .
        0x20, 0x10, 0x08, 0x04, 0x02,   // /

        0x3E, 0x51, 0x49, 0x45, 0x3E,   // 0
        0x00, 0x42, 0x7F, 0x40, 0x00,   // 1
        0x42, 0x61, 0x51, 0x49, 0x46,   // 2
        0x21, 0x41, 0x45, 0x4B, 0x31,   // 3
        0x18, 0x14, 0x12, 0x7F, 0x10,   // 4
        0x27, 0x45, 0x45, 0x45, 0x39,   // 5
        0x3C, 0x4A, 0x49, 0x49, 0x30,   // 6
        0x01, 0x71, 0x09, 0x05, 0x03,   // 7
        0x36, 0x49, 0x49, 0x49, 0x36,   // 8
        0x06, 0x49, 0x49, 0x29, 0x1E,   // 9

        0x00, 0x36, 0x36, 0x00, 0x00,   // :
        0x00, 0x56, 0x36, 0x00, 0x00,   // ;
        0x08, 0x14, 0x22, 0x41, 0x00,   // <
        0x14, 0x14, 0x14, 0x14, 0x14,   // =
        0x00, 0x41, 0x22, 0x14, 0x08,   // >
        0x02, 0x01, 0x51, 0x09, 0x06,   // ?
        0x32, 0x49, 0x59, 0x51, 0x3E,   // @

        0x7C, 0x12, 0x11, 0x12, 0x7C,   // A
        0x7F, 0x49, 0x49, 0x49, 0x36,   // B
        0x3E, 0x41, 0x41, 0x41, 0x22,   // C
        0x7F, 0x41, 0x41, 0x22, 0x1C,   // D
        0x7F, 0x49, 0x49, 0x49, 0x41,   // E
        0x7F, 0x09, 0x09, 0x09, 0x01,   // F
        0x3E, 0x41, 0x49, 0x49, 0x7A,   // G
        0x7F, 0x08, 0x08, 0x08, 0x7F,   // H
        0x00, 0x41, 0x7F, 0x41, 0x00,   // I
        0x20, 0x40, 0x41, 0x3F, 0x01,   // J
        0x7F, 0x08, 0x14, 0x22, 0x41,   // K
        0x7F, 0x40, 0x40, 0x40, 0x40,   // L
        0x7F, 0x02, 0x0C, 0x02, 0x7F,   // M
        0x7F, 0x04, 0x08, 0x10, 0x7F,   // N
        0x3E, 0x41, 0x41, 0x41, 0x3E,   // O
        0x7F, 0x09, 0x09, 0x09, 0x06,   // P
        0x3E, 0x41, 0x51, 0x21, 0x5E,   // Q
        0x7F, 0x09, 0x19, 0x29, 0x46,   // R
        0x46, 0x49, 0x49, 0x49, 0x31,   // S
        0x01, 0x01, 0x7F, 0x01, 0x01,   // T
        0x3F, 0x40, 0x40, 0x40, 0x3F,   // U
        0x1F, 0x20, 0x40, 0x20, 0x1F,   // V
        0x3F, 0x40, 0x38, 0x40, 0x3F,   // W
        0x63, 0x14, 0x08, 0x14, 0x63,   // X
        0x07, 0x08, 0x70, 0x08, 0x07,   // Y
        0x61, 0x51, 0x49, 0x45, 0x43,   // Z

        0x00, 0x7F, 0x41, 0x41, 0x00,   // [
        0x55, 0xAA, 0x55, 0xAA, 0x55,   // Backslash (Checker pattern)
        0x00, 0x41, 0x41, 0x7F, 0x00,   // ]
        0x04, 0x02, 0x01, 0x02, 0x04,   // ^
        0x40, 0x40, 0x40, 0x40, 0x40,   // _
        0x00, 0x03, 0x05, 0x00, 0x00,   // `

        0x20, 0x54, 0x54, 0x54, 0x78,   // a
        0x7F, 0x48, 0x44, 0x44, 0x38,   // b
        0x38, 0x44, 0x44, 0x44, 0x20,   // c
        0x38, 0x44, 0x44, 0x48, 0x7F,   // d
        0x38, 0x54, 0x54, 0x54, 0x18,   // e
        0x08, 0x7E, 0x09, 0x01, 0x02,   // f
        0x18, 0xA4, 0xA4, 0xA4, 0x7C,   // g
        0x7F, 0x08, 0x04, 0x04, 0x78,   // h
        0x00, 0x44, 0x7D, 0x40, 0x00,   // i
        0x40, 0x80, 0x84, 0x7D, 0x00,   // j
        0x7F, 0x10, 0x28, 0x44, 0x00,   // k
        0x00, 0x41, 0x7F, 0x40, 0x00,   // l
        0x7C, 0x04, 0x18, 0x04, 0x78,   // m
        0x7C, 0x08, 0x04, 0x04, 0x78,   // n
        0x38, 0x44, 0x44, 0x44, 0x38,   // o
        0xFC, 0x24, 0x24, 0x24, 0x18,   // p
        0x18, 0x24, 0x24, 0x18, 0xFC,   // q
        0x7C, 0x08, 0x04, 0x04, 0x08,   // r
        0x48, 0x54, 0x54, 0x54, 0x20,   // s
        0x04, 0x3F, 0x44, 0x40, 0x20,   // t
        0x3C, 0x40, 0x40, 0x20, 0x7C,   // u
        0x1C, 0x20, 0x40, 0x20, 0x1C,   // v
        0x3C, 0x40, 0x30, 0x40, 0x3C,   // w
        0x44, 0x28, 0x10, 0x28, 0x44,   // x
        0x1C, 0xA0, 0xA0, 0xA0, 0x7C,   // y
        0x44, 0x64, 0x54, 0x4C, 0x44,   // z

        0x00, 0x10, 0x7C, 0x82, 0x00,   // {
        0x00, 0x00, 0xFF, 0x00, 0x00,   // |
        0x00, 0x82, 0x7C, 0x10, 0x00,   // }
        0x00, 0x06, 0x09, 0x09, 0x06    // ~ (Degrees)
};



/**************************************************************************************************
                                void OLED_Init()
 ***************************************************************************************************
 * I/P Arguments:  none
 * Return value : none

 * description  :This function is used to initialize the OLED in the normal mode.
                After initializing the OLED, It clears the OLED and sets the cursor to first line first position. .

 **************************************************************************************************/
void OLED_Init(void)
{ 
  oledSendCommand(0xa8);
  oledSendCommand(0x3f);
  oledSendCommand(0xd3);
  oledSendCommand(0x00);
  oledSendCommand(0x40);
	oledSendCommand(0xa1);
	oledSendCommand(0xc8);
	oledSendCommand(0xda);
	oledSendCommand(0x12);
	oledSendCommand(0x81);
	oledSendCommand(0x7f);
	oledSendCommand(0xa4);
	oledSendCommand(0xa6);
	oledSendCommand(0xd5);
	oledSendCommand(0x80);
	oledSendCommand(0x8d);
	oledSendCommand(0x14);
	oledSendCommand(0xaf);
}




/***************************************************************************************************
                       void OLED_DisplayChar( char ch)
 ****************************************************************************************************
 * I/P Arguments: ASCII value of the char to be displayed.
 * Return value    : none

 * description  : This function sends a character to be displayed on LCD.
                  Any valid ascii value can be passed to display respective character

 ****************************************************************************************************/
void OLED_DisplayChar(int8_t ch)
{
    uint8_t i=0;
    int index;

    if(ch!='\n') {  /* TODO */ 
        index = ch;
        index = index - 0x20;
        index = index * FONT_SIZE; // As the lookup table starts from Space(0x20)

        for(i = 0; i < FONT_SIZE; i ++)
            oledSendData(OledFontTable[index + i]); /* Get the data to be displayed for LookUptable*/

        oledSendData(0x00); /* Display the data and keep track of cursor */
    }
}

/***************************************************************************************************
                       void OLED_DisplayString(char *ptr_stringPointer_u8)
 ****************************************************************************************************
 * I/P Arguments: String(Address of the string) to be displayed.
 * Return value    : none

 * description  :
               This function is used to display the ASCII string on the lcd.
                 1.The ptr_stringPointer_u8 points to the first char of the string
                    and traverses till the end(NULL CHAR)and displays a char each time.

 ****************************************************************************************************/
void OLED_DisplayString(uint8_t *ptr)
{
    while(*ptr)
        OLED_DisplayChar(*ptr++);
}

/***************************************************************************************************
                void OLED_SetCursor(char v_lineNumber_u8,char v_charNumber_u8)
 ****************************************************************************************************
 * I/P Arguments: char row,char col
                 row -> line number(line1=1, line2=2),
                        For 2line LCD the I/P argument should be either 1 or 2.
                 col -> char number.
                        For 16-char LCD the I/P argument should be between 0-15.
 * Return value    : none

 * description  :This function moves the Cursor to specified position

                   Note:If the Input(Line/Char number) are out of range
                        then no action will be taken
 ****************************************************************************************************/
void OLED_SetCursor(uint8_t page, uint8_t cursorPosition)
{
  cursorPosition = cursorPosition;
  oledSendCommand(0x0f&cursorPosition);
	oledSendCommand(0x10|(cursorPosition>>4));
	oledSendCommand(0xb0|page);
}

/***************************************************************************************************
                         void OLED_Clear(void)
 ****************************************************************************************************
 * I/P Arguments: none.
 * Return value    : none

 * description  :This function clears the LCD and moves the cursor to beginning of first line
 ****************************************************************************************************/
void OLED_Clear(void)
{	
    uint8_t oled_clean_col , oled_clean_page;
	for(oled_clean_page = 0 ; oled_clean_page < 8 ; oled_clean_page++) {
        OLED_SetCursor(oled_clean_page,0);
		for(oled_clean_col= 0 ; oled_clean_col < 128 ; oled_clean_col ++) {
            oledSendData(0);
		}
	}
}

void OLED_Clear_half(char location)
{	
    uint8_t oled_clean_col , oled_clean_page;
    if(location == 'L')
        for(oled_clean_page = 0 ; oled_clean_page < 8 ; oled_clean_page++) {
            OLED_SetCursor(oled_clean_page,0);
            for(oled_clean_col= 0 ; oled_clean_col < 28 ; oled_clean_col ++) {
                oledSendData(0);
            }
        }
    else if(location == 'R')
        for(oled_clean_page = 0 ; oled_clean_page < 8 ; oled_clean_page++) {
            OLED_SetCursor(oled_clean_page, 100);
            for(oled_clean_col= 0 ; oled_clean_col < 28; oled_clean_col ++) {
                oledSendData(0);
            }
        }
}

/********************************************************************************
                Local FUnctions for sending the command/data
 ********************************************************************************/
void oledSendCommand(uint8_t cmd)
{
	uint8_t data_write[2];
	uint8_t data_read[2];
	data_write[0] = SSD1306_COMMAND;
	data_write[1] = cmd;

    hx_drv_i2cm_write_data(USE_SS_IIC_X, SSD1306_ADDRESS, &data_write[0], 1, &data_write[1], 1); 
}

int32_t oledSendData(uint8_t cmd)
{

	uint8_t data_write[2];
	uint8_t data_read[2];
	data_write[0] = SSD1306_DATA_CONTINUE;
	data_write[1] = cmd;

    int32_t test = hx_drv_i2cm_write_data(USE_SS_IIC_X, SSD1306_ADDRESS, &data_write[0], 0, &data_write[0], 2); 
    return test;
}
/********************************************************************************
                homemade function
 ********************************************************************************/
#define HeartNUM 22
uint8_t HeartLocation[22][2] = {
    {0, 47}, {0, 52}, {0, 72}, {0, 77}, {1, 42}, 
    {1, 57}, {1, 67}, {1, 82}, {2, 37}, {2, 62}, 
    {2, 87}, {3, 37}, {3, 87}, {4, 42}, {4, 82},
    {5, 47}, {5, 77}, {6, 52}, {6, 72}, {7, 57},
    {7, 62}, {7, 67}};

uint8_t HeartData = 0xff;

void DisplayHeart(void){
    for(int j = 0; j < HeartNUM; j++){
        OLED_SetCursor(HeartLocation[j][0], HeartLocation[j][1]);
        for(int i = 0; i < FONT_SIZE; i++)
            oledSendData(HeartData);
        oledSendData(0x00);
    }
}
#define TickNUM 5

uint8_t TickLocation[TickNUM][2] = {
    {4, 2}, {5, 7}, {4, 12}, {3, 17}, {2, 22}
};

uint8_t TickData = 0xff;


void DisplayTick(void){
    for(int j = 0; j < TickNUM; j++){
        OLED_SetCursor(TickLocation[j][0], TickLocation[j][1]);
        for(int i = 0; i < FONT_SIZE; i++)
            oledSendData(TickData);
        oledSendData(0x00);
    }
}

#define MarNUM 9

uint8_t MarkLocation[MarNUM][2] ={
    {2, 102}, {2, 122}, {3, 107}, {3, 117}, {4, 112},
    {5, 107}, {5, 117}, {6, 102}, {6, 122}
    };

uint8_t MarkData = 0xff;

void DisplayExclamationMark(void){
    for(int j = 0; j < MarNUM; j++){
        OLED_SetCursor(MarkLocation[j][0], MarkLocation[j][1]);
        for(int i = 0; i < FONT_SIZE; i++)
            oledSendData(MarkData);
        oledSendData(0x00);
    }
}

int8_t c = -2;
void DisplayResult(char result){
    switch (result)
    {
    case 'N':{
        // DisplayHeart();
        OLED_Clear_half('R');
        DisplayTick();
        break;
    }

    case 'Y':{
        // DisplayHeart();
        OLED_Clear_half('L');
        DisplayExclamationMark();
        break;
    }

    case 'R':{
        DisplayReady();
        break;
    }

    default:{
        OLED_Clear();
        DisplayHeart();
        break;
    }
    }
    if(c >= 0){
        OLED_SetCursor(7, 6);
        OLED_DisplayChar(c + '0');
    }
    c += 1;
    if (c == 10)
    {
        c = 0;
    }
}
#define ReadyNum 67
uint8_t ReadyLocation[ReadyNum][2] ={
    {1, 12}, {1, 17}, {1, 32}, {1, 52}, {1, 67}, {1, 72}, {1, 87}, {1, 102}, {1, 112}, {1, 117}, {1, 122}, ///11
    {2, 7}, {2, 22}, {2, 32}, {2, 52}, {2, 62}, {2, 77}, {2, 87}, {2, 97}, {2, 112}, //9
    {3, 7},  {3, 22},  {3, 32},  {3, 42},  {3, 52},  {3, 62},  {3, 77},  {3, 87},  {3, 92},  {3, 112},  {3, 117},  {3, 122}, //12
    {4, 7}, {4, 12}, {4, 17}, {4, 22}, {4, 32}, {4, 42}, {4, 52}, {4, 62}, {4, 67}, {4, 72}, {4, 77}, {4, 87}, {4, 97}, {4, 112}, //14
    {5, 7}, {5, 22}, {5, 32}, {5, 42}, {5, 52}, {5, 62}, {5, 77}, {5, 87}, {5, 102}, {5, 112}, //10
    {6, 7}, {6, 22}, {6, 37}, {6, 47}, {6, 62}, {6, 77}, {6, 87}, {6, 102}, {6, 112}, {6, 117}, {6, 122}, //11
    };

uint8_t ReadyData = 0xff;

void DisplayReady(void){
    for(int j = 0; j < ReadyNum; j++){
        OLED_SetCursor(ReadyLocation[j][0], ReadyLocation[j][1]);
        for(int i = 0; i < FONT_SIZE; i++)
            oledSendData(ReadyData);
        oledSendData(0x00);
    }
}

#define AHINum 44 - 15
uint8_t AHILocation[AHINum][2] = {
    {0, 17}, {0, 22},
    {1, 12}, {2, 12}, {3, 12}, {5, 12}, {6, 12},
    {1, 27}, {2, 27}, {3, 27}, {5, 27}, {6, 27},
    {4, 12}, {3, 17}, {3, 22}, {4, 27}, //16
    // {0, 37}, {1, 37}, {2, 37}, {3, 37}, {4, 37}, {5, 37}, {6, 37},
    // {0, 47}, {1, 47}, {2, 47}, {3, 47}, {4, 47}, {5, 47}, {6, 47},  
    // {3, 42},///15
    {0, 47}, {1, 47}, {2, 47}, {3, 47}, {4, 47}, {5, 47}, {6, 47},
    {6, 42}, {6, 52},
    {0, 42}, {0, 52},///11
    {2, 62}, {4, 62}//2
};
uint8_t AHIData = 0xff;

uint8_t NumLocation[15][2] = {
   {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5},
   {0, 10}, {1, 10}, {2, 10}, {3, 10}, {4, 10},
   {0, 15}, {1, 15}, {2, 15}, {3, 15}, {4, 15},
};

uint8_t NumData[10][15] ={
    {255, 255, 255, 255, 255, 255, 0, 0, 0, 255, 255, 255, 255, 255, 255,},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255,},
    {255, 0, 255, 255, 255, 255, 0, 255, 0, 255, 255, 255, 255, 0, 255,},
    {255, 0, 255, 0, 255, 255, 0, 255, 0, 255, 255, 255, 255, 255, 255,},
    {255, 255, 255, 0, 0, 0, 0, 255, 0, 0, 255, 255, 255, 255, 255,},
    {255, 255, 255, 0, 255, 255, 0, 255, 0, 255, 255, 0, 255, 255, 255,},
    {255, 255, 255, 255, 255, 255, 0, 255, 0, 255, 255, 0, 255, 255, 255,},
    {255, 0, 0, 0, 0, 255, 0, 0, 0, 0, 255, 255, 255, 255, 255,},
    {255, 255, 255, 255, 255, 255, 0, 255, 0, 255, 255, 255, 255, 255, 255,},
    {255, 255, 255, 0, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 255,},
};



void DisplayAI(int num){
    OLED_Clear();
    for(int j = 0; j < AHINum; j++){
        OLED_SetCursor(AHILocation[j][0], AHILocation[j][1] - 10);
        for(int i = 0; i < FONT_SIZE; i++)
            oledSendData(AHIData);
        oledSendData(0x00);
    }
    

    
   
    for(int k = 0; k < 3 ; k++){
        for(int j = 0; j < 15; j++){
        OLED_SetCursor(NumLocation[j][0] + 1, 107 + NumLocation[j][1] - k * 25);  //97-k*20
        for(int i = 0; i < FONT_SIZE; i++)
            if(k == 0)
                oledSendData(NumData[(num % 10)][j]);
            else if(k == 1)
                oledSendData(NumData[((num / 10) % 10)][j]);
            else
                oledSendData(NumData[(num / 100)][j]);

        oledSendData(0x00);
    }
    }
   
    OLED_SetCursor(5, 106); 
    for(int i = 0; i < FONT_SIZE - 3; i++)
        oledSendData(0xff);
}