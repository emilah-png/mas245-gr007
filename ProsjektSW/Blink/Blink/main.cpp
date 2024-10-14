/*
 * Prosjekt1.cpp
 *
 * Created: 09/10/2024 11:14:39
 * Author : Are-Laptop
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>


int main(void)
{
	DDRB = (1 << PB1);
	
    while (1) 
    {
		PORTB |= (1 << PB1);
		_delay_ms(1000);
		
		PORTB &= ~(1 << PB1);
		_delay_ms(1000);
		
    }
}

