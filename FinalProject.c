#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Nokia5110.c"
#include "Nokia5110.h"

#define NVIC_ST_CTRL_R (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD_R (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT_R (*((volatile uint32_t *)0xE000E018))


uint32_t lastButtonState = 0x08;

void PLL_Init(void){

  // 0) Use RCC2
  SYSCTL_RCC2_R |=  0x80000000;  // USERCC2
  // 1) bypass PLL while initializing
  SYSCTL_RCC2_R |=  0x00000800;  // BYPASS2, PLL bypass
  // 2) select the crystal value and oscillator source
  SYSCTL_RCC_R = (SYSCTL_RCC_R &~0x000007C0)   // clear XTAL field, bits 10-6
                 + 0x00000540;   // 10101, configure for 16 MHz crystal
  SYSCTL_RCC2_R &= ~0x00000070;  // configure for main oscillator source
  // 3) activate PLL by clearing PWRDN
  SYSCTL_RCC2_R &= ~0x00002000;
  // 4) set the desired system divider
  SYSCTL_RCC2_R |= 0x40000000;   // use 400 MHz PLL
  SYSCTL_RCC2_R = (SYSCTL_RCC2_R&~ 0x1FC00000)  // clear system clock divider
                  + (4<<22);      // configure for 80 MHz clock
  // 5) wait for the PLL to lock by polling PLLLRIS
  while((SYSCTL_RIS_R&0x00000040)==0){};  // wait for PLLRIS bit
  SYSCTL_RCC2_R &= ~0x00000800;
}

void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0; // 1) disable SysTick during setup
	NVIC_ST_RELOAD_R = 0x00FFFFFF; // 2) maximum reload value
	NVIC_ST_CURRENT_R = 0; // 3) any write to CURRENT clears it
	NVIC_ST_CTRL_R = 0x00000005; // 4) enable SysTick with core clock
} 

void PortE_Init(){volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x10;
	delay = SYSCTL_RCGCGPIO_R;
	
	GPIO_PORTE_PCTL_R &= ~0x0000FFFF;
  GPIO_PORTE_AMSEL_R &= ~0x0F;
  GPIO_PORTE_DIR_R |= 0x03;
  GPIO_PORTE_AFSEL_R &= ~0x0F;
  GPIO_PORTE_DEN_R |= 0x0F;
	GPIO_PORTE_DATA_R = 0x00;
}

uint32_t PortE_Input(){
	return (GPIO_PORTE_DATA_R&0x0C);
}

void Heating_Element_Start(){		//turns on the red LED which represents a heating element
	GPIO_PORTE_DATA_R |= 0x01;
}

void Pump_Start(){		//turns on green LED which represents a pump
	GPIO_PORTE_DATA_R  |= 0x02;
}

void Heating_Element_Stop(){
	GPIO_PORTE_DATA_R &= 0xFE;
}

void Pump_Stop(){
	GPIO_PORTE_DATA_R &= 0xFD;
}

void SysTick_Wait(uint32_t delay){
	NVIC_ST_RELOAD_R = delay-1;
	NVIC_ST_CURRENT_R = 0;
	while((NVIC_ST_CTRL_R&0x00010000)==0){
	}
}
void SysTick_Wait1ms(uint32_t delay){
	unsigned long i;
	for(i=0; i<delay;i++){
		SysTick_Wait(80000);
	}
}
void Startup_Delay(uint32_t delay){
	uint32_t i = delay;
	Nokia5110_Clear();
	Nokia5110_OutString(" Pump on in");
	Nokia5110_SetCursor(2,2);
	Nokia5110_OutString("seconds");
  while(i > 0){
		Nokia5110_SetCursor(1,1);
		Nokia5110_OutUDec(i);
		SysTick_Wait1ms(1000);
		i--;
		Nokia5110_SetCursor(1,1);
		Nokia5110_OutUDec(i);
	}	
}

void Shutdown_Delay(uint32_t delay){
	uint32_t i = delay;
	Nokia5110_Clear();
	Nokia5110_OutString("Pump off in");
	Nokia5110_SetCursor(2,2);
	Nokia5110_OutString("seconds");
  while(i > 0){
		Nokia5110_SetCursor(1,1);
		Nokia5110_OutUDec(i);
		SysTick_Wait1ms(1000);
		i--;
		Nokia5110_SetCursor(1,1);
		Nokia5110_OutUDec(i);
	}	
}

int main(){
	PLL_Init();
	SysTick_Init();
	Nokia5110_Init();
	PortE_Init();
	Nokia5110_Clear();
	while(1){
		uint32_t porte = PortE_Input();
		SysTick_Wait1ms(1);
		if(PortE_Input() != lastButtonState){
			if(PortE_Input() == 0x04){
				Heating_Element_Start();
				Startup_Delay(5);
				Pump_Start();
				Nokia5110_Clear();
				Nokia5110_OutString("System      Running");
			}else if(PortE_Input() == 0x08){
				Heating_Element_Stop();
				Shutdown_Delay(5);
				Pump_Stop();
				Nokia5110_Clear();
				Nokia5110_OutString("System      Off");
			}else{
				continue;
			}
				
		}
		lastButtonState = PortE_Input();
	}
return 0;
}
