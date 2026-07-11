#include "registerMap.h"

#define RCC ((RCC_TypeDef*) RCC_BASE)
#define GPIOA ((GPIO_TypeDef*) GPIOA_BASE)
#define USART2 ((USART_TypeDef*) USART2_BASE)
#define SPI1 ((SPI_TypeDef*) SPI1_BASE)


// helper function to initialize USART from bare metal.
void UART_Init() {
	RCC->APB1ENR |= (1U << 17);
	RCC->AHB1ENR |= (1U << 0);
	GPIOA->MODER &= ~(1U << 4);
	GPIOA->MODER |= (1U << 5);
	GPIOA->MODER &= ~(1U << 6);
	GPIOA->MODER |= (1U << 7);
	GPIOA->AFRL |= (7U << 8);
	GPIOA->AFRL |= (7U << 12);
	USART2->CR1 |= (1U << 3);
	USART2->CR1 &= ~(1U << 12);
	USART2->BRR = 0x0000008B;//we are setting the whole register since the bits in BRR are mathematically linked with each other.
	USART2->CR1 |= (1U << 13);
}

void GPIOA_Init() {
	RCC->AHB1ENR |= (1U << 0);
	GPIOA->MODER |= (1U << 10);
	GPIOA->MODER &= ~(1U << 11);
	GPIOA->OSPEEDR |= (3U << 10);
}

void SPI1_Init() {
	RCC->AHB1ENR |= (1U << 0);
	RCC->APB2ENR |= (1U << 12);

	GPIOA->MODER &= ~((3U << 10) | (3U << 12) | (3U << 14));
	GPIOA->MODER |=  ((2U << 10) | (2U << 12) | (2U << 14));

	GPIOA->AFRL &= ~((0xFU << 20) | (0xFU << 24) | (0xFU << 28));
	GPIOA->AFRL |=  ((5U << 20) | (5U << 24) | (5U << 28));

	GPIOA->OSPEEDR = 0;
	GPIOA->OSPEEDR |= (3U << 10) | (3U << 12) | (3U << 14);

	SPI1->CR1 = 0;
	SPI1->CR1 |= (1U << 2);            // MSTR
	SPI1->CR1 |= (1U << 9) | (1U << 8); // SSM + SSI
	SPI1->CR1 |= (6U << 3);            // BR: /256 for bring-up
	SPI1->CR1 |= (1U << 6);            // SPE last
}
void set_bit(volatile uint32_t *reg, uint32_t pos, uint32_t width, uint32_t value) {
	uint32_t copy = *reg;
	copy &= ~(((1U << width) - 1) << pos);
	copy |= (value << pos);
	*reg = copy;
}
// helper function to transmit data in USART.
void UART_Transmit_Data(char * word) {
	while(*word != '\0') {

		while (!(USART2->SR & (1 << 7)));

		USART2->DR = *word;

		word++;
	}

	while (!(USART2->SR & (1 << 6)));
}

void SPI1_Transmit(uint8_t data) {
	while (!(SPI1->SR & (1U << 1)));
//	&SPI1->DR;// wait TXE
	SPI1->DR = data;
	while (SPI1->SR & (1U << 7));     // wait until not BSY
}

	int main () {
	GPIOA_Init();
	UART_Init();
	char *send = "Hello World!\r\n";
	SPI1_Init();
	char byte = 0x50;


	for (;;) {
		GPIOA->ODR ^= (1U << 5);
		UART_Transmit_Data(send);
		for (int i = 0; i < 1000000; i++);
		SPI1_Transmit(byte);
	}
	return 0;

}


