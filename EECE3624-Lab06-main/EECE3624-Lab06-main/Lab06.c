/*
 * Lab06.c
 *
 * Created: 10/4/2022 9:16:57 AM
 * Initial Coder: jfhutton
 * Current Coder: Silverio Rivera-Lopez
 * Modified: Silverio Rivera-Lopez
 *
 * This lab uses hardware LEDs wired to PORTA, an interrupt from the joystick center
 * button, and a timer interrupt to build a game loop program that can display 
 * different patterns on the LEDs.
 *
 * While there are many "down and dirty" ways to get this coding done, try to 
 * remember your coding and data structures classes.  Things like ENUM, Arrays, Functions
 * could help make for more elegant coding.
 *
 */ 

#include <avr/io.h>              // Needed for AVR IO defines
#include <avr/interrupt.h>       // Needed for AVR interupt devines

#define LEDS  PORTA    // alias PORTA

	
// global variables for communication between ISRs and main
const unsigned char TCNT0_COUNT_SET = 0x8E;// Count for 1ms loop (Provided by Prof Hutton)
// <TBD> Your global variables should go here
enum Modes {Mode0, Mode1, Mode2, Mode3}; 
	
volatile enum Modes ModeOption = Mode0;
volatile unsigned char Tick = 0;
volatile unsigned char ChangeMode = 0;
	

int main(void){
	// Variables for main
	// <TBD> Your local main variables should go here
	unsigned char LEDposition = 0;
	signed char direction = 1;
	
	ModeOption = Mode0;
	Tick = 0;
	ChangeMode = 0;
	
	// Initialization for LEDs 
	// LEDs using A0-A7) 
	
	
	
	// Set direction for A ports.
	// (Prof Note:  This is similar to our two line assembly commands.)
	
	DDRA = 0xFF;  // Set the Direction for all PORTA pints to be outputs
	LEDS = 0xFF;  // Set the PORTA for all pins to be high (i.e. OFF)
	//LEDS = 0x00;  // TEST - Set all PORTA pins to be low (i.e. ON)
	
	// Initialization for Timer Interrupt
	// With a 7.3MHz crystal we have a 0.137us period
	// To build a 1ms timer "tick" we
	// - set the pre-scaler to 1/64 (8.77us)
	// - set the TCNT1 to count 114 counts (0-114=0x8E)
	// - enable the interrupt on overflow
	// (Prof Note: these are the commands you need, BUT they are commented
	//  out to start.  Be sure you fully understand them (book, datasheet, etc)
	//  before you enable them...  You also have to have the proper ISR routine
	//  ready for this to work.)
	// (Prof Note Two:  You will need to show your own calculations in the 
	//  Lab Report to verify the TCNT0_COUNT_SET value!)
	
	//TCCR0 = (1<<CS02);
	//TCNT0 = TCNT0_COUNT_SET;
	//TIMSK = (1<<TOIE0);
	
	// Port Initialization
	// Using the LED PORTA initilization above and your Lab05 code, 
	// Configure the joystick button.
	// (Prof Note: you will need to add another jumper wire, or 
	// change the ones you have, to get the center button working!
	TCNT0 = TCNT0_COUNT_SET;
	TCCR0 = (1 << CS01) | (1 << CS00);
	TIMSK |= (1 << TOIE0);
	
	
	// JoyStick
	
	DDRB &= ~(1 << PB4);
	PORTB |= (1 << PB4);
	DDRD &= ~(1 << PD0);
	PORTD |= (1 << PD0);
	
	
	
	
	// Interrupt Enable Block
	// Using your Lab05 code, you will need to update these
	// lines from 0x00 to have the appropriate mask.
	EICRA = (1 << ISC01) | (1 << ISC00);  // <TBD> Needs to be updated!
	EIMSK = (1 << INT0);  // <TBD> Needs to be updated!
	// Enable Global Interrupts
	sei();
	
	// Main Loop
	while (1) {
		// Your game loop will go here.
		if( ChangeMode)
		{
			LEDposition = 0;
			direction = 1;
			ChangeMode = 0;
			Tick = 0;
			LEDS = 0xFF; 
		}
		// <TBD>  Game Loop
		
	
		if (Tick >= 50)
		{
			Tick = 0;
		
			switch (ModeOption)
			{
			case Mode0:
				LEDS = (unsigned char)~(1 << LEDposition);
				LEDposition++;
				
				if (LEDposition >= 8)
				{
					LEDposition = 0;
				}
				
				break;
				
			case Mode1: 
				LEDS = (unsigned char)~(1 << (7 - LEDposition));
				LEDposition++;
				
				if( LEDposition >= 8)
				{
					LEDposition = 0;
				}
				
				break;
				
			case Mode2:
				LEDS = (unsigned char)~(1 << LEDposition);
				
				if (LEDposition ==7)
				{
					direction = -1;
				}
				
				else if (LEDposition == 0)
				{
					direction = 1;
				}
				
				LEDposition += direction;
				
				break;
				
			case Mode3:
					
				LEDS = 0xFF;
				break;
			default:
				
				ModeOption = Mode0;
				break;
			}
		}
	
	}
return 0;
}

// This is the required format of an ISR routine for a 
// Timer.  Your ISR code should go inside
// (Prof Note:  When you define this correctly, the JUMP TABLE
//  will be properly updated by the compiler.  (Thank you 
ISR(TIMER0_OVF_vect){
	
	// <TBD>  ISR code for timer interrupt.
	TCNT0 = TCNT0_COUNT_SET;
	
	if (Tick < 50){
		Tick++;
	}
	
}

// This is the required format of an ISR routine for a
// Hardware Pin Interupt.  Format is "PIN#_vect" where 
// '#' is the number of the interupt...  PD0->0, PD1->1, etc
ISR(INT0_vect){
	
	// <TBD>  
	switch(ModeOption){
		case Mode0:	
			ModeOption = Mode1;
			break;
		case Mode1:
			ModeOption = Mode2;
			break;
		case Mode2: 
			ModeOption = Mode3; 
			break;
		case Mode3:	
			ModeOption = Mode0;
			break;
		default:
			ModeOption = Mode0;
			break;
			
	}
	ChangeMode = 1; 
}