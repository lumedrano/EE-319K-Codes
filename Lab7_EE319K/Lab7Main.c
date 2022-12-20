// Lab7Main.c
// Runs on TM4C123
// Test the functions in ST7735.c by printing basic
// patterns to the LCD.
//    16-bit color, 128 wide by 160 high LCD
// Daniel Valvano
// Last Modified: 8/24/2022
// Ramesh Yerraballi modified 3/20/2017

// hardware connections
// **********ST7735 TFT and SDC*******************
// ST7735
// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

// **********wide.hk ST7735R with ADXL345 accelerometer *******************
// Silkscreen Label (SDC side up; LCD side down) - Connection
// VCC  - +3.3 V
// GND  - Ground
// !SCL - PA2 Sclk SPI clock from microcontroller to TFT or SDC
// !SDA - PA5 MOSI SPI data from microcontroller to TFT or SDC
// DC   - PA6 TFT data/command
// RES  - PA7 TFT reset
// CS   - PA3 TFT_CS, active low to enable TFT
// *CS  - (NC) SDC_CS, active low to enable SDC
// MISO - (NC) MISO SPI data from SDC to microcontroller
// SDA  – (NC) I2C data for ADXL345 accelerometer
// SCL  – (NC) I2C clock for ADXL345 accelerometer
// SDO  – (NC) I2C alternate address for ADXL345 accelerometer
// Backlight + - Light, backlight connected to +3.3 V

// **********wide.hk ST7735R with ADXL335 accelerometer *******************
// Silkscreen Label (SDC side up; LCD side down) - Connection
// VCC  - +3.3 V
// GND  - Ground
// !SCL - PA2 Sclk SPI clock from microcontroller to TFT or SDC
// !SDA - PA5 MOSI SPI data from microcontroller to TFT or SDC
// DC   - PA6 TFT data/command
// RES  - PA7 TFT reset
// CS   - PA3 TFT_CS, active low to enable TFT
// *CS  - (NC) SDC_CS, active low to enable SDC
// MISO - (NC) MISO SPI data from SDC to microcontroller
// X– (NC) analog input X-axis from ADXL335 accelerometer
// Y– (NC) analog input Y-axis from ADXL335 accelerometer
// Z– (NC) analog input Z-axis from ADXL335 accelerometer
// Backlight + - Light, backlight connected to +3.3 V

// **********HiLetgo ST7735 TFT and SDC (SDC not tested)*******************
// ST7735
// LED-   (pin 16) TFT, connected to ground
// LED+   (pin 15) TFT, connected to +3.3 V
// SD_CS  (pin 14) SDC, chip select
// MOSI   (pin 13) SDC, MOSI
// MISO   (pin 12) SDC, MISO
// SCK    (pin 11) SDC, serial clock
// CS     (pin 10) TFT, PA3 (SSI0Fss)
// SCL    (pin 9)  TFT, SCK  PA2 (SSI0Clk)
// SDA    (pin 8)  TFT, MOSI PA5 (SSI0Tx)
// A0     (pin 7)  TFT, Data/Command PA6 (GPIO), high for data, low for command
// RESET  (pin 6)  TFT, to PA7 (GPIO)
// NC     (pins 3,4,5) not connected
// VCC    (pin 2)  connected to +3.3 V
// GND    (pin 1)  connected to ground

// Tyenaza Tyenazaqhf72mi9s3
// ST7735
// LED (pin 8) connected to +3.3 V
// SCK (pin 7) connected to PA2 (SSI0Clk)
// SDA (pin 6) MOSI, connected to PA5 (SSI0Tx)
// A0  (pin 5) Data/Command connected to PA6 (GPIO), high for data, low for command
// RESET (pin 4) connected to PA7 (GPIO)
// CS  (pin 3) connected to PA3 (SSI0Fss)
// Gnd (pin 2) connected to ground
// VCC (pin 1) connected to +3.3 V

#include <stdio.h>
#include <stdint.h>
#include "../inc/ST7735.h"
#include "../inc/PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/IO.h"
#include "../inc/StringConversion.h"
#include "images.h"
#include "Lab7Grader.h"
// Modify these two with your EIDs
char EID1[]="LM45562";  // student's EID
char EID2[]="QTV73";  // student's EID
#define SIZE 16
uint32_t const TestData[SIZE] ={
  0,7,99,100,654,999,1000,5009,9999,10000,20806,65535,
  103456,12000678,123400009,0xFFFFFFFF
};


