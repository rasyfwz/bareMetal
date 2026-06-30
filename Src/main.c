#include "registerMap.h"

#define RCC ((RCC_TypeDef*) RCC_BASE)
#define GPIOA ((GPIO_TypeDef*) GPIOA_BASE)
#define USART2 ((USART_TypeDef*) USART2_BASE)


// helper function to initialize USART from bare metal.
void UART_Init() {
	RCC->APB1ENR |= (1U << 17);
	USART2->CR1 |= (1U << 13);
	USART2->CR1 |= (1U << 3);
	USART2->CR1 &= ~(1U << 12);
	USART2->BRR = 0x0000008B;//we are setting the whole register since the bits in BRR are mathematically linked with each other.
}

void GPIOA_Init() {
	RCC->AHB1ENR |= (1U << 0);
	GPIOA->MODER |= (1U << 10);
	GPIOA->MODER &= ~(1U << 11);
}

// helper function to transmit data in USART.
void UART_Transmit_Data(char *word) {
	while(*word != '\0') {

		while (!(USART2->SR & (1 << 7)));

		USART2->DR = *word;

		word++;
	}

	while (!(USART2->SR & (1 << 6)));
}

int main () {
	GPIOA_Init();
//	UART_Init();
//	char *send = "Hello World!\n";


	for (;;) {
		GPIOA->ODR ^= (1U << 5);
//		UART_Transmit_Data(send);
	}
	return 0;

}


