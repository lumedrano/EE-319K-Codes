// ****************** Lab2.c ***************
// Program written by: put your names here
// Date Created: 1/18/2017 
// Last Modified: 8/21/2022
#include "Lab2.h"
#define False 0
#define True 1
// Put your name and EID in the next two lines
char Name[] = "Luigi Medrano";
char EID[] = "lm45562";
// Brief description of the Lab: 
// An embedded system is capturing data from a
// sensor and performing analysis on the captured data.
//   The three analysis subroutines are:
//    1. Calculate the average error
//    2. Perform a linear equation using integer math 
//    3. Check if the captured readings are a monotonic series
// Possibility 1)
//       Return 1 if the readings are in non-increasing order.
//       Examples:
//         10,9,7,7,5,2,-1,-5 is True=1
//         2,2,2,2 is True=1
//         9,7,7,5,6,-1,-5 is False=0 (notice the increase 5 then 6)
//         3,2,1,0,1 is False (notice the increase 0 then 1)
// Possibility 2)
//       Return 1 if the readings are in non-decreasing order.
//       Examples:
//         -5,-1,2,5,7,7,9,10 is True=1
//         2,2,2,2 is True=1
//         -1,6,5,7,7,9,10 is False=0 (notice the decrease 6 then 5)
//         1,2,3,4,3 is False=0 (notice the decrease 4 then 3)
#include <stdint.h>

// Inputs: Data is an array of 32-bit signed measurements
//         N is the number of elements in the array
// Let x0 be the expected or true value
// Define error as the absolute value of the difference between the data and x0
// Output: Average error
// Notes: you do not need to implement rounding
// The value for your x0 will be displayed in the UART window


int32_t AverageError(const int32_t Readings[], const uint32_t N){
// Replace this following line with your solution
	int32_t c = 0;
	int32_t num = 209;
	
	for(int32_t i = 0; i < N; i++){
		c -= Readings[i]- num; //subtract num(209)x0 from each reading in the array
	}
	
	if(c < 0){
	c = c * -1; // if any number in the array is a negative number due to subtraction then mult. by -1 for abs.
	}

	int32_t avg_error = c/N; //divides the sum of all error values and divides by number of items in array.
   return avg_error; //returns the final value for average error.
}







// Consider a straight line between two points (x1,y1) and (x2,y2)
// Input: x an integer ranging from x1 to x2 
// Find the closed point (x,y) approximately on the line
// Output: y
// Notes: you do not need to implement rounding
// The values of (x1,y1) and (x2,y2) will be displayed in the UART window
int32_t Linear(int32_t const x){
// Replace this following line with your solution
	int32_t x1 = -100;
	int32_t x2 = 100;
	int32_t y1 = -70000;
	int32_t y2 = 70000;
	
	
  int32_t slope = (y2 - y1)/(x2-x1);//slope will be found by using all 4 variables and using the point slope formula.
	int32_t yint = y1 - (slope*x1); // the y intercept is found by plugging in one set of variables.
	
	int32_t y_final = (slope * x)+ yint; //the final y result is found by multiplying the slop by the constant x and adding the y intercept found.
	return y_final; //returns the final value for the x coordinate given.
}





// Return 1 or 0 based on whether the readings are a monotonic series
// Inputs: Readings is an array of 32-bit measurements
//         N is the number of elements in the array
// Output: 1 if monotonic, 0 if nonmonotonic
// Whether you need to implement non-increasing or non-decreasing will be displayed in the UART window
int IsMonotonic(int32_t const Readings[], uint32_t const N){
// Replace this following line with your solution
	for(int32_t i =0; i< N-1 ; i++){
		if( Readings[i] > Readings[i+1]){
			
  return False;
		}
	}
		return True;
}


