/* Explorer 16 Examples
 * Copyright (C) 2018  Robert Buj <rbuj@fedoraproject.org>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <xc.h>
#include <lcd.h>
#include <stdint.h> /* Includes uint8_t definition */
#include <libpic30.h> /* Includes __delay32 */

// <editor-fold defaultstate="collapsed" desc="Private Variables">

static union {
    unsigned char CMD;

    struct {
        unsigned S : 1; // S = 1 : Display Shift On. S = 0 : Display Shift Off.
        unsigned ID : 1; // ID = 1 : Increment. ID = 0 : Decrement.
        unsigned : 6;
    };
} SET_ENTRY_MODEbits = {.CMD = 0x04};

static union {
    unsigned char CMD;

    struct {
        unsigned B : 1; // Blink. B = 1 : Cursor Blink On. Blink. B = 0 : Cursor Blink Off.
        unsigned C : 1; // Cursor. C = 1 : Cursor Display On. C = 0 : Cursor Display Off.
        unsigned D : 1; // Display. D = 1 : Display On. D = 1 : Display Off.
        unsigned : 5;
    };
} DISPLAY_CURSOR_BLINK_ACTbits = {.CMD = 0x0C};

static union {
    unsigned char CMD;

    struct {
        unsigned : 2;
        unsigned RL : 1; // RL = 1 : Shift Right. RL = 0 : Shift Left.
        unsigned SC : 1; // SC = 1 : Shift Display. S/C = 0 : Move Cursor.
        unsigned : 4;
    };
} SHIFT_DISPLAY_MOVE_CURSORbits = {.CMD = 0x10};

static union {
    unsigned char CMD;

    struct {
        unsigned : 2;
        unsigned F : 1; // Character font. F = 1 : 5x10 dots. F = 0 : 5X8 dots.
        unsigned N : 1; // Number of display line. N = 1 : DualLine. N =0 : SignalLine.
        unsigned DL : 1; // Data Length. DL = 1 : 8-Bit. DL = 0 : 4-Bit.
        unsigned : 3;
    };
} SET_FUNCTION_MODEbits = {.CMD = 0x3C};

static union {
    unsigned char CMD;

    struct {
        unsigned COL : 4;
        unsigned BLK : 2;
        unsigned ROW : 1;
        unsigned : 1;
    };

    struct {
        unsigned OFFSET : 6;
        unsigned : 2;
    };

    struct {
        unsigned ADDR : 7;
        unsigned : 1;
    };
} SET_DD_RAM_ADDRbits = {.CMD = 0x80};

static union {
    unsigned char ANS;

    struct {
        unsigned AC : 7;
        unsigned BF : 1;
    };
} BF_ACbits;

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Private Function Prototypes">
static void LCD_SendData(char);
static void LCD_SendCommand(char, unsigned long);
// </editor-fold>

/* Private Definitions ***********************************************/
#define LCD_F_INSTR        160UL // Fast instruction delay = 40us -> cycles for __delay32
#define LCD_S_INSTR       6560UL // Slow instruction delay = 1.64ms -> cycles for __delay32
#define LCD_STARTUP     120000UL // Start up delay = 30ms -> cycles for __delay32

/* LCD Instructions **************************************************/
#define LCD_COMMAND_CLEAR_SCREEN              0x01
#define LCD_COMMAND_RETURN_HOME               0x02
#define LCD_COMMAND_SET_ENTRY_MODE            SET_ENTRY_MODEbits.CMD
#define LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT  DISPLAY_CURSOR_BLINK_ACTbits.CMD
#define LCD_COMMAND_SHIFT_DISPLAY_MOVE_CURSOR SHIFT_DISPLAY_MOVE_CURSORbits.CMD
#define LCD_COMMAND_SET_RAM_ADDR              SET_RAM_ADDRbits.CMD
#define LCD_COMMAND_SET_DD_RAM_ADDR           SET_DD_RAM_ADDRbits.CMD
#define LCD_COMMAND_SET_FUNCTION_MODE         SET_FUNCTION_MODEbits.CMD
#define LCD_ANSWER_BF_AC                      BF_ACbits.ANS

#define LCD_RAM_ADDRESS                       SET_RAM_ADDRbits.ADDR
#define LCD_DD_RAM_ADDRESS                    SET_DD_RAM_ADDRbits.ADDR
#define LCD_ROW                               SET_DD_RAM_ADDRbits.ROW
#define LCD_ACUMULATOR                        BF_ACbits.AC
#define LCD_BUSY_FLAG                         BF_ACbits.BF

