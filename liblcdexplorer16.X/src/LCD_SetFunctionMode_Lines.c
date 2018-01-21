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

#ifdef LCD_PMP
#include "lcd_pmp.h"
#endif
#ifdef LCD_NO_PMP_8BIT
#include "lcd_no_pmp_8bit.h"
#endif
#ifdef LCD_NO_PMP_4BIT
#include "lcd_no_pmp_4bit.h"
#endif

void LCD_SetFunctionMode_Lines(LCD_REGs_st *LCD_REGs, bool twoLines) {
   LCD_REGs->FUNCTION_MODE.FUNCTION_MODEbits.N = twoLines;
   LCD_SendCommand(&(LCD_REGs->BF_AC), LCD_REGs->FUNCTION_MODE.REG);
}