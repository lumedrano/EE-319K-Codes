// Lab5.c starter program EE319K Lab 5, Fall 2022
// Runs on TM4C123
// Luigi Medrano & Quan Vo
// Last Modified: 10/11/2022

/* Option A1, connect LEDs to PB5-PB0, switches to PA5-3, walk LED PF321
   Option A2, connect LEDs to PB5-PB0, switches to PA4-2, walk LED PF321
   Option A6, connect LEDs to PB5-PB0, switches to PE3-1, walk LED PF321
   Option A5, connect LEDs to PB5-PB0, switches to PE2-0, walk LED PF321
   Option B4, connect LEDs to PE5-PE0, switches to PC7-5, walk LED PF321
   Option B3, connect LEDs to PE5-PE0, switches to PC6-4, walk LED PF321
   Option B1, connect LEDs to PE5-PE0, switches to PA5-3, walk LED PF321
   Option B2, connect LEDs to PE5-PE0, switches to PA4-2, walk LED PF321
  */
// east/west red light connected to bit 5
// east/west yellow light connected to bit 4
// east/west green light connected to bit 3
// north/south red light connected to bit 2
// north/south yellow light connected to bit 1
// north/south green light connected to bit 0
// pedestrian detector connected to most significant bit (1=pedestrian present)
// north/south car detector connected to middle bit (1=car present)
// east/west car detector connected to least significant bit (1=car present)
// "walk" light connected to PF3-1 (built-in white LED)
// "don't walk" light connected to PF3-1 (built-in red LED)

#include <stdint.h>
#include "SysTick.h"
#include "Lab5grader.h"
#include "../inc/tm4c123gh6pm.h"


// put both EIDs in the next two lines
char EID1[] = "Lm45562"; //  ;replace abc123 with your EID
char EID2[] = "Qtv73"; //  ;replace abc123 with your EID


// create struct
struct state{
	uint32_t outE; //Output for our 6 LEDs at traffic light
	uint32_t outF; //Ouputs for our 3 walk LEDs
	uint32_t delay; // delay timer
	uint32_t next[8]; //skip the other 8 combos for that one state.
};
// create type def
typedef const struct state state_t;

// Defining all possible States
#define go_s 0
#define wait_s 1
#define stop_s 2
#define go_w 3
#define wait_w 4
#define stop_w 5
#define walk 6
#define red1 7
#define off1 8
#define red2 9
#define off2 10
#define stop_walk 11

state_t FSM[12] = {
//	 Format: {/*output1*/, /*output2*/, /*wait time*/, 
//          { /*next state if 0x00*/, /*next state if 0x01*/, … /*number of inputs*/ }},

	
	{0x21, 0x02, 2000, {wait_s, wait_s, go_s, wait_s, wait_s, wait_s, wait_s, wait_s}}, //go south
	
	{0x22, 0x02, 2000, {stop_s, stop_s, stop_s, stop_s, stop_s, stop_s, stop_s, stop_s}}, //wait south
	
	{0x24, 0x02, 2000, {go_w, go_w, go_s, go_w, walk, go_w, walk, go_w}}, //stop south
	
	{0x0C, 0x02, 2000, {wait_w, go_w, wait_w, wait_w, walk, wait_w, walk, wait_w}}, //go west
	
	{0x14, 0x02, 2000, {stop_w, stop_w, stop_w, stop_w, stop_w, stop_w, stop_w, stop_w}}, //wait west
	
	{0x24, 0x02, 2000, {walk, go_w, go_s, go_s, walk, walk, walk, walk}}, //stop west
	
	{0x24, 0x0E, 3500, {red1, red1, red1, red1, red1, red1, red1, red1}}, //walk
	
	{0x24, 0x02, 1000, {off1, off1, off1, off1, off1, off1, off1, off1}}, //red1 
	
	{0x24, 0x00, 1000,{red2, red2, red2, red2, red2, red2, red2, red2}}, //off1 
	
	{0x24, 0x02, 1000,{off2, off2, off2, off2, off2, off2, off2, off2}}, //red2	
	
	{0x24, 0x00, 1000,{stop_walk, stop_walk, stop_walk, stop_walk, stop_walk, stop_walk, stop_walk, stop_walk}}, // off2 
	
	{0x24, 0x02, 2000,{go_s, go_w, go_s, go_s, walk, go_w, go_s, go_s}}// STOP walk 
		
};

// Notes:
/*
- Challenge: Design the correct FSM
- SysTick: Given
- Between 10-20 States
- Follow the order: All output -> Delay -> All state transistion
- Make sure that we have the Correct # of input combination leaving each states

- *Improvement:
	- Change stop_s and stop_w -> all_stop -> Reduce # of states
	- [DONE] When there are 000 -> Loop back to the state [DONE] 


TM4C LED:
- LED order : GBR = Bit 3-2-1 on Port F
- White = xE (111)
- RED = x2	 (001)
- OFF = x0	 (000)

Questions:
- Why does Port E Clock doesn't turn on?
- Did we copy the SysTick code in our program yet?



*/



void DisableInterrupts(void);
void EnableInterrupts(void);


int main(void){ 
  TExaS_Init(GRADER);
  SysTick_Init();   // Initialize SysTick for software waits
  // initialize system

	// *Something wrong here? //
	SYSCTL_RCGCGPIO_R |= 0x34; //C E F clocks
	while((SYSCTL_RCGCGPIO_R&0x34) == 0){};
		
	GPIO_PORTE_DIR_R |= 0x3F; //Enables PE5-0 as outputs for connect LEDs
	GPIO_PORTE_DEN_R |= 0x3F; //enable pins


	GPIO_PORTC_DIR_R &= ~0x70; //makes PORTC selected bits inputs
	GPIO_PORTC_DEN_R |= 0x70;  //enable pins



	GPIO_PORTF_DIR_R |= 0x0E; //makes PORTF selected bits outputs for walk LEDs
	GPIO_PORTF_DEN_R |= 0x0E; //enable pins

	DisableInterrupts();
  EnableInterrupts(); 	

	uint32_t input;
	uint8_t S = go_s;
  while(1){
		
    // 1) output
		
			GPIO_PORTE_DATA_R = FSM[S].outE;
			GPIO_PORTF_DATA_R = FSM[S].outF;
		
		
    // 2) wait
		
			SysTick_Wait10ms(FSM[S].delay);
		
    // 3) input
		
			input = (GPIO_PORTC_DATA_R & 0x70)>>4;
		
    // 4) next

			S = FSM[S].next[input];
	}
}

/*	SysTick code from Textbook (Program 5.2 or Slide 7-25 in Lec7)

#define NVIC_ST_CTRL_R      (*((volatile uint32_t *)0xE000E010))

#define NVIC_ST_RELOAD_R    (*((volatile uint32_t *)0xE000E014))

#define NVIC_ST_CURRENT_R   (*((volatile uint32_t *)0xE000E018))

void SysTick_Init(void){

  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup

  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock

}

// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)

void SysTick_Wait(uint32_t delay){

  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait

  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears

  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag

  }

}

// 800000*12.5ns equals 10ms

// Call this routine to wait for delay*10ms

void SysTick_Wait10ms(uint32_t delay){

  uint32_t i;

  for(i=0; i<delay; i++){

    SysTick_Wait(800000);  // wait 10ms

  }

}


*/


// SysTick.c
// Runs on TM4C123
// Put your names here
// Last Modified: 8/21/2022