int main(void){  
  uint32_t i;
  PLL_Init(Bus80MHz);    
  TExaS_Init(GRADER); // set system clock to 80 MHz
  // 1) implement SPIOutCommand and SPIOutData
  ST7735_InitR(INITR_REDTAB);
  ST7735_OutString("Lab 7 Fall 2022\n\xADHola!\nBienvenida al EE319K");
	// 2) remove lines the next two lines if the ST7735_OutString outputs correctly
//  while(1){
//  }
  IO_Init();
	IO_Touch();
  ST7735_FillScreen(0xFFFF);   // set screen to white
  ST7735_DrawBitmap(44, 159, Logo, 40, 160);
  IO_Touch();
  ST7735_FillScreen(0);       // set screen to black
  for(i=0;i<SIZE;i++){
    IO_HeartBeat();
    ST7735_SetCursor(0,i);
    ST7735_OutUDec(TestData[i]);
    ST7735_SetCursor(11,i);
    ST7735_OutFix(TestData[i]);
   IO_Touch(); // remove this line to see all test cases
  }
  while(1){
  }
}


//; StringConversion.s
//; Student names: change this to your names or look very silly
//; Last modification date: change this to the last modification date or look very silly
//; Runs on TM4C123
//; EE319K lab 7 number to string conversions
//;
//; Used in ECE319K Labs 7,8,9,10. You write these two functions

//    EXPORT   Dec2String
//    EXPORT   Fix2String
//		
//		


//    AREA    |.text|, CODE, READONLY, ALIGN=2
//    THUMB
//    PRESERVE8

//  
//;-----------------------Dec2String-----------------------
//; Convert a 32-bit number into unsigned decimal format
//; String the string into the empty array add null-termination
//; Input: R0 (call by value) 32-bit unsigned number
//;        R1 pointer to empty array
//; Output: none
//; Invariables: This function must not permanently modify registers R4 to R11
//Dec2String
//x		EQU 0 					;local variable
//	PUSH {R4-R11,LR}
//	;SUBS SP, #8					
//	
//	MOV R4, #0
//	PUSH {R0,R4}			;Allocation
//	MOV R11, SP	

//	CMP R0, #0					; input == 0?
//	BNE Stack_Access			
//	PUSH {R0}					
//	B Display
//	
//Stack_Access						
//	LDR R4, [R11,#x]
//	CMP R4, #0					; done? (data = 0)?
//	BEQ Display					
//	MOV R5, #10
//	UDIV R5, R4, R5				; x = num/10
//	STR R5, [R11,#x]			; store R5 into data offset

//	MOV R6, #10
//	MUL R5, R6					
//	SUB R4,R4,R5				
//	;MLS R4, R4, R5, R6			; mult and modulo
//	PUSH {R4}					; push remainder on stack
//	
//	B	Stack_Access
//loop ADD R1, R1, #1				;if not done popping go back here and increment pointer
//Display
//	POP {R0}					
//	ADD R0, #0x30				; convert to ASCII
//	STR R0, [R1]
//	CMP SP, R11					
//	BNE loop
//	ADD SP, #8					; DeAllocation
//	
//	POP {R4-R11,LR}			

//    BX LR



//;* * * * * * * * End of Dec2String * * * * * * * *


