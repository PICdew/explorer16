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

#include "../../include/led.h"

void LED_OnOff(LED led, bool value) {
    switch (led) {
        case LED_D3:
            LED_D3_LAT = value;
            break;
        case LED_D4:
            LED_D4_LAT = value;
            break;
        case LED_D5:
            LED_D5_LAT = value;
            break;
        case LED_D6:
            LED_D6_LAT = value;
            break;
        case LED_D7:
            LED_D7_LAT = value;
            break;
        case LED_D8:
            LED_D8_LAT = value;
            break;
        case LED_D9:
            LED_D9_LAT = value;
            break;
        case LED_D10:
            LED_D10_LAT = value;
            break;
        case LED_NONE:
            break;
    }
}