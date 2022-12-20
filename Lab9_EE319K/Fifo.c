// FiFo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 8/24/2022
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
#include <stdint.h>

// Declare state variables for FiFo
//        size, buffer, put and get indexes
#define FIFO_SIZE 9
static uint8_t PutI; //index for new
static uint8_t GetI;//index of old
static char Fifo[FIFO_SIZE];

// *********** FiFo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations

void Fifo_Init() {
PutI = GetI = 0; //sets both indexes to 0(empty)
}



//GET DECREMENTS A COUNTER
//PUT INCREMENTS A COUNTER
//ALLOWS FOR US TO KEEP TRACK OF FULL AND EMPTY INDEXES



// *********** FiFo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t Fifo_Put(char data) {
	
  if(((PutI+1)% FIFO_SIZE) == GetI){return 0;}//returns 0 if the input fails(full)
  Fifo[PutI] = data; //saves the data input into index [PutI] within Fifo array
	PutI = (PutI+1)%FIFO_SIZE;//increments index within PutI
   return(1);//if successfull then Fifo_Put returns a 1

}

// *********** Fifo_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t Fifo_Get(char *datapt)
{
  if(GetI == PutI){return 0;}//return a 0 if failure
  *datapt = Fifo[GetI]; //else retrieve data
	GetI = (GetI+1)%FIFO_SIZE; // increments place to get 
   return(1);//if successfull then returns a 1
}