#define LCD_DATA_LAT              LATE
#define LCD_DATA_TRIS             TRISE
#define LCD_DATA_PORT             PORTE
#define LCD_RSSignal_Set()        LATBbits.LATB15 = 1 //set Register Select bit
#define LCD_RSSignal_Clear()      LATBbits.LATB15 = 0 //clear Register Select bit
#define LCD_RWSignal_Set()        LATDbits.LATD5 = 1  //set Read/Write bit
#define LCD_RWSignal_Clear()      LATDbits.LATD5 = 0  //clear Read/Write bit
#define LCD_EnableSignal_Set()    LATDbits.LATD4 = 1  //set Enable bit
#define LCD_EnableSignal_Clear()  LATDbits.LATD4 = 0  //clear Enable bit
#define LCD_RSSignal_Input()      TRISBbits.TRISB15 = 1 //set Register Select bit
#define LCD_RSSignal_Output()     TRISBbits.TRISB15 = 0 //clear Register Select bit
#define LCD_RWSignal_Input()      TRISDbits.TRISD5 = 1  //set Read/Write bit
#define LCD_RWSignal_Output()     TRISDbits.TRISD5 = 0  //clear Read/Write bit
#define LCD_EnableSignal_Input()  TRISDbits.TRISD4 = 1  //set Enable bit
#define LCD_EnableSignal_Output() TRISDbits.TRISD4 = 0  //clear Enable bit
/* Configure PORTE<7:0> as outputs/ inpputs.
 * One instruction cycle is required between a port direction change.  */
#define LCD_WriteData(d)          LCD_DATA_LAT &= 0xFF00; LCD_DATA_LAT |= d
#define LCD_ConfigureDataOutput() LCD_DATA_TRIS &= 0xFF00; Nop()
#define LCD_ConfigureDataInput()  LCD_DATA_TRIS |= 0x00FF; Nop()

// <editor-fold defaultstate="collapsed" desc="User Functions">

char LCD_GetChar(unsigned char address) {
    return 0x00;
}

bool LCD_Initialize(void) {
    /* Initialize data pins to zero */
    LCD_WriteData(0x00);

    /* Initialize the control signal data pins */
    LCD_RWSignal_Clear();
    LCD_RSSignal_Clear();
    LCD_EnableSignal_Clear();

    /* Pin direction */
    LCD_ConfigureDataInput(); // Configure the data pins as input
    LCD_RSSignal_Output();
    LCD_RWSignal_Output();
    LCD_EnableSignal_Output();

    /* LCD: Wait for more than 30ms after VDD on */
    __delay32(LCD_STARTUP);

    LCD_SendCommand(LCD_COMMAND_SET_FUNCTION_MODE, LCD_F_INSTR);
    LCD_SendCommand(LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT, LCD_F_INSTR);
    LCD_SendCommand(LCD_COMMAND_SET_ENTRY_MODE, LCD_F_INSTR);

    LCD_ClearScreen();

    return true;
}

void LCD_PutString(char* inputString, uint16_t length) {
    while (length--) {
        switch (*inputString) {
            case 0x00:
                return;
            default:
                LCD_PutChar(*inputString++);
                break;
        }
    }
}

void LCD_PutChar(char inputCharacter) {
    switch (inputCharacter) {
        case '\r':
            if (LCD_ACUMULATOR >= 40) {
                SET_DD_RAM_ADDRbits.OFFSET = 0;
            } else {
                SET_DD_RAM_ADDRbits.OFFSET = 40;
            }
            LCD_SendCommand(LCD_COMMAND_SET_DD_RAM_ADDR, LCD_F_INSTR);
            break;
        case '\n':
            LCD_ROW ^= 1;
            LCD_SendCommand(LCD_COMMAND_SET_DD_RAM_ADDR, LCD_F_INSTR);
            break;
        case '\b':
            LCD_MoveCursor_Left();
            LCD_PutChar(' ');
            LCD_MoveCursor_Left();
            break;
        default:
            LCD_SendData(inputCharacter);
            break;
    }
}

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Primary Setters">

void LCD_ClearScreen(void) {
    LCD_SendCommand(LCD_COMMAND_CLEAR_SCREEN, LCD_S_INSTR);
}

void LCD_ReturnHome(void) {
    LCD_SendCommand(LCD_COMMAND_RETURN_HOME, LCD_S_INSTR);
}

void LCD_SetEntryMode(bool incdec, bool shift) {
    SET_ENTRY_MODEbits.S = shift;
    SET_ENTRY_MODEbits.ID = incdec;
    LCD_SendCommand(LCD_COMMAND_SET_ENTRY_MODE, LCD_F_INSTR);
}

void LCD_DisplayCursorBlinkActivation(bool display, bool cursor, bool blink) {
    DISPLAY_CURSOR_BLINK_ACTbits.D = display;
    DISPLAY_CURSOR_BLINK_ACTbits.C = cursor;
    DISPLAY_CURSOR_BLINK_ACTbits.B = blink;
    LCD_SendCommand(LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT, LCD_F_INSTR);
}

