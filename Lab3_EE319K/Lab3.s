;****************** Lab3.s ***************
; Program written by: Luigi Medrano & Quan Vo
; Date Created: 2/4/2017
; Last Modified: 9/22/2022
; Brief description of the program
;   The LED toggles at 2 Hz and a varying duty-cycle
; Hardware connections (External: Two buttons and one LED)
;  Change is Button input  (1 means pressed, 0 means not pressed)
;  Breathe is Button input  (1 means pressed, 0 means not pressed)
;  LED is an output (1 activates external LED)
; Overall functionality of this system is to operate like this
;   1) Make LED an output and make Change and Breathe inputs.
;   2) The system starts with the the LED toggling at 2Hz,
;      which is 2 times per second with a duty-cycle of 30%.
;      Therefore, the LED is ON for 150ms and off for 350 ms.
;   3) When the Change button is pressed-and-released increase
;      the duty cycle by 20% (modulo 100%). Therefore for each
;      press-and-release the duty cycle changes from 30% to 70% to 70%
;      to 90% to 10% to 30% so on
;   4) Implement a "breathing LED" when Breathe Switch is pressed:
; PortE device registers
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_DEN_R   EQU 0x4002451C
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

        IMPORT  TExaS_Init
        THUMB
        AREA    DATA, ALIGN=2
;global variables go here
PE0    			   EQU 0x40024004 ; positive-logic button input 
PE3           	   EQU 0x40024020 ; positive-logic LED output
PE5		   		   EQU 0x40024080 
       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB
       EXPORT EID1
EID1   DCB "Lm45562",0  ;replace ABC123 with your EID
       EXPORT EID2
EID2   DCB "qtv73",0  ;replace ABC123 with your EID
       ALIGN 4

     EXPORT  Start

Start
; TExaS_Init sets bus clock at 80 MHz, interrupts, ADC1, TIMER3, TIMER5, and UART0
     MOV R0,#2	 ;0 for TExaS oscilloscope, 1 for PORTE logic analyzer, 2 for Lab3 grader, 3 for none
     BL  TExaS_Init ;enables interrupts, prints the pin selections based on EID1 EID2
 ; Your Initialization goes here
	LDR R0, =SYSCTL_RCGCGPIO_R; loads clock to activate
	LDR R1, [R0]
	ORR R1, R1, #0x10 ;turns on clock for PORT E
	STR R1, [R0]
	
	NOP
	NOP ;waits for clock to stabalize
	
	;define inputs and outputs
	LDR R0, =GPIO_PORTE_DIR_R
	LDR R1, [R0]
	ORR R1, #0x20; SET BIT 5
	BIC R1, #0x09 ; PE 0 & 3 inputs
	STR R1, [R0]
	
	;Digital enable outputs and inputs
	LDR R0, =GPIO_PORTE_DEN_R
	LDR R1, [R0]
	ORR R1, #0x29 ;ENABLES ALL BITS NEEDED TO BE ACTIVE
	STR R1, [R0]
	
	 
loop  
; main engine goes here
	LDR R0,=GPIO_PORTE_DATA_R			;collects data
	LDR R2, =7000000; 					count for off time of LED. Can be involked later.
	LDR R3, =3000000; 					subtraction value that can used throughout code (300 count) while on
	LDR R4, =9000000; 				; maximum count before resetting delay
	LDR R6, =2000000;				;will be the number to increment by	(20%)
	
	LDR R1, [R0]							; load data into R1
	AND R1, R1, #0x01						;isolate bit 0 for PE0
	CMP R1, #0x01							;compare value of PE0 to value 
	BEQ changepressed						;if (PE0 is 1 == 1 in 0th bit) then branch to changepressed function
	LDR R1, [R0]							;else(load data and check again)
	AND R1, R1, #0x08						;isolates data from pin PE3
	CMP R1, #0x08							;compares data from pin to value
	BEQ breathepressed						;if PE3 is a one (meaning pressed) == 1 value then branch to breathing funcyion instead.
	
	LDR R1, [R0]						;else run at standard hz going by on and off delay before looping back to check buttons pressed	
	ORR R1, #0x20 							;set bit PE5 to 1 to turn on LED.
	STR R1, [R0]							
