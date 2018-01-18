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
#include "led.h"
#include "lcd.h"
#include "adc.h"
#include "timer_1ms.h"

#define LCD_MAX_COLUMN             16
#define LCD_DISPLAY_DATA_RAM_SIZE  80
#define LCD_LINE_DATA_RAM_SIZE     (LCD_DISPLAY_DATA_RAM_SIZE/2)
#define LCD_SHIFT_DATA_RAM_SIZE    LCD_LINE_DATA_RAM_SIZE - LCD_MAX_COLUMN

typedef struct {
    /* Arrays used for Explorer 16 LCD display */
    char messageLine1[LCD_DISPLAY_DATA_RAM_SIZE];
    char messageLine2[LCD_DISPLAY_DATA_RAM_SIZE];
} APP_DATA;

extern APP_DATA appData;