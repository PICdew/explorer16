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
#include "leds.h"
#include "lcd.h"
#include "adc.h"
#include "buttons.h"
#include "timer_1ms.h"
#include "print_lcd.h"

typedef struct {
    /* Variables used by Timer module */
    volatile unsigned char hours;
    volatile unsigned char minutes;
    volatile unsigned char seconds;
    volatile unsigned char lcd_update_flag;
    volatile unsigned char lcd_clear_flag;

    /* Arrays used for Explorer 16 LCD display */
    char messageLine1[18];
    char messageLine2[18];
} APP_DATA;

extern APP_DATA appData;