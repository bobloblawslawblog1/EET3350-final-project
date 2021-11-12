#include <stdint.h>
#include "tm4c123gh6pm.h"

uint32_t lastButtonState = 0x00;
//initialize port a
void PortA_Init(){volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x00000001;
	delay = SYSCTL_RCGCGPIO_R;
	
	GPIO_PORTA_PCTL_R &= ~0x0000FF00;
  GPIO_PORTA_AMSEL_R &= ~0x0C;
  GPIO_PORTA_DIR_R |= 0x04;
  GPIO_PORTA_AFSEL_R &= ~0x0C;
  GPIO_PORTA_DEN_R |= 0x0C;
	GPIO_PORTA_DATA_R = 0x04;
}

uint32_t PortA_Input(){
	return (GPIO_PORTA_DATA_R&0x08);
}

void LED_Switch(){
	GPIO_PORTA_DATA_R ^= 0x04;
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
				LED_Switch();
			}
		}
		lastButtonState = PortA_Input();
	}
return 0;
}