delay_on_Orig									; start of delay when turned on if no button pressed
	SUBS R3, R3, #1							
	BNE delay_on_Orig								
	
	 LDR R1, [R0]							
	AND R1, R1, #0x01						
	CMP R1, #0x01							
	BEQ changepressed						; if PE0=1 the change button is pressed.
	LDR R1, [R0]							
	AND R1, R1, #0x08						
	CMP R1, #0x08							
	BEQ breathepressed						; if PE3=1, logic high. breathe button is pressed.
	
	LDR R1, [R0]							; load data into R1
	BIC R1, #0x20 							
	STR R1, [R0]							
delay_off_Orig								;start of delay when off		
	SUBS R2, R2, #1							
	BNE delay_off_Orig								
	B    loop								
changepressed					; start this iteration when the change button (PE0) is pressed			
	LDR R1, [R0]
	AND R1, R1, #0x01
	CMP R1, #0								 
	BEQ change_released			; checks to see if button PE0 is still pressed			  
	B changepressed							
change_released 					;if Change button is unpressed, branch to this subroutine.		
	LDR R1, [R0]								
	AND R1, R1, #0x08
	CMP R1, #0x08							
	BEQ breathepressed						
	SUB R5, R4, R3						
	CMP R5, #0								
	BEQ reset								
	ADD R3, R3, R6							
	SUB R2, R2, R6							
released								
	LDR R1, [R0]
	AND R1, R1, #0x08
	CMP R1, #0x08
	BEQ breathepressed						
	MOV R7, R3								
	MOV R8, R2
	LDR R1, [R0]
	AND R1, R1, #0x01
	CMP R1, #0x01
	BEQ changepressed						
	LDR R1, [R0]
	ORR R1, #0x20 							
	STR R1, [R0]							
delay_off_Release								;second delay before returning back to released loop	
	SUBS R7, R7, #1							
	BNE delay_off_Release
	
	LDR R1, [R0]
	BIC R1, #0x20 							
	STR R1, [R0]							
delay_on_Release									
	SUBS R8, R8, #1
	BNE delay_on_Release
	B released							
breathepressed								; goes through the different duty cycles that turn on and off the LED
	LDR R3, =0 								
	LDR R2, =200000						
	LDR R4, =190000 						
	LDR R6, =10000							
	LDR R9, =6 								
breatheincrease							; Increases the breathpressed by loading in the cycle numbers and checking pin value
	LDR R1, [R0]
	AND R1, R1, #0x01
	CMP R1, #0x01
	BEQ changepressed						 
	LDR R1, [R0]
	AND R1, R1, #0x08 
	CMP R1, #0								
	BEQ loop 								
	SUB R5, R4, R3 							
	CMP R5, #0
	BEQ breathedecrease					
	ADD R3, R3, R6							
	SUB R2, R2, R6							
	MOV R10, R9									
increment_repeat ; continues the cycles for as long as pressed
	MOV R7, R3								
	MOV R8, R2
	LDR R1, [R0]
	ORR R1, #0x20 							
	STR R1, [R0]	
delay_on_Breathe										;third delay for the repetition of breath cycle
	SUBS R7, R7, #1
	BNE delay_on_Breathe
	LDR R1, [R0]
	BIC R1, #0x20 							
	STR R1, [R0]
delay_off_Breathe								
	SUBS R8, R8, #1
	BNE delay_off_Breathe
	
	SUBS R10, R10, #1						
	BNE increment_repeat					
	B breatheincrease				
breathedecrease							;same as breathe increase but to decrement breathe cycle to turn off LED
	LDR R1, [R0]
	AND R1, R1, #0x08
	CMP R1, #0
	BEQ loop 								
	SUB R5, R4, R2 							
	CMP R5, #0
	BEQ breatheincrease				
	SUB R3, R3, R6							
	ADD R2, R2, R6							
	MOV R10, R9	
decrement_repeat						
	MOV R7, R3
	MOV R8, R2
	LDR R1, [R0]
	ORR R1, #0x20 							
	STR R1, [R0]
delay_on_BreatheD									
	SUBS R7, R7, #1
	BNE delay_on_BreatheD
	
	LDR R1, [R0]
	BIC R1, #0x20 							
	STR R1, [R0]
delay_off_BreatheD									
	SUBS R8, R8, #1
	BNE delay_off_BreatheD
	
	SUBS R10, R10, #1						
	BNE decrement_repeat					
	B breathedecrease					
reset										
	LDR R3, =1000000		;resets the value back to 1000000 after reaching 9000000				
	LDR R2, =9000000						
	B released
     ALIGN      ; make sure the end of this section is aligned
     END        ; end of file