//; -----------------------Fix2String----------------------
//; Create characters for LCD display in fixed-point format
//; unsigned decimal, resolution 0.001, range 0.000 to 9.999
//; Inputs:  R0 is an unsigned 32-bit number
//;          R1 pointer to empty array
//; Outputs: none
//; E.g., R0=0,    then create "0.000 "
//;       R0=3,    then create "0.003"
//;       R0=89,   then create "0.089"
//;       R0=123,  then create "0.123"
//;       R0=9999, then create "9.999"
//;       R0>9999, then create "*.***"
//; Invariables: This function must not permanently modify registers R4 to R11
//Fix2String
//y EQU 0 ;local variable
//				; allocate space
//	PUSH { R4-R11, LR}			
//	
//	MOV R4, #0
//	PUSH {R0,R4}				; push R0 and zero
//	MOV R11, SP					; set r11 to pointer
//	
//	LDR R4, [R11, #y]
//	MOV R5, #1000	
//	UDIV R4,R5; input/1000 
//	CMP R4, #9		; check for overflow
//	BHI Asterisk
//	
//	ADD R4, #0x30		
//	MOV R0, R4
//	STR R0, [R1]	; display ones place value
//	
//	LDR R4, [R11, #y]
//	MOV R6, #1000
//	UDIV R5, R4, R6
//	MUL R5, R6
//	SUB R4,R4,R5			; calculate data%1000
//	STR R4, [R11, #y]	; store remainder in y
//	
//	MOV R0, #0x2E			; display decimal point
//	STR R0, [R1, #1] ;#1
//	
//	LDR R4, [R11, #y] 
//	MOV R6, #100			; data/100
//	UDIV R4,R6
//	ADD R4, #0x30			
//	MOV R0, R4
//	STR R0, [R1, #2]	;#2	; display tenths place value
//	
//	LDR R4, [R11, #y]
//	MOV R6, #100
//	UDIV R5, R4, R6
//	MUL R5, R6
//	SUB R4,R4,R5			; calculate data%100
//	STR R4, [R11, #y]	; store remainder 
//	
//	LDR R4, [R11, #y]	;divides input by 10
//	MOV R6, #10				
//	UDIV R4,R6		
//	ADD R4, #0x30			
//	MOV R0, R4
//	STR R0, [R1, #3]	;#3	 
//	
//	LDR R4, [R11, #y]
//	MOV R6, #10
//	UDIV R5, R4, R6
//	MUL R5, R6
//	SUB R4,R4,R5			; modulo
//	STR R4, [R11, #y] 

//	LDR R4, [R11, #y]	
//	ADD R4, #0x30			; convert to ASCII
//	MOV R0, R4
//	STR R0, [R1, #4]		
//	
//	MOV R0, #0x20
//	STR R0, [R1, #5]
//	B finish
//	
//Asterisk					
//	MOV R0, #0x2A
//	STR R0, [R1]		; *
//	MOV R0, #0x2E
//	STR R0, [R1, #1]		; .
//	MOV R0, #0x2A
//	STR R0, [R1, #2]		; *
//	MOV R0, #0x2A
//	STR R0, [R1, #3]		; *
//	MOV R0, #0x2A
//	STR R0, [R1, #4]		; *

//finish
//	ADD SP, #8				; de allocate space
//	POP {R4-R11, LR}		; restore registers
//    BX   LR
//     ALIGN                           ; make sure the end of this section is aligned
//     END                             ; end of file



// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  August 24, 2022
// Lab number: 7


//#include "../inc/tm4c123gh6pm.h"
//#include <stdint.h>


//void Wait10ms(uint32_t);

////------------IO_Init------------
//// Initialize GPIO Port for a switch and an LED
//// Input: none
//// Output: none
//void IO_Init(void) { volatile uint32_t delay;
// // --UUU-- Code to initialize PF4 and PF2
//	SYSCTL_RCGCGPIO_R |= 0x20;
//	while((SYSCTL_PRGPIO_R & 0x20) == 0);
//	GPIO_PORTF_PUR_R |= 0x10; //sets pull up resistor for PF4
//	GPIO_PORTF_DIR_R |= 0x0E; //set PF2 as an output
//	GPIO_PORTF_DIR_R &= ~0x10; //sets PF4 as an input
//	GPIO_PORTF_DEN_R |= 0x14; //enable PF2 and PF4 Pin
//}

////------------IO_HeartBeat------------
//// Toggle the output state of the LED.
//// Input: none
//// Output: none


//void IO_HeartBeat(void) {
// // --UUU-- PF2 is heartbeat
//	 uint32_t breath = 100000;
//		//stay in this function until breath = 0
//		//decrement breath until 0 and then toggle the light
//		
//		while(breath > 0){
//			breath--;
//		}
//		GPIO_PORTF_DATA_R ^= 0x0E;
//}


