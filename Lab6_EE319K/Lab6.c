// Lab6.c
// Runs on TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// ECE319K lab6 starter
// Program written by: Luigi Medrano and Quan Vo
// Date Created: 3/6/17 
// Last Modified: 8/21/22  
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
#include "Key.h"
#include "Music.h"
#include "Lab6Grader.h"


// Period =  80000000/64/Freq=1250000/Freq
#define AF0 3010   // 415.3 Hz
#define GF0 3378   // 370 Hz
#define EF0 4018   // 311.1 Hz
#define B7  5062   // 246.9 Hz

uint16_t note = 0;
uint32_t f_input = 0;

// put both EIDs in the next two lines
char EID1[] = "QTV73"; //  ;replace abc123 with your EID
char EID2[] = "Lm45562"; //  ;replace abc123 with your EID

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void DAC_Init(void);          // your lab 6 solution
void DAC_Out(uint8_t data);   // your lab 6 solution
uint8_t Testdata;

// lab video Lab6_voltmeter, Program 6.1
// A simple program that outputs sixteen DAC values. Use this main if you have a voltmeter.
const uint32_t Inputs[16]={0,1,7,8,15,16,17,18,31,32,33,47,48,49,62,63};
int voltmetermain(void){ uint32_t i;  
  DisableInterrupts();
  TExaS_Init(SCOPE);    
  LaunchPad_Init();
  DAC_Init(); // your lab 6 solution
  i = 0;
  EnableInterrupts();
  while(1){                
    Testdata = Inputs[i];
    DAC_Out(Testdata); // your lab 6 solution
    i=(i+1)&0x0F;  // <---put a breakpoint here
  }
}

// DelayMs
//  - busy wait n milliseconds
// Input: time to wait in msec
// Outputs: none
void static DelayMs(uint32_t n){
  volatile uint32_t time;
  while(n){
    time = 6665;  // 1msec, tuned at 80 MHz
    while(time){
      time--;
    }
    n--;
  }
}
// lab video Lab6_static. Program 6.2
// A simple program that outputs sixteen DAC values. 
// Use this main if you do not have a voltmeter. 
// Connect PD3 to your DACOUT and observe the voltage using TExaSdisplay in scope mode.
int staticmain(void){  
  uint32_t last,now,i;  
  DisableInterrupts();
  TExaS_Init(SCOPE);    // bus clock at 80 MHz
  LaunchPad_Init();
  DAC_Init(); // your lab 6 solution
  i = 0;
  EnableInterrupts();
  last = LaunchPad_Input();
  while(1){                
    now = LaunchPad_Input();
    if((last != now)&&now){
      Testdata = Inputs[i];
      DAC_Out(Testdata); // your lab 6 solution
      i=(i+1)&0x0F;
    }
    last = now;
    DelayMs(25);   // debounces switch
  }
}


     
int main(void){       
  DisableInterrupts();
  TExaS_Init(SIMULATIONGRADER);    // bus clock at 80 MHz
//    SCOPE,
//    LOGICANALYZERA,
//    LOGICANALYZERB,
//    LOGICANALYZERC,
//    LOGICANALYZERE,
//    REALBOARDGRADER,
//    SIMULATIONGRADER,
//    NONE
  Key_Init();
  LaunchPad_Init();
  Sound_Init();
  Music_Init();
	uint32_t breath = 100000;
  
	
	
  while(1){                
		
		breath--;
		if(breath == 0){
		GPIO_PORTF_DATA_R ^= 0x0E;
			breath = 100000;
		}
		
		EnableInterrupts();
		uint8_t Input = Key_In();
		
		if(Input == 0x01){
		Sound_Start(B7);}
		
		else if(Input == 0x02){
		Sound_Start(EF0);}
		
		else if(Input == 0x04){
		Sound_Start(GF0);}
		
		else if(Input == 0x08){
		Sound_Start(AF0);}
		
		else{Sound_Off();}
}

}



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
//#include <stdint.h>
//#include "../inc/dac.h"
//#include "../inc/tm4c123gh6pm.h"

//volatile uint32_t Counts = 0;
//	 uint8_t Index = 0;




//// 6-bit 64-element sine wave				
//			
//const unsigned short wave[64] = {				
//  32,35,38,41,44,47,49,52,54,56,58,				
//  59,61,62,62,63,63,63,62,62,61,59,				
//  58,56,54,52,49,47,44,41,38,35,				
//  32,29,26,23,20,17,15,12,10,8,				
//  6,5,3,2,2,1,1,1,2,2,3,				
//  5,6,8,10,12,15,17,20,23,26,29};				
//		

//	
//	

//	
//	
//	

//// **************Sound_Init*********************
//// Initialize digital outputs and SysTick timer
//// Called once, with sound/interrupts initially off
//// Input: none
//// Output: none
//void Sound_Init(void){
//// init systick 
////init flags, counts, indices
////start with quiet
//	DAC_Init();
//	Index = 0;
//	NVIC_ST_CTRL_R = 0;
//	NVIC_ST_CURRENT_R = 0;
//	//NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000;
//	
//	
//	//Heartbeat initializations
//  SYSCTL_RCGCGPIO_R |= 0x20; //activate clock for Port F LED
//	while((SYSCTL_PRGPIO_R & 0x20) == 0);
//	GPIO_PORTF_DIR_R |= 0x0E; //set PF2 as an output
//	GPIO_PORTF_DEN_R |= 0x0E; //enable PF2 Pin
//	GPIO_PORTF_DATA_R |= 0x0E; //set bit to turn on PF2
//}

