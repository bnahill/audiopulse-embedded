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
  mov.w r0, 0x2000
  movt  r0, 0x4006
  movw r1, 0xC520
  movw r2, 0xD928
  strh r1, [r0, #14] /* Unlock 1 */
  strh r2, [r0, #14] /* Unlock 2 */
  mov.w r1, 0x01D2
  strh r1, [r0, #0]  /* Disable  */

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
 
