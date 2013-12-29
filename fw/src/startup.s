/*!
 (C) Copyright 2013, Ben Nahill
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 @file
 @brief
 @author Ben Nahill <bnahill@gmail.com>
 */

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb
 
.global earlyInitC


  .section .text.__reset_vector
  .weak __reset_vector
  .type __reset_vector, %function
__reset_vector:

  /* Disable WDT */
;  mov.w r0, 0x2000
;  movt  r0, 0x4005
;  movw r1, 0xC520
;  movw r2, 0xD928
;  strh r1, [r0, #14] /* Unlock 1 */
;  strh r2, [r0, #14] /* Unlock 2 */
;  ;mov.w r1, 0x01D2
;  mov.w r1, 0x4000
;  add r1, 0x10
;  strh r1, [r0, #0]  /* Disable  */

  bl earlyInitC      /* Go initialize low-level stuff */

  movs r1, #0
  b LoopCopyDataInit

 
CopyDataInit:
  ldr r3, =___ROM_AT
  ldr r3, [r3, r1]
  str r3, [r0, r1]
  adds r1, r1, #4
 
LoopCopyDataInit:
  ldr r0, =_sdata
  ldr r3, =_edata
  adds r2, r0, r1
  cmp r2, r3
  bcc CopyDataInit
  ldr r2, =__START_BSS
  b LoopFillZerobss

/* Zero fill the bss segment: */ 
FillZerobss:
  movs r3, #0
  str r3, [r2]
  adds r2, r2, #4
 
LoopFillZerobss:
  ldr r3, = __END_BSS
  cmp r2, r3
  bcc FillZerobss
/* Call the clock system intitialization function: */
  bl  __init_hardware
/* Call the application's entry point: */
  bl main
  bx lr
.size __reset_vector, .-__reset_vector
 
