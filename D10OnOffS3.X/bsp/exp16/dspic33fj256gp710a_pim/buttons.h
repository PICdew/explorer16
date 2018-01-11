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
#include <stdbool.h>

#ifndef BUTTONS_H
#define BUTTONS_H

typedef enum {
    BUTTON_DISABLED,
    BUTTON_S3,
    BUTTON_S6,
    BUTTON_S5,
    BUTTON_S4
    //S1 is MCLR
} BUTTON;

bool BUTTON_IsPressed(BUTTON button);
void BUTTON_Enable(BUTTON button);

#endif // BUTTONS_H