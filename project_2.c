/*
 * project_2.c
 *
 *  Created on: Feb 5, 2024
 *      Author: RC
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char sec = 0;
//unsigned char sec_2 = 0;
unsigned char min = 0;
//unsigned char min_2 = 0;
unsigned char hour = 0;
//unsigned char hour_2 = 0;



ISR(TIMER1_COMPA_vect)
{

	sec++;
	if(sec == 60)
	{
		sec =0;
		min++;
	}
	if(min == 60)
	{
		min = 0;
		hour++;
	}
	if(hour == 100)
	{
		hour = 0;
	}

}

ISR(INT0_vect)
{
	//reset

	hour=0;

	min=0;

	sec =0;
}


ISR(INT1_vect)
{
	//pause
	TCCR1B &= ~(1<<CS12) & ~(1<<CS10); //stop clock source

}


ISR(INT2_vect)
{
	//resume
	TCCR1B |= (1<<CS12) | (1<<CS10); //1024
}


void TIMER_ONE_Init()
{
	TCNT1=0;  // initial value 0
	OCR1A =976;
	/* from the equation we set the N vale for prescaler = 1024
	 * , so we need 976 counts means 1 interrupt  to get count 1 SEC
	 */
	TCCR1A = (1<<FOC1A); // set value of FOC1A to Force Output Compare for Compare unit A

	TCCR1B =  (1<<WGM12) | (1<<CS12) | (1<<CS10);
	/* WGM12 Timer Mode of Operation  CTC
	 * set pins CS12 and CS10 to enable prescaler 1024
	 */
	TIMSK = (1<<OCIE1A); //Timer/Counter1, Output Compare A Match Interrupt Enable
}





void INTERRUPT0_Reset_Intit()
{
	DDRD =DDRD &~(1<<PD2);  //input pin
	PORTD = PORTD | (1<<PD2); // internal pull up resistor
	MCUCR = MCUCR | (1<<ISC11);  // falling edge
	GICR = GICR | (1<<INT0);  // external interrupt0 enable

}


void INTERRUPT1_Pause_Intit()
{
	DDRD =DDRD &~(1<<PD3);  //input pin
	MCUCR = MCUCR | (1<<ISC11) | (1<<ISC10);  // raising edge
	GICR = GICR | (1<<INT1);  // external interrupt1 enable
}



void INTERRUPT2_Resume_Init()
{
	DDRB =DDRB &~(1<<PB2);  //input pin
	//PORTB = PORTB | (1<<PB2); //internal pull up resistor
	MCUCR = MCUCR & ~(1<<ISC2);  // falling edge
	GICR = GICR | (1<<INT2);  // external interrupt1 enable
}






void LCD (void)
{

	DDRC = DDRC |0x0F;
	PORTC = PORTC & 0xF0;
	DDRA = DDRA |0x3F;
	PORTA = PORTA |0x3F;
}


int main(void)
{
	SREG  = SREG | (1<<7);
	LCD();
	TIMER_ONE_Init();
	INTERRUPT0_Reset_Intit();
	INTERRUPT1_Pause_Intit();
	INTERRUPT2_Resume_Init();
	unsigned char arr[6];

	while(1)
	{


		arr[0]=(sec%10);
		arr[1]=(sec/10);
		arr[2]=(min%10);
		arr[3]=(min/10);
		arr[4]=(hour%10);
		arr[5]=(hour/10);
		for (unsigned char i =0 ; i<6;i++)
		{
			PORTA = (PORTA & 0xc0) | (1<<i);
			PORTC = (PORTC & 0xf0) | arr[i];
			_delay_ms(1);
		}

	}
}
