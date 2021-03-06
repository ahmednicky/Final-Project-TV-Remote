/*==============================================================================
 Project: TV-Remote control transmitter - Project starter code
 Date:    June 8, 2022
 
 This program provides basic functions and a program outline to start a TV
 remote control transmitter programming project. 
==============================================================================*/

#include    "xc.h"              // Microchip XC8 compiler include file
#include    "stdint.h"          // Include integer definitions
#include    "stdbool.h"         // Include Boolean (true/false) definitions

#include    "UBMP410.h"         // Include UBMP4.1 constant and function definitions

// TODO Set linker ROM ranges to 'default,-0-7FF' under "Memory model" pull-down.
// TODO Set linker code offset to '800' under "Additional options" pull-down.

// Sony IR command/data codes
#define ROKU_VOLUP  0x12        // Volume up button command code
#define	ROKU_VOLDN  0x13        // Volume down button command code
#define ROKU_POWER  0x15        // Power button command code
#define ROKU_VOLUP  0b00001111        // Volume up button command code
#define	ROKU_VOLDN  0b00010000        // Volume down button command code
#define ROKU_POWER  0b00010111        // Power button command code

// Sony IR device/address codes
#define TV   0b11101010      // TV device address code

// Sony IR protocol timing and definitions
#define ROKU_START_PULSES   96  // Number of 40kHz IR pulses to make Start bit
#define ROKU_ZERO_PULSES    24  // Number of IR pulses to make 'zero' bit
#define ROKU_ONE_PULSES     48  // Number of IR pulses to make 'one' bit
#define ROKU_BIT_DELAY  600     // Delay between bits in microseconds
#define ROKU_DATABITS   7       // Number of data/command bits to send
#define ROKU_ADDRBITS   5       // Number of address/device bits to send


// Program variable definitions
unsigned char bits;             // Bit counter variable for bit transmission

void pulse_40kHz(unsigned int pulses)
{
    for(pulses; pulses != 0; pulses --)
    {
        IRLED = 1;
        __delay_us(12);
        IRLED = 0;
        __delay_us(11);
    }
}

void pulse_38kHz(unsigned int pulses)
{
    for(pulses; pulses != 0; pulses --)
    {
        IRLED = 1;
        __delay_us(13);
        IRLED = 0;
        __delay_us(12);
    }
}

void transmit_NEC(unsigned char address, unsigned char command)
{
    // Save !(address), !(command)
    unsigned char addressCopy = !address;
    unsigned char commandCopy = !command;

    // Send start pulse
    pulse_38kHz(341);

    // Start pulse delay
    __delay_us(4500);

    // Transmit address
    for(bits = 8; bits != 0; bits --)
    {
        pulse_38kHz(21);
        if((address & 0b11101010) == 0)
        {
            __delay_us(560);
        }
        else
        {
            __delay_us(1690);
        }
        address = address >> 1;
    }

    // Transmit !(address)
    for(bits = 8; bits != 0; bits --)
    {
        pulse_38kHz(21);
        if((addressCopy & 0b11000111) == 0)
        {
            __delay_us(560);
        }
        else
        {
            __delay_us(1690);
        }
        addressCopy = addressCopy >> 1;
    }

    // Transmit command
    for(bits = 8; bits != 0; bits --)
    {
        pulse_38kHz(21);
        if((command & 0b00010111) == 0)
        {
            __delay_us(560);
        }
        else
        {
            __delay_us(1690);
        }
        command = command >> 1;
    }

    // Transmit !(command)
    for(bits = 8; bits != 0; bits --)
    {
        pulse_38kHz(21);
        if((commandCopy & 11101000) == 0)
        {
            __delay_us(560);
        }
        else
        {
            __delay_us(1690);
        }
        commandCopy = commandCopy >> 1;
    }
}

/* void transmit_Sony(unsigned char address, unsigned char command)
{
    // Send start pulse
    // pulse_40kHz(ROKU_START_PULSES);
    
    // Start pulse delay
    //__delay_us(ROKU_BIT_DELAY);
    
    // Transmit 7 command bits LSB-first
    /* for(bits = 8; bits != 0; bits --)
    {
        if((command & 0b00000001) == 0)
        {
            pulse_40kHz(ROKU_ZERO_PULSES);
        }
        else
        {
            pulse_40kHz(ROKU_ONE_PULSES);
        }
        __delay_us(ROKU_BIT_DELAY);
        command = command >> 1;
    }
    
    // Transmit 5 address bits LSB-first
    
} */

int main(void)
{
    OSC_config();               // Configure internal oscillator for 48 MHz
    UBMP4_config();             // Configure on-board UBMP4 I/O devices
	
    while(1)
	{
        // Pulse timing test code
        // pulse_38kHz(100);
        
        // Read pushbuttons
        // Power on & off
        if(SW2 == 0)
        {
            transmit_NEC(TV, ROKU_POWER);
        }
        // Volume up
        if(SW4 == 0)
        {
            transmit_NEC(TV, ROKU_VOLUP);
        }
        // Volume down
        if(SW5 == 0)
        {
            transmit_NEC(TV, ROKU_VOLDN);
        }
        // Is it a new button press? Transmit address and command for button.
        
        // Is button a repeat press? Transmit repeat code - protocol dependent.
        
        // Delay between button presses (adjust based on protocol)
        __delay_ms(20);
        
        // Activate bootloader if SW1 is pressed.
        if(SW1 == 0)
        {
            RESET();
        }
    }
}

/* Learn More - Program Analysis Activities
 * 
 * 1. Modify the UBMP4_config() function to allow output on the IR led pin. Use
*     the test code and an oscilloscope to measure the width of the high and
 *    low pulses.
 * 
 * 2. Why are the delays in the pulse_40kHz function different by 1us, yet the
 *    pulses are similar in length?
 * 
 * 3. Adjust the delays in the pulse_40kHz() function to make the high and low
 *    pulses as close to 25us as possible. Add NOP(); single cycle delays, or
 *    replace __delay_us() functions with _delay() clock cycle functions to
 *    modify the pulse high and low period delays.
 * 
 * 4. Determine the number of pulses required to create a start bit, a zero bit,
 *    and a one bit for the protocol that your remote control uses.
 * 
 * 5. Complete the transmit_Sony() function by adding the code to transmit the
 *    5-bit device address code after the command code.
 *
 * 6. Create the definitions and the functions required to transmit the protocol
 *    used by your IR device.
 */