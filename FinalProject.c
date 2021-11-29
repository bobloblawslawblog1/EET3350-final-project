#include <stdint.h>
#include "tm4c123gh6pm.h"

uint32_t lastButtonState = 0x00;
//initialize port a
void PortA_Init(){volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x00000001;
	delay = SYSCTL_RCGCGPIO_R;
	
	GPIO_PORTA_PCTL_R &= ~0x000FFF00;
  GPIO_PORTA_AMSEL_R &= ~0x1C;
  GPIO_PORTA_DIR_R |= 0x14;
  GPIO_PORTA_AFSEL_R &= ~0x1C;
  GPIO_PORTA_DEN_R |= 0x1C;
	GPIO_PORTA_DATA_R = 0x00;
}

uint32_t PortA_Input(){
	return (GPIO_PORTA_DATA_R&0x08);
}

void Heating_Element_Switch(){		//switches state of the red LED which represents a heating element
	GPIO_PORTA_DATA_R ^= 0x10;
}

void Pump_Switch(){		//switches state of green LED which represents a pump
	GPIO_PORTA_DATA_R  ^= 0x04;
}
void delay(int loops){
	int i;
	for(i=0;i<loops;i++){
	}
}

int main(){
	
	PortA_Init();
	while(1){
		uint32_t porta = PortA_Input();
		delay(400000);
		if(PortA_Input() != lastButtonState){
			if(PortA_Input() == 0x08){
				Heating_Element_Start();
				delay(4000000);
				Pump_Start();
			}
		}
		lastButtonState = PortA_Input();
	}
return 0;
}
