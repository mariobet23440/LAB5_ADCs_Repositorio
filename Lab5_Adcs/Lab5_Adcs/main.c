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

// Variables
unsigned char contador = 0;
unsigned char adc_lo;
unsigned char adc_hi;

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
void show_number(unsigned char num)
{
	unsigned int Tabla[] = {0x77, 0x42, 0x6D, 0x6B, 0x5A, 0x3B, 0x3F, 0x62, 0x7F, 0x7A, 0x7E, 0x1F, 0x35, 0x4F, 0x3D, 0x3C};
	PORTD = Tabla[num];
}

// Sacar el nibble bajo de un número de 1 byte
unsigned char low_nibble(char num)
{
	unsigned char low_nib = num & 0x0F; // Aplicamos una máscara sólo para los últimos 4 bits
	return low_nib;
}

// Sacar el nibble alto de un número de 1 byte
unsigned char high_nibble(char num)
{
	unsigned char high_nib = (num >> 4); // Aplicamos una máscara sólo para los últimos 4 bits
	return high_nib;
}

void setup_adc(void)
{
	ADMUX  = (1 << ADLAR) | (1 << REFS0) | (1 << MUX2);   // Seleccionar el canal ADC4 y fuente AVCC (Descartar últimos 2 bits)
	ADCSRA = (1 << ADEN) | (1 << ADIE);
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);  // Establecer prescaler de 64
	ADCSRA |= (1 << ADSC);
}

// SETUP Y MAINLOOP
int main(void)
{	
	// CONFIGURACIÓN DE PUERTOS
	DDRB = (1 << 5);						// PB5 es salida
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
    while (1)
	{
		//unsigned char temp = ADCL;
		
		// ENCENDER ALARMA SI ADCL es mayor a contador
		if(adc_hi > contador)
		{
			PORTB |= (1 << PORTB5);		// Encender sólo PB5
		}	
		else
		{
			PORTB &= ~(1 << PORTB5);	// Apagar solo PB5
		}
	}
	
	return 0;
}




// RUTINAS DE INTERRUPCIÓN
ISR(PCINT0_vect)
{	
	unsigned char temp = PINB & 0b00000011;  // Quedarnos con solo PB0 y PB1
	
	switch (temp)
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
			//show_number(adc_hi); Mostrar ADCH
			show_number(high_nibble(adc_hi));
			break;	
		}
		
		case (1 << PINC2):
		{	
			// MOSTRAR NÚMERO 2
			temp2 = (1 << PINC3);  // Activamos PC3
			//show_number(adc_lo); // Mostrar ADCL
			show_number(low_nibble(adc_hi));
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