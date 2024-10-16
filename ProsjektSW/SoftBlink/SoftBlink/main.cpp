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

/* Nyttig info for PWM setup
https://avr-guide.github.io/pwm-on-the-atmega328/
*/

// PWM output configuration on OC0A (pin PD6/12)
int main(void)
{
	DDRD |= (1 << PD6); // Byttet til PIN PD6 for å ha analog utgang
	TCCR0A |= (1 << COM0A1); // Bruker som sammenlikningspin (Non-inverted mode)
	TCCR0A |= (1 << WGM01); // Set fast PWM mode
	TCCR0A |= (1 << WGM00); // Set fast PWM mode
	TCCR0B |= (1 << CS00); // No prescaling
	
    while (1) 
    {
		for (int i = 0; i < 255; i++)
		{
			OCR0A = i;
			_delay_ms(50);
		}
		
		for (int i = 255; i > 0; i--)
		{
			OCR0A = i;
			_delay_ms(50);
		}
    }
}

