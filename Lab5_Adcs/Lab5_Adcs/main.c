/*
 * LABORATORIO 5 - ADCs
 *
 * Created: 3/04/2025 14:05:25
 * Author : mario
 */ 

// LIBRERÍAS
#include <avr/io.h>
#include <avr/interrupt.h>


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
	PORTD ^= (1<<5);	// Encender y apagar el bit 5 de PORTD siempre que un botón cambie de estado.
}

