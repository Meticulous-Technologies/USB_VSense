/*
    [USB VSense] - [USB Voltage Indicator main firmware]
    Copyright (C) [2026] [Meticulous Technologies]

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/* ========================================================================== */
/* main.h USB Voltage indicator (USB VSense)                                  */
/* ========================================================================== */
/* Description: Main firmware (intended for production)                       */
/*                                                                            */
/* ========================================================================== */
/* Requires:  Modules: GPIO                                                   */
/*            main.c (part of main module)                                    */
/*                                                                            */
/* ========================================================================== */
/* author:              S. BHONWAL                                            */
/* company:             Meticulous Technologies                               */
/* library:                                                                   */
/* project:             005 USB Volt Indicator (USB VSense)                   */
/* hardware version     Beta, Gamma                                           */
/* version:             1                                                     */
/* revision:            0                                                     */
/* date:                13-09-2024                                            */
/* changes:                                                                   */
/* ========================================================================== */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <avr/io.h>

FUSES = {
	.WDTCFG = 0x00, // WDTCFG {PERIOD=OFF, WINDOW=OFF}
	.BODCFG = 0x00, // BODCFG {SLEEP=DIS, ACTIVE=DIS, SAMPFREQ=1KHz, LVL=BODLEVEL0}
	.OSCCFG = 0x7D, // OSCCFG {FREQSEL=16MHZ, OSCLOCK=CLEAR}
	.TCD0CFG = 0x00, // TCD0CFG {CMPA=CLEAR, CMPB=CLEAR, CMPC=CLEAR, CMPD=CLEAR, CMPAEN=CLEAR, CMPBEN=CLEAR, CMPCEN=CLEAR, CMPDEN=CLEAR}
	.SYSCFG0 = 0xF6, // SYSCFG0 {EESAVE=CLEAR, RSTPINCFG=UPDI, CRCSRC=NOCRC}
	.SYSCFG1 = 0xFF, // SYSCFG1 {SUT=64MS}
	.APPEND = 0x00, // APPEND {APPEND=User range:  0x0 - 0xFF}
	.BOOTEND = 0x00, // BOOTEND {BOOTEND=User range:  0x0 - 0xFF}
};

LOCKBITS = 0xC5; // {LB=NOLOCK}


#define F_CPU 32000UL

#endif	/* XC_HEADER_TEMPLATE_H */

