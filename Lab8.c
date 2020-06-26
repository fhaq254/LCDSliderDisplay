// Lab8.c
// Runs on LM4F120 or TM4C123
// Student names: Fawadul Haq and Rafael Herrejon
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 11/8/2017 

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats

#include <stdint.h>

#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is on Nokia

void DisableInterrupts(void);       // Disable interrupts
void EnableInterrupts(void);        // Enable interrupts
void SysTick_Init(uint32_t period); // Initialize systick interrupts
void SysTick_Handler(void);         // Systick Interrupt, samples ADC

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	//F
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PDR_R = 0x11;          // enable pull-down on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}

void SysTick_Init(uint32_t period){
  NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period; 
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
	NVIC_ST_CTRL_R = 0x00000007; //0111
}


uint32_t Data;        // 12-bit ADC
uint32_t ADCMail;     // input from Systick Handler sampling ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm
uint8_t flag = 0;         // ADC flag

void SysTick_Handler(void){
	PF3 ^= 0x08;
	PF3 ^= 0x08;
	ADCMail = ADC_In();
	flag = 1;
	PF3 ^= 0x08;
}


int main1(void){       // single step this program and look at Data
  TExaS_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 1
  while(1){                
    Data = ADC_In();  // sample 12-bit channel 1
  }
}

int main2(void){
  TExaS_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 1
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  while(1){           // use scope to measure execution time for ADC_In and LCD_OutDec           
    PF2 = 0x04;       // Profile ADC
    Data = ADC_In();  // sample 12-bit channel 1
    PF2 = 0x00;       // end of ADC Profile
    ST7735_SetCursor(0,0);
    PF1 = 0x02;       // Profile LCD
    LCD_OutDec(Data); 
    ST7735_OutString("    ");  // spaces cover up characters from last output
    PF1 = 0;          // end of LCD Profile
  }
}

uint32_t Convert(uint32_t input){
  return ((511*input)/1250)+263;    // write this your self
}
int main3(void){ 
  TExaS_Init();         // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB); 
  ADC_Init();         // turn on ADC, set channel to 1
	PortF_Init();
  while(1){  
    PF2 ^= 0x04;      // Heartbeat
    Data = ADC_In();  // sample 12-bit channel 1
    PF3 = 0x08;       // Profile Convert
    Position = Convert(Data); 
    PF3 = 0;          // end of Convert Profile
    PF1 = 0x02;       // Profile LCD
    ST7735_SetCursor(0,0);
    LCD_OutDec(Data); ST7735_OutString("    "); 
    ST7735_SetCursor(6,0);
    LCD_OutFix(Position);
    PF1 = 0;          // end of LCD Profile
  }
}   
int main(void){
  TExaS_Init();
	ST7735_InitR(INITR_REDTAB); 
	DisableInterrupts();
	SysTick_Init(1600000); // change 0 to whatever the RELOAD needs to be 50 Hz
	ADC_Init();   // inits port E (clock) and ADC
	PortF_Init(); // inits Port F
	EnableInterrupts();
  // your Lab 8
  while(1){
		while(flag == 0){} // busy wait for systick to sample ADC
		Data = ADCMail;           // Getting mail
		flag = 0;
		
		PF2 ^= 0x04;
		Position = Convert(Data); // COnverting
		PF2 ^= 0x04;
		
		PF1 ^= 0x02;	
		ST7735_SetCursor(6,0);    // Outputting
		LCD_OutFix(Position);		
    PF1 ^= 0x02;
	}
}