void LCD_ShiftDisplayMoveCursor(bool shiftDisplayCursor, bool rightLeft) {
    SHIFT_DISPLAY_MOVE_CURSORbits.SC = shiftDisplayCursor;
    SHIFT_DISPLAY_MOVE_CURSORbits.RL = rightLeft;
    LCD_SendCommand(LCD_COMMAND_SHIFT_DISPLAY_MOVE_CURSOR, LCD_F_INSTR);
}

void LCD_SetFunctionMode(bool eightBitsDataLenght, bool twoLines, bool tenDots) {
    SET_FUNCTION_MODEbits.DL = eightBitsDataLenght;
    SET_FUNCTION_MODEbits.F = tenDots;
    SET_FUNCTION_MODEbits.N = twoLines;
    LCD_SendCommand(LCD_COMMAND_SET_FUNCTION_MODE, LCD_F_INSTR);
}

void LCD_SetDDRAMAdrress(unsigned char address) {
    LCD_DD_RAM_ADDRESS = address;
    LCD_SendCommand(LCD_COMMAND_SET_DD_RAM_ADDR, LCD_F_INSTR);
}

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Non Primary Setters">

void LCD_SetIncCursor_CursorMoviment(bool incdec) {
    SET_ENTRY_MODEbits.ID = incdec;
    LCD_SendCommand(LCD_COMMAND_SET_ENTRY_MODE, LCD_F_INSTR);
}

void LCD_SetEntryMode_Shift(bool shift) {
    SET_ENTRY_MODEbits.S = shift;
    LCD_SendCommand(LCD_COMMAND_SET_ENTRY_MODE, LCD_F_INSTR);
}

void LCD_DisplayCursorBlinkActivation_Display(bool display) {
    DISPLAY_CURSOR_BLINK_ACTbits.D = display;
    LCD_SendCommand(LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT, LCD_F_INSTR);
}

void LCD_DisplayCursorBlinkActivation_Cursor(bool cursor) {
    DISPLAY_CURSOR_BLINK_ACTbits.C = cursor;
    LCD_SendCommand(LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT, LCD_F_INSTR);
}

void LCD_DisplayCursorBlinkActivation_Blink(bool blink) {
    DISPLAY_CURSOR_BLINK_ACTbits.B = blink;
    LCD_SendCommand(LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT, LCD_F_INSTR);
}

void LCD_SetFunctionMode_DataLenght(bool eightBitsDataLenght) {
    SET_FUNCTION_MODEbits.DL = eightBitsDataLenght;
    LCD_SendCommand(LCD_COMMAND_SET_FUNCTION_MODE, LCD_F_INSTR);
}

void LCD_SetFunctionMode_Lines(bool twoLines) {
    SET_FUNCTION_MODEbits.N = twoLines;
    LCD_SendCommand(LCD_COMMAND_SET_FUNCTION_MODE, LCD_F_INSTR);
}

void LCD_SetFunctionMode_Font(bool tenDots) {
    SET_FUNCTION_MODEbits.F = tenDots;
    LCD_SendCommand(LCD_COMMAND_SET_FUNCTION_MODE, LCD_F_INSTR);
}

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Private Functions">

static void LCD_SendData(char data) {
    LCD_RWSignal_Clear(); /* select write operation */
    LCD_RSSignal_Set(); /* select data register */
    LCD_ConfigureDataOutput();
    LCD_WriteData(data);
    LCD_EnableSignal_Set();
    __delay32(18);
    LCD_EnableSignal_Clear();
    LCD_RSSignal_Clear();
    LCD_ConfigureDataInput();

    // Receive BF & AC
    LCD_RWSignal_Set(); /* select read operation */
    LCD_RSSignal_Clear(); /* select BF/AC register */
    __delay32(18);
    do {
        LCD_EnableSignal_Set();
        __delay32(18);
        LCD_EnableSignal_Clear();
        LCD_ANSWER_BF_AC = LCD_DATA_PORT & 0x00FF;
    } while (LCD_BUSY_FLAG);
}

static void LCD_SendCommand(char command, unsigned long delay) {
    LCD_RWSignal_Clear(); /* select write operation */
    LCD_RSSignal_Clear(); /* select instruction register */
    LCD_ConfigureDataOutput();
    LCD_WriteData(command);
    LCD_EnableSignal_Set();
    __delay32(18);
    LCD_EnableSignal_Clear();
    LCD_EnableSignal_Clear();
    LCD_ConfigureDataInput();

    // Receive BF & AC
    LCD_RWSignal_Set(); /* select read operation */
    LCD_RSSignal_Clear(); /* select BF/AC register */
    __delay32(18);
    do {
        LCD_EnableSignal_Set();
        __delay32(18);
        LCD_EnableSignal_Clear();
        LCD_ANSWER_BF_AC = LCD_DATA_PORT & 0x00FF;
    } while (LCD_BUSY_FLAG);
}

// </editor-fold>
