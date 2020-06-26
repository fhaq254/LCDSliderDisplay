// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 11/8/2017 
// Student names: Fawadul Haq and Rafael Herrejon
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x30;      // 1) activate clock for Port E and F
  while((SYSCTL_PRGPIO_R&0x30) == 0){};
  GPIO_PORTE_DIR_R &= ~0x04;      // 2) make PE2 input
  GPIO_PORTE_AFSEL_R |= 0x04;     // 3) enable alternate fun on PE2
  GPIO_PORTE_DEN_R &= ~0x04;      // 4) disable digital I/O on PE2
  GPIO_PORTE_AMSEL_R |= 0x04;     // 5) enable analog fun on PE2
	
	SYSCTL_RCGCADC_R |= 0x01;   // Enable ADC clock
	delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;       // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
	ADC0_PC_R      = 0x01;      // Set 125kHz ADC conversion speed
	ADC0_SSPRI_R   = 0x0123;    // Set sequencer priority: sequence 3 is highest priority
	ADC0_ACTSS_R  &= ~(0x08);   // Disable selected sequence 3
	ADC0_EMUX_R   &= ~(0xF000); // Set software start trigger event
	ADC0_SSMUX3_R &= ~(0x0F);   // Set input source (channel 1: PE2)
	ADC0_SSMUX3_R |= 0x01;
	ADC0_SSCTL3_R  = 0x06;       // Set sample control bits
	ADC0_IM_R     &= ~(0x08);   // Disable interrupts
	ADC0_ACTSS_R  |= 0x08;      // Enable selected sequencer 3
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){
  uint32_t output;
  
	ADC0_PSSI_R |= 0x08;               // set software trigger
	while((ADC0_RIS_R&0x08) == 0){}    // busy-wait for Raw Interrupt Status
	output = ADC0_SSFIFO3_R&0x0FFF;    // Read the ADC Data
	ADC0_ISC_R = 0x08;                 // Clear sample complete flag
	
	return output;
}