//// **************Sound_Start*********************
//// Start sound output, and set Systick interrupt period 
//// Sound continues until Sound_Start called again, or Sound_Off is called
//// This function returns right away and sound is produced using a periodic interrupt
//// Input: interrupt period
////           Units of period to be determined by YOU
////           Maximum period to be determined by YOU
////           Minimum period to be determined by YOU
////         if period equals zero, disable sound output
//// Output: none
//void Sound_Start(uint32_t period){
////flag = 1; to play sound
//	NVIC_ST_RELOAD_R = period-1;  //reloads value
//	NVIC_ST_CTRL_R = 0x0007;
//}


//	
//	
//// **************Sound_Voice*********************
//// Change voice
//// EE319K optional
//// Input: voice specifies which waveform to play
////           Pointer to wave table
//// Output: none
//void Sound_Voice(const uint8_t *voice){
//  
//}
//// **************Sound_Off*********************
//// stop outputing to DAC
//// Output: none
//void Sound_Off(void){
//	//stops sounds
//	NVIC_ST_CTRL_R = 0x0;

//}
//// **************Sound_GetVoice*********************
//// Read the current voice
//// EE319K optional
//// Input: 
//// Output: voice specifies which waveform to play
////           Pointer to current wavetable
//const uint8_t *Sound_GetVoice(void){
//  return 0; // replace
//}
//#define PF4 (*((volatile uint32_t *)0x40025040))
//#define PF3 (*((volatile uint32_t *)0x40025020))
//#define PF2 (*((volatile uint32_t *)0x40025010))
//#define PF1 (*((volatile uint32_t *)0x40025008))
//#define PF0 (*((volatile uint32_t *)0x40025004))

//// Interrupt service routine
//// Executed every 12.5ns*(period)
//void SysTick_Handler(void){
//	DAC_Out(wave[Index]);
//	Index = (Index+1)&0x3F;
//	
//	}




// DAC.c
// This software configures DAC output
// Lab 6 requires 6 bits for the DAC
// Runs on TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 8/21/22 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

//#include <stdint.h>
//#include "../inc/tm4c123gh6pm.h"
//// Code files contain the actual implemenation for public functions
//// this file also contains an private functions and private data



//// **************DAC_Init*********************
//// Initialize 6-bit DAC, called once 
//// Input: none
//// Output: none
//void DAC_Init(void){
//  SYSCTL_RCGCGPIO_R |= 0x02; //activate Clock B
//	while((SYSCTL_RCGCGPIO_R & 0x02) == 0); //stabalize clock , NOP>>NOP
//	GPIO_PORTB_DR8R_R |= 0x3F;    // can drive up to 8mA out on PB3-0
//	GPIO_PORTB_DIR_R |= 0x3F; //Make PB5-0 outputs
//	GPIO_PORTB_DEN_R |= 0x3F; //Enables pins
//}

//// **************DAC_Out*********************
//// output to DAC
//// Input: 6-bit data, 0 to 63 
//// Input=n is converted to n*3.3V/63
//// Output: none
//void DAC_Out(uint32_t data){
//	GPIO_PORTB_PCTL_R &= ~0x00FFFFFF;
//	GPIO_PORTB_DATA_R = data;
//}



// Key.c
// This software configures the off-board piano keys
// Lab 6 requires a minimum of 4 keys, but you could have more
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 8/21/22  
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
//#include <stdint.h>
//#include "../inc/tm4c123gh6pm.h"

//// **************Key_Init*********************
//// Initialize piano key inputs on PA5-2 or PE3-0
//// Input: none 
//// Output: none
//void Key_Init(void){ volatile uint32_t delay;
//	SYSCTL_RCGCGPIO_R |= 0x10;
//	//SYSCTL_RCGCGPIO_R &= ~0x10;
//	while((SYSCTL_RCGCGPIO_R&0x10) == 0){};
//  GPIO_PORTE_DIR_R &= ~0x0F;         // 5) make PE3-0 in
//  GPIO_PORTE_DEN_R |= 0x0F;         // 7) enable digital I/O on PE3-0	
//}
//// **************Key_In*********************
//// Input from piano key inputs on PA5-2 or PE3-0
//// Input: none 
//// Output: 0 to 15 depending on keys
////   0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2, 0x08 is just Key3
//uint32_t key_input = 0;
//uint32_t Key_In(void){ 
//	//Read/Mask input keys
//	key_input = GPIO_PORTE_DATA_R & 0x0F;
//  return key_input;

//  
//}

////------------LaunchPad_Init------------
//// Initialize Switch input and LED output
//// Input: none
//// Output: none
//void LaunchPad_Init(void){
//// implement if needed
//}


////------------LaunchPad_Input------------
//// Input from Switches, 
//// Convert hardware negative logic to software positive logic 
//// Input: none
//// Output: 0x00 none
////         0x01 SW2 pressed (from PF4)
////         0x02 SW1 pressed (from PF1)
////         0x03 both SW1 and SW2 pressed
//uint8_t LaunchPad_Input(void){
//// implement if needed
//  return 0; // replace   
//}
////------------LaunchPad__Output------------
//// Output to LaunchPad LEDs 
//// Positive logic hardware and positive logic software
//// Input: 0 off, bit0=red,bit1=blue,bit2=green
//// Output: none
//void LaunchPad_Output(uint8_t data){  // write three outputs bits of PORTF
//// implement if needed

//}