////------------IO_Touch------------
//// wait for release and press of the switch
//// Input: none
//// Output: none
//void IO_Touch(void) {
// // --UUU-- wait for release; delay for 20ms; and then wait for press
//	while((GPIO_PORTF_DATA_R & 0x10) == 0x10){}
//		Wait10ms(2);
//	while((GPIO_PORTF_DATA_R & 0x10) ==0){}
//	} 

////Must really wait for 10ms x count
//void Wait10ms(uint32_t count){
//  uint32_t i;
//  for(i=0; i <count*32000; i++){}
//}



//; BusyWait.s
//; Student names: change this to your names or look very silly
//; Last modification date: change this to the last modification date or look very silly

//; Runs on TM4C123
//; Use SPI to send an 8-bit code to the LCD.

//; As part of Lab 7, students need to implement these outcommand and outdata
//; This driver assumes two low-level LCD functions
//; this file is in the inc folder so it automatically will be applied to labs 7 8 9 and 10


//      EXPORT   SPIOutCommand
//      EXPORT   SPIOutData

//      AREA    |.text|, CODE, READONLY, ALIGN=2
//      THUMB
//      ALIGN
//; Used in ECE319K Labs 7,8,9,10. You write these two functions

//; ***********SPIOutCommand*****************
//; This is a helper function that sends an 8-bit command to the LCD.
//; Inputs: R0 = 32-bit command (number)
//;         R1 = 32-bit SPI status register address
//;         R2 = 32-bit SPI data register address
//;         R3 = 32-bit GPIO port address for D/C
//; Outputs: none
//; Assumes: SPI and GPIO have already been initialized and enabled
//; Note: must be AAPCS compliant
//; Note: access to bit 6 of GPIO must be friendly
//SPIOutCommand
//; --UUU-- Code to write a command to the LCD
//;1) Read the SPI status register (R1 has address) and check bit 4, 
//	PUSH {R0, R4-R11, LR}
//loop_command
//	LDR R4, [R1] ;gets information from R1
//	AND R4, #0x10 ;isolates bit 4
//;2) If bit 4 is high, loop back to step 1 (wait for BUSY bit to be low)
//	 CMP R4, #0x10
//	BEQ loop_command
//;3) Clear D/C (GPIO bit 6) to zero, be friendly (R3 has address)
//	LDR R4, [R3]
//	BIC R4, #0x40 ;clears bit 6 fiendly
//	STR R4, [R3]
//;4) Write the command to the SPI data register (R2 has address)
//	STR R0, [R2]
//;5) Read the SPI status register (R1 has address) and check bit 4,
//loop_command_2
//	LDR R4, [R1] ;gets information from R1
//	AND R4, R4, #0x10 ;isolates bit 4
//;6) If bit 4 is high, loop back to step 5 (wait for BUSY bit to be low)
//	 CMP R4, #0x10
//	BEQ loop_command_2
//	POP {R0, R4-R11, LR}
//    BX  LR             ;   return

//; ***********SPIOutData*****************
//; This is a helper function that sends an 8-bit data to the LCD.
//; Inputs: R0 = 32-bit data (number)
//;         R1 = 32-bit SPI status register address
//;         R2 = 32-bit SPI data register address
//;         R3 = 32-bit GPIO port address for D/C
//; Outputs: none
//; Assumes: SPI and GPIO have already been initialized and enabled
//; Note: must be AAPCS compliant
//; Note: access to bit 6 of GPIO must be friendly

//SPIOutData
//; --UUU-- Code to write data to the LCD
//;1) Read the SPI status register (R1 has address) and check bit 1, 
//	PUSH {R0, R4-R11, LR}
//loop_command_3
//	LDR R4, [R1] ;gets information from R1
//	AND R4, R4, #0x02 ;isolates bit 1
//;2) If bit 1 is low, loop back to step 1 (wait for TNF bit to be high)
//	CMP R4, #0x02
//	BNE loop_command_3
//;3) Set D/C (GPIO bit 6) to one, be friendly (R3 has address)
//	LDR R4, [R3]
//	ORR R4, #0x40 ;sets bit 6 fiendly
//	STR R4, [R3]
//;4) Write the data to the SPI data register (R2 has address)
//   STR R0, [R2]
//	POP {R0, R4-R11, LR}
//    BX  LR             ;return
//;****************************************************
//	
//    ALIGN                           ; make sure the end of this section is aligned
//    END                             ; end of file



