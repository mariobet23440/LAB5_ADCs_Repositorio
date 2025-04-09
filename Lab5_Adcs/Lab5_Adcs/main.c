/*
 * LABORATORIO 5 - ADCs
 *
 * Created: 3/04/2025 14:05:25
 * Author : Mario Alejandro Betancourt Franco
 *
 
 PLANTEAMIENTO
 Uso de interrupciones
 + PIN-CHANGE: Contador de LEDs
 + TIMER0: Señal de Multiplexado
 
 
¿Qué debo hacer?
1. Multiplexado (Interrupciones con TIMER0) check
2. Mostrar contador en displays check
3. Activar ADC
4. Mostrar ADC en displays
 */ 

// LIBRERÍAS
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned int contador = 0;
unsigned int adc_lo;
unsigned int adc_hi;

// PROTOTIPOS DE FUNCIONES
int reset_timer0(void)
{
	// Operamos el temporizador en modo NORMAL
	TCCR0B |= (1 << CS01) | (1 << CS00);	// Prescaler de 64
	TCNT0 = 254;
	
	// Habilitar interrupciones por overflow de TIMER0
	TIMSK0 |= (1 << TOIE0);
	return 0;
}

// Mostrar números en Displays
int show_number(int num)
{
	unsigned int Tabla[] = {0x77, 0x44, 0x6B, 0x6D, 0x5C, 0x3D, 0x3F, 0x64, 0x7F, 0x7C, 0x5F, 0x3E, 0x39, 0x7A, 0x3F, 0x5F};
	PORTD = Tabla[num];
	return 0;
}


void setup_adc(void)
{
	ADMUX  = (1 << REFS0) | (1 << MUX2);   // Seleccionar el canal ADC4
	ADCSRA = (1 << ADEN) | (1 << ADIE);
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  // Establecer prescaler de 64
	ADCSRA |= (1 << ADSC);
}

// SETUP Y MAINLOOP
int main(void)
{	
	// CONFIGURACIÓN DE PUERTOS
	DDRB = 0X00;							// El puerto B es de entradas
	PORTB = (1 << 0) | (1 << 1);			// Habilitar pull-ups en PORTB
	DDRC = (1 << 1) | (1 << 2) | (1 << 3);	// BITS 1-3 del puerto C son salidas
	PORTC = (1 << PORTC1);					// Encender el bit 1 de PORTC
	DDRD = 0XFF;							// El puerto D es de salidas
	
	// CONFIGURACIÓN DE TIMER DE SISTEMA
	//CLKPR |= (1 << CLKPCE);					// Habilitar cambio en prescaler de reloj principal
	//CLKPR |= (1 << CLKPS2);					// Utilizar un prescaler de 16 

	// HABILITACIÓN DE INTERRUPCIONES PIN-CHANGE
	PCMSK0 = (1 << 0) | (1 << 1);
	PCICR = (1 << PCIE0);					// Habilitar interrupciones pin-change en PORTB
	
	// CONFIGURACIÓN DE TIMER0
	reset_timer0();
	
	// CONFIGURACIÓN DE ADC
	setup_adc();
	
	// Habilitar interrupciones
	sei();
	
	// MAINLOOP
    while (1);
	
	return 0;
}


// RUTINAS DE INTERRUPCIÓN
ISR(PCINT0_vect)
{	
	switch (PINB)
	{
	// Estas operaciones se dejaron en este formato por posibles cambios de compilador
	case (1 << PINB1):
		contador = contador + 1;
		break;
		
	case (1 << PINB0):
		contador = contador - 1;
		break;
		
	default :
		break;
	}
	PORTD = contador;
}

// Rutina de interrupción por overflow de Timer0 - Multiplexado
ISR(TIMER0_OVF_vect)
{
	unsigned char temp1 = PINC & 0b00001110;  // Quedarnos con solo PC1 a PC3
	unsigned char temp2 = 0;  // Inicializamos temp2 en 0
	
	// Comprobamos el estado de los pines PC1, PC2, y PC3
	switch (temp1)
	{
		case (1 << PINC1):
		{
			// MOSTRAR CONTADOR
			temp2 = (1 << PINC2);  // Activamos PC2
			//show_number(adc_hi);
			show_number(contador);
			break;	
		}
		
		case (1 << PINC2):
		{	
			// MOSTRAR NÚMERO 2
			temp2 = (1 << PINC3);  // Activamos PC3
			//show_number(adc_lo);
			show_number(contador);
			break;	
		}
		
		case (1 << PINC3):
		{
			// MOSTRAR NÚMERO 1 
			temp2 = (1 << PINC1);  // Activamos PC1
			
			PORTD = contador;
			break;	
		}
		
		default:
		{
			// MOSTRAR NÚMERO 1
			temp2 = (1 << PINC1);  // Si no se cumple ningún caso, activamos PC1 por defecto
			break;			
		}
	}

	// Limpiamos los pines de PC1 a PC3, y luego activamos el nuevo pin
	PORTC &= 0b11110000;    // Apagamos PC1, PC2 y PC3 (pines más altos se mantienen intactos)
	PORTC |= temp2;         // Activamos el pin seleccionado
}


ISR(ADC_vect)
{
	adc_lo = ADCL;			// Guardar byte bajo
	adc_hi = ADCH;			// Guardar byte alto
	ADCSRA |= (1 << ADSC);	// Iniciar conversión
}