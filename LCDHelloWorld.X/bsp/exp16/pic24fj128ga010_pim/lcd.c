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

// <editor-fold defaultstate="collapsed" desc="Private variables">

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
        unsigned F : 1; // character font. F = 1 : 5x10 dots. F = 0 : 5X8 dots.
        unsigned N : 1; // Number of display line. N = 1 : DualLine. N =0 : SignalLine.
        unsigned DL : 1; // Data Length. DL = 1 : 8-Bit. DL = 0 : 4-Bit.
        unsigned : 3;
    };
} SET_FUNCTION_MODEbits = {.CMD = 0x3C};

static union {
    unsigned char CMD;

    struct {
        unsigned ADDR : 6;
        unsigned : 2;
    };
} SET_RAM_ADDRbits = {.CMD = 0x40};

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
// <editor-fold defaultstate="collapsed" desc="Private Functions Prototypes">

static inline void LCD_Send(uint16_t, char);
static inline char LCD_Recive(uint16_t);
static inline void LCD_WaitUntilPMPIsNotBusy(void);
static inline void LCD_WaitUntilLCDIsNotBusy(void);

// </editor-fold>

/* Private Definitions ***********************************************/
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

#define LCD_SendCommand(d) LCD_Send(0x0000, d)
#define LCD_SendData(d) LCD_Send(0x0001, d)
#define LCD_ReciveBusyAC() LCD_Recive(0x0000)
#define LCD_ReciveRamContent() LCD_Recive(0x0001)

// <editor-fold defaultstate="collapsed" desc="User Functions">

bool LCD_Initialize(void) {
    PMCON = 0x8383;
    PMMODE = 0x030C;
    PMAEN = 0x0001;

    /* LCD: Wait for more than 30ms after VDD on */
    __delay32(LCD_STARTUP);

    LCD_SendCommand(LCD_COMMAND_SET_FUNCTION_MODE);
    LCD_SendCommand(LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT);
    LCD_SendCommand(LCD_COMMAND_SET_ENTRY_MODE);

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
            LCD_SendCommand(LCD_COMMAND_SET_DD_RAM_ADDR);
            break;
        case '\n':
            LCD_ROW ^= 1;
            LCD_SendCommand(LCD_COMMAND_SET_DD_RAM_ADDR);
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

char LCD_GetChar(unsigned char address) {
    LCD_SendData(address);
    return LCD_ReciveRamContent();
}

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Primary Setters">

void LCD_ClearScreen(void) {
    LCD_SendCommand(LCD_COMMAND_CLEAR_SCREEN);
}

void LCD_ReturnHome(void) {
    LCD_SendCommand(LCD_COMMAND_RETURN_HOME);
}

void LCD_SetEntryMode(bool incdec, bool shift) {
    SET_ENTRY_MODEbits.S = shift;
    SET_ENTRY_MODEbits.ID = incdec;
    LCD_SendCommand(LCD_COMMAND_SET_ENTRY_MODE);
}

void LCD_DisplayCursorBlinkActivation(bool display, bool cursor, bool blink) {
    DISPLAY_CURSOR_BLINK_ACTbits.D = display;
    DISPLAY_CURSOR_BLINK_ACTbits.C = cursor;
    DISPLAY_CURSOR_BLINK_ACTbits.B = blink;
    LCD_SendCommand(LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT);
}

void LCD_ShiftDisplayMoveCursor(bool shiftDisplayCursor, bool rightLeft) {
    SHIFT_DISPLAY_MOVE_CURSORbits.SC = shiftDisplayCursor;
    SHIFT_DISPLAY_MOVE_CURSORbits.RL = rightLeft;
    LCD_SendCommand(LCD_COMMAND_SHIFT_DISPLAY_MOVE_CURSOR);
}

void LCD_SetFunctionMode(bool eightBitsDataLenght, bool twoLines, bool tenDots) {
    SET_FUNCTION_MODEbits.DL = eightBitsDataLenght;
    SET_FUNCTION_MODEbits.F = tenDots;
    SET_FUNCTION_MODEbits.N = twoLines;
    LCD_SendCommand(LCD_COMMAND_SET_FUNCTION_MODE);
}

void LCD_SetDDRAMAdrress(unsigned char address) {
    LCD_DD_RAM_ADDRESS = address;
    LCD_SendCommand(LCD_COMMAND_SET_DD_RAM_ADDR);
}

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Non Primary Setters">

void LCD_SetIncCursor_CursorMoviment(bool incdec) {
    SET_ENTRY_MODEbits.ID = incdec;
    LCD_SendCommand(LCD_COMMAND_SET_ENTRY_MODE);
}

void LCD_SetEntryMode_Shift(bool shift) {
    SET_ENTRY_MODEbits.S = shift;
    LCD_SendCommand(LCD_COMMAND_SET_ENTRY_MODE);
}

void LCD_DisplayCursorBlinkActivation_Display(bool display) {
    DISPLAY_CURSOR_BLINK_ACTbits.D = display;
    LCD_SendCommand(LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT);
}

void LCD_DisplayCursorBlinkActivation_Cursor(bool cursor) {
    DISPLAY_CURSOR_BLINK_ACTbits.C = cursor;
    LCD_SendCommand(LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT);
}

void LCD_DisplayCursorBlinkActivation_Blink(bool blink) {
    DISPLAY_CURSOR_BLINK_ACTbits.B = blink;
    LCD_SendCommand(LCD_COMMAND_DISPLAY_CURSOR_BLINK_ACT);
}

void LCD_SetFunctionMode_DataLenght(bool eightBitsDataLenght) {
    SET_FUNCTION_MODEbits.DL = eightBitsDataLenght;
    LCD_SendCommand(LCD_COMMAND_SET_FUNCTION_MODE);
}

void LCD_SetFunctionMode_Lines(bool twoLines) {
    SET_FUNCTION_MODEbits.N = twoLines;
    LCD_SendCommand(LCD_COMMAND_SET_FUNCTION_MODE);
}

void LCD_SetFunctionMode_Font(bool tenDots) {
    SET_FUNCTION_MODEbits.F = tenDots;
    LCD_SendCommand(LCD_COMMAND_SET_FUNCTION_MODE);
}

// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Private Functions">

static inline void LCD_WaitUntilLCDIsNotBusy(void) {
    do {
        LCD_ANSWER_BF_AC = LCD_ReciveBusyAC();
    } while (LCD_BUSY_FLAG);
}

static inline void LCD_WaitUntilPMPIsNotBusy(void) {
    while (PMMODEbits.BUSY);
}

static inline void LCD_Send(uint16_t address, char content) {
    LCD_WaitUntilPMPIsNotBusy();
    PMADDR = address;
    PMDIN1 = content;
    LCD_WaitUntilLCDIsNotBusy();
}

static inline char LCD_Recive(uint16_t address) {
    char dummy;
    LCD_WaitUntilPMPIsNotBusy();
    PMADDR = address;
    dummy = PMDIN1;
    LCD_WaitUntilPMPIsNotBusy();
    dummy = PMDIN1;
    return dummy;
}

// </editor-fold>
