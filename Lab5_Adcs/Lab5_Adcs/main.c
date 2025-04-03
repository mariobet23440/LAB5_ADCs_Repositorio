/*
 * LABORATORIO 5 - ADCs
 *
 * Created: 3/04/2025 14:05:25
 * Author : Mario Alejandro Betancourt Franco
 */ 

// LIBRERÍAS
#include <avr/io.h>
#include <avr/interrupt.h>

// VARIABLES GLOBALES
unsigned char z = 0;

int main(void)
{	
	// CONFIGURACIÓN DE PUERTOS
	DDRB = 0X00;					// El puerto B es de entradas
	PORTB = (1 << 0) | (1 << 1);	// Habilitar pull-ups en PORTB
	DDRC = DDRC | (1 << PORTC1);	// El bit 1 del puerto C es salida
	PORTC = PORTC | (1 << PORTC1);	// Encender el bit 1 de PORTC
	DDRD = 0XFF;					// El puerto D es de salidas

	// HABILITACIÓN DE INTERRUPCIONES PIN-CHANGE
	PCMSK0 = (1 << 0) | (1 << 1);
	PCICR = (1 << PCIE0);			// Habilitar interrupciones pin-change en PORTB
	
	// Habilitar interrupciones
	sei();
	
	// MAINLOOP
    while (1);
}

// RUTINAS DE INTERRUPCIÓN
ISR(PCINT0_vect)
{	
	unsigned char temp = PIND;
	
	switch (PINB)
	{
	// Estas operaciones se dejaron en este formato por posibles cambios de compilador
	case (1 << PINB1):
		temp = temp + 1;
		break;
		
	case (1 << PINB0):
		temp = temp - 1;
		break;
		
	default :
		break;
	}
	PORTD = temp;
}

