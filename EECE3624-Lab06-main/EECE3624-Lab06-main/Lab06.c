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
 *
 * LED Patterns 
 * Mode 0: Low to HIgh LED0-LED7
 * Mode 1: High to Low LED7-LED0
 * Mode 2: Back and Forth
 * Mode 3: All LED's OFF
 *
 *
 *
 */ 

#include <avr/io.h>              // Needed for AVR IO defines like DDRA, PORTA, TCCR0, and TCNT0
#include <avr/interrupt.h>       // Needed for interrupt functions 

#define LEDS  PORTA    // alias PORTA

	
// global variables for communication between ISRs and main
const unsigned char TCNT0_COUNT_SET = 0x8E;// Count for 1ms loop (Provided by Prof Hutton)

// <TBD> Your global variables should go here

enum Modes {Mode0, Mode1, Mode2, Mode3}; // Modes define in description
	
volatile enum Modes ModeOption = Mode0; // Stores the current selected LED pattern. Volatile 

volatile unsigned char Tick = 0; // stores the number of timer ticks. Timer0 increments tick every 1ms

volatile unsigned char ChangeMode = 0; // Flag used to notify main that the joystick changed modes. 
	

int main(void){
	// Variables for main
	// <TBD> Your local main variables should go here
	
	unsigned char LEDposition = 0; // stores position of the current active LED
	signed char direction = 1; // Movement for direction of LEDs. 1 means move toward LED7 or -1 move toward LED0 
	
	
// State Machine Initialization
	ModeOption = Mode0; // Start in Mode 0
	Tick = 0;		    // Reset ms counter
	ChangeMode = 0;     // clear the change mode flag
//  	

		
	// Set direction for A ports.
	// (Prof Note:  This is similar to our two line assembly commands.)
// PORT Initialization 
	DDRA = 0xFF;  // Set the Direction for all PORTA pints to be outputs
	// 1 = Output or 0 = Input
				 
				 
	LEDS = 0xFF;  // Set the PORTA for all pins to be high (i.e. OFF)
	//LEDS = 0x00;  // TEST - Set all PORTA pins to be low (i.e. ON)
	// 0 = LED ON or 1 = LED OFF
	
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
	
// Timer 0 Initialization

	TCNT0 = TCNT0_COUNT_SET; // Load 142 into Timer0 before starting
	TCCR0 = (1 << CS01) | (1 << CS00); // Timer0 using prescaler of 64
	TIMSK |= (1 << TOIE0); // Enable Timer0 overflow interrupt
	
// 
	
// JoyStick Initialization
	
	DDRB &= ~(1 << PB4); // Configure PB4 as an input
	PORTB |= (1 << PB4); // Enable PB4 interal pull up resistor
	DDRD &= ~(1 << PD0); // Configure PD0 as an input
	PORTD |= (1 << PD0); // Keep PD0 HIGH when button is not pressed
	
//
	
	
	// Interrupt Enable Block
	// Using your Lab05 code, you will need to update these
	// lines from 0x00 to have the appropriate mask.
	
	EICRA = (1 << ISC01) | (1 << ISC00);
	EIMSK = (1 << INT0);  // Enable External Interrupt INT0
	
	
	sei(); // Enable Global Interrupts
	
	// Main Loop
	while (1) { // micro controller checks continuously 
		// Your game loop will go here.
		
		if( ChangeMode) // start here if ChangeMode = 1
		{
			LEDposition = 0; // restart LED position at LED0 
			direction = 1;   // Reset the direction toward LED7
			ChangeMode = 0;  // Clear the change mode flag
			Tick = 0;		 // Restart timing internal
			LEDS = 0xFF;     // Turn OFF all LEDs before starting the new mode
		}
	
		if (Tick >= 180) // LED delay of 180ms. Makes it slower
		{
			Tick = 0;  // Reset Tick for next LED interval
		
			switch (ModeOption) // select pattern based on the current mode
			{
			case Mode0:
				LEDS = (unsigned char)~(1 << LEDposition); // reversing the bits
				LEDposition++; // increase LED position by 1
				
				if (LEDposition >= 8) // LED position reaches 8 then restart at 0. 
				{
					LEDposition = 0;
				}
				
				break;
				
			case Mode1: 
				LEDS = (unsigned char)~(1 << (7 - LEDposition)); // subtracting LED postion from reverse order
				LEDposition++; // next position
				
				if( LEDposition >= 8) // restart after displaying LED0
				{
					LEDposition = 0;
				}
				
				break; // exit mode0 
				
			case Mode2:
				LEDS = (unsigned char)~(1 << LEDposition); // Turn on the LED at current position
				
				if (LEDposition ==7) // Checks if the LED reached the upper end
				{
					direction = -1; // Reverse the direction toward LED0
				}
				
				else if (LEDposition == 0) // Reach the LED lower end
				{
					direction = 1; // Reverse the direction toward LED7
				}
				
				LEDposition += direction; // Update the LED position using direction
				
				break; //exit Mode2
				
			case Mode3:
					
				LEDS = 0xFF; // all of them are off. A simple implementation
				break;
				
			default: // returns to Mode0 if an unexpected mode happens
				
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


// ISR executes when Timer0 overflows
ISR(TIMER0_OVF_vect){
	
	// <TBD>  ISR code for timer interrupt.
	TCNT0 = TCNT0_COUNT_SET; // Reloads timer0 with 142
	
	if (Tick < 180){ // Check if TICK is below the LED delay
		Tick++; // increment ms counter
	}
	
}

// This is the required format of an ISR routine for a
// Hardware Pin Interupt.  Format is "PIN#_vect" where 
// '#' is the number of the interupt...  PD0->0, PD1->1, etc

// ISR executes when the center joystick button is released
// 
ISR(INT0_vect){
	
	// <TBD>  
	
	switch(ModeOption){ // check the current selected LED pattern
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
			ModeOption = Mode0; // Reset to Mode0 
			break;
			
	}
	ChangeMode = 1;  // notifies the main that the mode has changed
}