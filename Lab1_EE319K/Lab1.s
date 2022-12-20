;****************** Lab1.s ***************
; Program initially written by: Yerraballi and Valvano
; Author: Luigi Medrano
; Date Created: 1/15/2018 
; Last Modified: 9/7/2022
; Brief description of the program: Solution to Lab1
; The objective of this system is to implement a parity system
; Hardware connections: 
;  One output is positive logic, 1 turns on the LED, 0 turns off the LED
;  Three inputs are positive logic, meaning switch not pressed is 0, pressed is 1
GPIO_PORTD_DATA_R  EQU 0x400073FC
GPIO_PORTD_DIR_R   EQU 0x40007400
GPIO_PORTD_DEN_R   EQU 0x4000751C
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_DEN_R   EQU 0x4002451C
SYSCTL_RCGCGPIO_R  EQU 0x400FE608	
       PRESERVE8 
       AREA   Data, ALIGN=2
;Put following in RAM, aligned to 32-bits
; No global variables needed for Lab 1
       AREA    |.text|, CODE, READONLY, ALIGN=2
;Put following in ROM, aligned to 32-bits 
       THUMB
       EXPORT EID
EID    DCB "lm45562",0  ;replace abc123 with your EID
       EXPORT RunGrader
       ALIGN 4
RunGrader DCD 1 ; change to nonzero when ready for grading
           
      EXPORT  Lab1
Lab1 
;Initializations
	LDR R0, =SYSCTL_RCGCGPIO_R ; need to turn on clock once 
	LDR R1, [R0]
	ORR R1, R1, #0x10 ;turns on the clock for Port E
	STR R1, [R0]
	
	NOP
	NOP ; wait for clock to stabalize
	
	;CLOCK INIT IS DONE!

;define inputs and outputs (DIR)
	LDR R0, =GPIO_PORTE_DIR_R ;READ
	LDR R1, [R0] ; READ
	ORR R1, #0x20 ; sets bit 5
	BIC R1, #0x07 ;PE 0, 1, & 2 inputs
	STR R1, [R0] ;WRITE
	
;Digital Enable for inputs and outputs (DEN)
	LDR R0, =GPIO_PORTE_DEN_R 
	LDR R1, [R0] ;LOAD UNSIGNED 8-BITS 
	ORR R1, #0x27 ;SET BITS TO ENABLE SELECT PINS IN PORT E
	STR R1, [R0] ;STORE LEAST SIG. 8-BIT 
;start of the loop, all configurations need to be made to each pin before starting this.
loop
;input, calculate, output  
	LDR R0, =GPIO_PORTE_DATA_R
	LDR R1, [R0]
	AND R2, R1, #1 ; SELECTS PE0
	AND R3, R1, #2 ; SELECTS PE1
	AND R4, R1, #4 ; SELECTS PE2
	LSL R2, R2, #1 ;SHIFTS R2 TO LEFT BY 1
	EOR R2, R2, R3 ; TOGGLE R2 WITH R3
	LSL R2, R2, #1 ; SHIFT R2 TO THE LEFT ONCE 
	EOR  R2, R2, R4 ; TOGGLE R2 WITH R4
	EOR R2, #-1 ; flips bit with EOR in order to get odd parity.
	LSL R2, R2, #3 ; LEFT SHIFT R2 BY THREE BITS
	STR R2, [R0] ; end of program where all data is restored to give the result to LED before looping back.
    B    loop ;branch statement back to the top of loop in order to run until stopped. 
    ALIGN        ; make sure the end of this section is aligned
    END          ; end of file
               

