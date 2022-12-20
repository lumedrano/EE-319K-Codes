// SpaceInvaders.c
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 12/01/2022 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php

// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "../inc/ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer1.h"



void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void DAC_Init(void);          // your lab 6 solution
void endScreen(void);
void DrawEnemy(void);
void rules(void);
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
uint32_t Data;        // 12-bit ADC
uint32_t score = 0;
uint8_t numenemy = 5;
uint8_t lives = 3;
uint8_t counter = 0;
uint8_t level = 1;
int language;
uint32_t Position, OldPosition, Flag;

//structs
struct PlayerShip{
    int32_t x;
	  uint8_t y;
    uint8_t alive; 
		uint8_t dead;
    int8_t lives;
    uint32_t score;
};
typedef struct PlayerShip Player;
Player p1;

struct Laser{
    uint32_t x;
	  uint8_t y;
		uint8_t linex;
    uint8_t exist;
		uint8_t lasermove;
};
typedef struct Laser Laser;
Laser beam;

struct Enemy{
    int32_t x;
	  int32_t y;
    uint8_t exist;
    uint32_t score;
};
typedef struct Enemy Enemy;
Enemy smallEnemy[5];




//30 Hz > 2*fmax
//low priority
void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
   // execute user task
	Data = ADC_In();//sample ADC for slide pot (SAC = 0)to move player
	OldPosition = Position;
	Position = ((127-18)* Data)/4095;
	Flag = 1;
}





uint8_t Collision(int8_t laserx, uint8_t lasery){
	for(int i = 0; i < numenemy; i++){
		if(smallEnemy[i].exist == 1){
			if((smallEnemy[i].x+12 >= laserx-2) && (smallEnemy[i].x <= laserx+2) && (smallEnemy[i].y+10 >= lasery)){
				score += smallEnemy[i].score;
				ST7735_DrawBitmap(smallEnemy[i].x, smallEnemy[i].y, EnemyBlack, 16, 10);
				smallEnemy[i].exist = 0;
				Sound_Killed();
				return(1);
			}
		}
	}
	return(0);
}


void Draw(void){ int32_t d;
	d = OldPosition-Position;
	//draw updated ship position
	if((d>2)|| (d<-2)){
	ST7735_DrawBitmap(OldPosition,159,PlayerBlack,18,8);
	ST7735_DrawBitmap(Position,159,PlayerShip0,18,8); //draw player ship
	}else{
		ST7735_DrawBitmap(Position,159,PlayerShip0,18,8);
	}
}


void moveEnemy(void){
	for(int i = 0; i < numenemy; i++){
		if(smallEnemy[i].exist == 1){
			if(i%2==0){
				if(counter % (5-level) == 0){
					ST7735_DrawBitmap(smallEnemy[i].x, smallEnemy[i].y, EnemyBlack, 16, 10);
					ST7735_DrawBitmap(smallEnemy[i].x, smallEnemy[i].y, SmallEnemy10pointB, 16,10);
					smallEnemy[i].y += 1;
					if(smallEnemy[i].y >= 149){
						for(int i = 0; i < numenemy; i++){
							ST7735_DrawBitmap(smallEnemy[i].x, smallEnemy[i].y, EnemyBlack, 16, 10);
							smallEnemy[i].exist = 0;
						}
						lives--;
						if(lives == 0){
							endScreen();
						} else {
							DrawEnemy();
						}
					}
				}
			} else {
			  if(counter % (4-level) == 0){
					ST7735_DrawBitmap(smallEnemy[i].x, smallEnemy[i].y, EnemyBlack, 16, 10);
					ST7735_DrawBitmap(smallEnemy[i].x, smallEnemy[i].y, SmallEnemy20pointA, 16,10);
					smallEnemy[i].y+= 1;
					if(smallEnemy[i].y >= 149){
						for(int i = 0; i < numenemy; i++){
							ST7735_DrawBitmap(smallEnemy[i].x, smallEnemy[i].y, EnemyBlack, 16, 10);
							smallEnemy[i].exist = 0;
						}
						lives--;
						if(lives == 0){
							endScreen();
						} else {
							DrawEnemy();
						}
					}
				}
			}
		}
	}
}






void createlaser(void){
	if(beam.exist == 0){
		ST7735_DrawBitmap(beam.x, beam.y, laser, 4,6);
		beam.linex = beam.x;
		beam.lasermove = 1;
		beam.exist = 1;
		Sound_Shoot();
	}
}

