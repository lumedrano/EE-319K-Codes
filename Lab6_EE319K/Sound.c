// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 8/21/22 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "../inc/dac.h"
#include "../inc/tm4c123gh6pm.h"

volatile uint32_t Counts = 0;
	 uint8_t Index = 0;




// 6-bit 64-element sine wave				
			
const unsigned short wave[64] = {				
  32,35,38,41,44,47,49,52,54,56,58,				
  59,61,62,62,63,63,63,62,62,61,59,				
  58,56,54,52,49,47,44,41,38,35,				
  32,29,26,23,20,17,15,12,10,8,				
  6,5,3,2,2,1,1,1,2,2,3,				
  5,6,8,10,12,15,17,20,23,26,29};				
		

	
	

	
	
	

// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
// init systick 
//init flags, counts, indices
//start with quiet
	DAC_Init();
	Index = 0;
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CURRENT_R = 0;
	//NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000;
	
	
	//Heartbeat initializations
  SYSCTL_RCGCGPIO_R |= 0x20; //activate clock for Port F LED
	while((SYSCTL_PRGPIO_R & 0x20) == 0);
	GPIO_PORTF_DIR_R |= 0x0E; //set PF2 as an output
	GPIO_PORTF_DEN_R |= 0x0E; //enable PF2 Pin
	GPIO_PORTF_DATA_R |= 0x0E; //set bit to turn on PF2
}

// **************Sound_Start*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Start called again, or Sound_Off is called
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Start(uint32_t period){
//flag = 1; to play sound
	NVIC_ST_RELOAD_R = period-1;  //reloads value
	NVIC_ST_CTRL_R = 0x0007;
}


	
	
// **************Sound_Voice*********************
// Change voice
// EE319K optional
// Input: voice specifies which waveform to play
//           Pointer to wave table
// Output: none
void Sound_Voice(const uint8_t *voice){
  
}
// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
	//stops sounds
	NVIC_ST_CTRL_R = 0x0;

}
// **************Sound_GetVoice*********************
// Read the current voice
// EE319K optional
// Input: 
// Output: voice specifies which waveform to play
//           Pointer to current wavetable
const uint8_t *Sound_GetVoice(void){
  return 0; // replace
}
#define PF4 (*((volatile uint32_t *)0x40025040))
#define PF3 (*((volatile uint32_t *)0x40025020))
#define PF2 (*((volatile uint32_t *)0x40025010))
#define PF1 (*((volatile uint32_t *)0x40025008))
#define PF0 (*((volatile uint32_t *)0x40025004))

// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	DAC_Out(wave[Index]);
	Index = (Index+1)&0x3F;
	
	}
