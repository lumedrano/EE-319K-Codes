// UART1.c
// Runs on LM4F120/TM4C123
// Use UART1 to implement bidirectional data transfer to and from 
// another microcontroller in Lab 9.  This time, interrupts and FIFOs
// are used.
// Daniel Valvano
// 8/24/2022

/* Lab solution, Do not post
 http://users.ece.utexas.edu/~valvano/
*/

// U1Rx (VCP receive) connected to PC4
// U1Tx (VCP transmit) connected to PC5
#include <stdint.h>
#include "Fifo.h"
#include "UART1.h"
#include "../inc/tm4c123gh6pm.h"


#define CR  0x0D

int RxCounter = 0;

// Initialize UART1
// Baud rate is 1000 bits/sec

void UART1_Init(void){
  // write this
	SYSCTL_RCGCUART_R |= 0x00000002;  // activate UART1

  SYSCTL_RCGCGPIO_R |= 0x00000004;  // activate port C

	while((SYSCTL_PRGPIO_R&0x00000004) == 0){}; //wait to activate clock
	
  UART1_CTL_R &= ~0x001;    // disable UART

  UART1_IBRD_R = 2500;     // IBRD = int(80,000,000/(16/2000)) = int(5000)

  UART1_FBRD_R = 0;     // FBRD = round(.000 * 64) = 0???

  UART1_LCRH_R = 0x00000070;  // 8 bit, no parity bits, one stop, FIFOs

	UART1_IM_R |= 0x10; //enable receiver	
		
  UART1_CTL_R |= 0x0301;     // enable UART

  GPIO_PORTC_AFSEL_R |= 0x30;    // enable alt funct on PC5-4


  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;
		
	GPIO_PORTC_DEN_R |= 0x30;      // configure PC5-4 as UART1
		
  GPIO_PORTC_AMSEL_R &= ~0x30;   // disable analog on PC5-4
	
	NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFF0FF) | 0x04000;
		
	NVIC_EN0_R |= 0x40;
	
	
}

//------------UART1_InChar------------
// Wait for new input, interrupt driven
// Use your FIFO
// then return ASCII code
// Input: none
// Output: char read from UART
char UART1_InChar(void){
  while((UART1_FR_R&0x0010) != 0);      // wait until RXFE is 0
  return((char)(UART1_DR_R&0xFF));
}




//------------UART1_InMessage------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until > is typed
//    or until max length of the string is reached.
// Input: pointer to empty buffer of 8 characters
// Output: Null terminated string
void UART1_InMessage(char *bufPt){
  // write this
	char data;
	uint32_t i = 0;
	while(data!=0x03){
		data = UART1_InChar();
		bufPt[i++] = data;
	}
}





//------------UART1_OutChar------------
// Output 8-bit to serial port
// Do not use interrupts or FIFO, use busy wait
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART1_OutChar(char data){
  while((UART1_FR_R&0x0020) != 0);      // wait until TXFF is 0
  UART1_DR_R = data;

}
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
// hardware RX FIFO goes from 7 to 8 or more items
// UART1 receiver has timed out
// Use your FIFO
void UART1_Handler(void){
	//char letter;				//uncomment when working on this function or else will give error for declaring and not using
  PF2 ^= 0x04;  // Heartbeat
  // write this
	while((UART1_FR_R & 0x10) == 0){
	Fifo_Put(UART1_DR_R & 0xFF);
	}

	UART1_ICR_R = 0x40; //what is this
	
	RxCounter++;
}

//------------UART1_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART1_OutString(char *pt){
  while(*pt){
    UART1_OutChar(*pt);
    pt++;
  }
}