void laserF(void){
	if(beam.exist == 1){
		ST7735_DrawBitmap(beam.linex, p1.y-(7*beam.lasermove), blacklaser, 4,6);//7
		beam.lasermove++;
		ST7735_DrawBitmap(beam.linex, p1.y-(7*beam.lasermove+1), laser, 4,6);//7
		//If laser reaches top, remove existence
		if(beam.lasermove >= 23 || Collision(beam.linex, p1.y-(7*beam.lasermove+1))){
			ST7735_DrawBitmap(beam.linex, p1.y-(7*beam.lasermove+1), blacklaser, 4,6);
			beam.exist = 0;
		}
		beam.x = p1.x+6;
		beam.y = p1.y-7;
	} 
	beam.x = p1.x+6;
	beam.y = p1.y-7;
}

void DrawEnemy(void){
	for(int i = 0; i < numenemy; i++){
		smallEnemy[i].exist = 1;
		smallEnemy[i].x = 25*i + 5;
		smallEnemy[i].y = 19;
		if(i%2==0){
			ST7735_DrawBitmap(smallEnemy[i].x, 19, SmallEnemy10pointB, 16,10);
			smallEnemy[i].score = 10;
		} else {
			ST7735_DrawBitmap(smallEnemy[i].x, 19, SmallEnemy20pointA, 16,10);
			smallEnemy[i].score = 20;
		}
	}
}

void nextLevel(void){
	uint8_t enemyalive = 0;
	for(int i = 0; i < numenemy; i++){
		if(smallEnemy[i].exist == 1){
			enemyalive++;
		}
	}
	if(enemyalive == 0){
		level++;
		if(level > 3){
			endScreen();
		} else {
			DrawEnemy();
		}
	}
}

void PORTS_Init(void){
    SYSCTL_RCGCGPIO_R |= 0x10; 
    volatile uint32_t delay = SYSCTL_RCGCGPIO_R;
    GPIO_PORTE_DEN_R |= 0x03;
    GPIO_PORTE_AFSEL_R &= ~0x01;
    GPIO_PORTE_AMSEL_R &= ~0x01;
    GPIO_PORTE_DIR_R &= ~0x03;
    GPIO_PORTE_PCTL_R &= ~0x0000000F;
    GPIO_PORTE_IS_R &= ~0x01; 
    GPIO_PORTE_IBE_R &= ~0x01;
    GPIO_PORTE_IEV_R |= 0x01;
    GPIO_PORTE_ICR_R = 0x01;
    GPIO_PORTE_IM_R |= 0x01; 
    NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00) | 0x00000040; // Make priority 5
    NVIC_EN0_R = 0x00000010; 
}

void GPIOPortE_Handler(void) {
	GPIO_PORTE_ICR_R = 0x01;
	createlaser();
}

void SysTick_Init(uint32_t period){
	NVIC_ST_CTRL_R=0x00000000;
	NVIC_ST_RELOAD_R = period-1;
	NVIC_ST_CURRENT_R=0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; 
	NVIC_ST_CTRL_R=7;
}




// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
  volatile int delay;
  SYSCTL_RCGCGPIO_R |= 0x20;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTF_DIR_R |= 0x0E;
  GPIO_PORTF_DEN_R |= 0x0E;

}

void PromptScreen(void){
	language = 0;
	ST7735_FillScreen(0x0000);	// set screen to black
	ST7735_SetCursor(4, 1);
  ST7735_OutString("SPACE INVADERS"); //add enemy image to start and end of line
	//ST7735_DrawBitmap(52,48,space,45,45);
	ST7735_SetCursor(5, 2);
	ST7735_OutString("Luigi & Quan");
  ST7735_SetCursor(5, 7);
  ST7735_OutString("Press Red for");
	ST7735_SetCursor(7, 8);
  ST7735_OutString("English");
  ST7735_SetCursor(5, 10);
	ST7735_OutString("Presione verde");
	ST7735_SetCursor(6, 11);
	ST7735_OutString("para Espa\xA4ol");
	while((GPIO_PORTE_DATA_R & 0x01) != 0x01 && (GPIO_PORTE_DATA_R & 0x02) != 0x02){
		Delay100ms(4);
		while((GPIO_PORTE_DATA_R & 0x02) == 0x02){
				language = 1;
				break;	
		}
	}
	Delay100ms(4);
	rules();
	ST7735_FillScreen(0x0000);
	DrawEnemy();
	EnableInterrupts();
}

