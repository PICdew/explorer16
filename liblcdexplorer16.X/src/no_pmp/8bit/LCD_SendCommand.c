/* Samples for Explorer 16 Development Board of Microchip

   Copyright (C) 2018  Robert Buj <rbuj@fedoraproject.org>

   This file is part of explorer16 - https://github.com/rbuj/explorer16

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "lcd_no_pmp_8bit.h"

inline void LCD_SendCommand(BF_AC_u *BF_AC, char command) {
   LCD_RWSignal_Clear(); /* select write operation */
   LCD_RSSignal_Clear(); /* select instruction register */
   LCD_ConfigureDataOutput();
   LCD_WriteData(command);
   LCD_EnableSignal_Set();
   __delay32(18);
   LCD_EnableSignal_Clear();
   LCD_EnableSignal_Clear();
   LCD_ConfigureDataInput();

   /* Receive BF & AC */
   LCD_RWSignal_Set();   /* select read operation */
   LCD_RSSignal_Clear(); /* select BF/AC register */
   __delay32(18);
   do {
      LCD_EnableSignal_Set();
      __delay32(18);
      LCD_EnableSignal_Clear();
      BF_AC->REG = LCD_DATA_PORT & 0x00FF;
   } while (BF_AC->BF_ACbits.BF);
}