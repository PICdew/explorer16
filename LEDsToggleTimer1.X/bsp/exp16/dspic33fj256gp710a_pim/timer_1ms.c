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
#include <stdint.h>
#include <stddef.h>
#include <xc.h>
#include <string.h>
#include <timer_1ms.h>

/* Compiler checks and configuration *******************************/
#ifndef TIMER_MAX_1MS_CLIENTS
#define TIMER_MAX_1MS_CLIENTS 1
#endif

#define TMR1_PERIOD 0x8000

#define STOP_TIMER_IN_IDLE_MODE     0x2000
#define TIMER_SOURCE_INTERNAL       0x0000
#define TIMER_SOURCE_EXTERNAL       0x0002
#define TIMER_ON                    0x8000
#define GATED_TIME_DISABLED         0x0000
#define TIMER_16BIT_MODE            0x0000

#define TIMER_PRESCALER_1           0x0000
#define TIMER_PRESCALER_8           0x0010
#define TIMER_PRESCALER_64          0x0020
#define TIMER_PRESCALER_256         0x0030
#define TIMER_INTERRUPT_PRIORITY    0x0001
#define TIMER_INTERRUPT_PRIORITY_4  0x0004

bool TIMER_Configuration() {
    IPC0bits.T1IP = TIMER_INTERRUPT_PRIORITY_4; /* set priority level */
    IFS0bits.T1IF = 0; /* clear interrupt flag */
    TMR1 = 0; /* clear timer1 register */
    PR1 = TMR1_PERIOD; /* set period1 register */
    T1CON = TIMER_ON |
            TIMER_SOURCE_EXTERNAL |
            GATED_TIME_DISABLED |
            TIMER_16BIT_MODE |
            TIMER_PRESCALER_1;
    IEC0bits.T1IE = 1; /* enable interrupts */
    return true;
}