void rules(void){
	ST7735_FillScreen(0x0000);
	if(language == 0){
		ST7735_SetCursor(1, 1);
		ST7735_OutString("Red to shoot");
		ST7735_SetCursor(1, 3);
		ST7735_OutString("Green for pause");
		ST7735_SetCursor(3, 8);
		ST7735_OutString("Press Red Button");
		ST7735_SetCursor(4, 9);
		ST7735_OutString("to continue");
	}
	if(language == 1){
		ST7735_SetCursor(0,0);
		ST7735_OutChar(0x2A);
		ST7735_SetCursor(1, 0);
		ST7735_OutString("Presione roja");
		ST7735_SetCursor(0, 1);
		ST7735_OutString("para disparar");
		ST7735_SetCursor(0,3);
		ST7735_OutChar(0x2A);
		ST7735_SetCursor(1, 3);
		ST7735_OutString("Presione verde");
		ST7735_SetCursor(0, 4);
		ST7735_OutString("para pausar");
		ST7735_SetCursor(3, 9);
		ST7735_OutString("Presione roja");
		ST7735_SetCursor(3, 10);
		ST7735_OutString("para continuar");
	}
	while((GPIO_PORTE_DATA_R & 0x01) != 0x01){
	}
}

void endScreen(){
	DisableInterrupts();
	ST7735_FillScreen(0x0000);	// set screen to black
	if(language == 0){
		ST7735_SetCursor(6, 1);
		ST7735_OutString("GAME OVER");
		ST7735_SetCursor(4,4);
		ST7735_OutString("Your Score was");
		ST7735_SetCursor(9, 7);
		ST7735_OutUDec(score);
		ST7735_SetCursor(4, 10);
		ST7735_OutString("Press Red to");
		ST7735_SetCursor(5, 12);
		ST7735_OutString("Restart");
	}
	if(language == 1){
		ST7735_SetCursor(5, 1);
		ST7735_OutString("FIN DEL JUEGO");
		ST7735_SetCursor(1,4);
		ST7735_OutString("Tu puntuaci\xA2n fue");
		ST7735_SetCursor(9, 7);
		ST7735_OutUDec(score);
		ST7735_SetCursor(1, 10);
		ST7735_OutString("Presione roja para");
		ST7735_SetCursor(1, 11);
		ST7735_OutString("reiniciar");
	}
	// Vietnamese
//if(language == 2){
//		ST7735_SetCursor(6, 1);
//		ST7735_OutString("Ket Thuc");
//		ST7735_SetCursor(4,4);
//		ST7735_OutString("Diem cua ban la");
//		ST7735_SetCursor(9, 7);
//		ST7735_OutUDec(score);
//		ST7735_SetCursor(4, 10);
//		ST7735_OutString("Bam nut do de");
//		ST7735_SetCursor(5, 12);
//		ST7735_OutString("Choi lai");
// }
	while((GPIO_PORTE_DATA_R & 0x01) != 0x01){
	}
	score = 0;
	lives = 3;
	counter = 0;
	level = 1;
	PromptScreen();
}

void Pause(){
	while((GPIO_PORTE_DATA_R & 0x02) == 0x02){
		DisableInterrupts();
		while((GPIO_PORTE_DATA_R & 0x02) != 0x02){}
	}
	EnableInterrupts();
}

int main(void){
  DisableInterrupts();
	//Initializations
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
	Random_Init(1);
	Output_Init();
	//PLL_Init(Bus80MHz);
  ADC_Init();
	PORTS_Init();
	Sound_Init();
	PortF_Init();
	Sound_Init();
  PromptScreen();
	//p1.xpos = Position;
	p1.y = 159;
	beam.x = p1.x+6;
	beam.y = p1.y-7;
	Timer1_Init(80000000/30,5);
	beam.exist = 0;
	OldPosition = ((127-18)* Data)/4095;
	Flag = 0;
	EnableInterrupts();
  while(1){
			Pause();
			p1.x = Position;
			Draw();
			Flag = 1;
			moveEnemy();
			laserF();
			ST7735_SetCursor(0, 0);
			if (language == 0){
				ST7735_OutString("Score: ");
				ST7735_OutUDec(score);
				ST7735_SetCursor(11, 0);
				ST7735_OutString("Lives: ");
			}
			if (language == 1){
				ST7735_OutString("Puntaje:");
				ST7735_OutUDec(score);
				ST7735_SetCursor(14, 0);
				ST7735_OutString("Vida:");
			}
			ST7735_OutUDec(lives);
			nextLevel();
			counter++;
  }
}

// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
      time--;
    }
    count--;
  }
}








typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};





int main2(void){ char l;
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  for(phrase_t myPhrase=HELLO; myPhrase<= GOODBYE; myPhrase++){
    for(Language_t myL=English; myL<= French; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Delay100ms(30);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    Delay100ms(20);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }  
}



