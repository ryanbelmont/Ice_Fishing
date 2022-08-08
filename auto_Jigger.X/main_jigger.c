/*
 * File:   main_jigger.c
 * Author: ryan
 *
 * Created on August 8, 2022, 1:30 PM
 */


#include "xc.h"

#include <stdio.h>

// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)


// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, 
                                       // Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

void setup(void)
{
 // simple pic setup
    _RCDIV = 0;                 // timer 16 MHz
    AD1PCFG = 0xffff;           // all pins digital
    
 // SETUP SERVO:
    TRISBbits.TRISB6 = 0;       // set B6 to output
        // map output compare to pin B6:
    __builtin_write_OSCCONL(OSCCON & 0xbf);     // unlock PPS
    RPOR3bits.RP6R = 18;        // pin RP6 for output compare 1 = "18"
    RPINR7bits.IC1R = 8;        // input capture for button on pin RB8
    __builtin_write_OSCCONL(OSCCON | 0X40);     // LOCK PPS
    
    OC1CON = 0;                 // reset oc1 bits;
    OC1CONbits.OCTSEL = 0b1;    // use timer 3;
    OC1CONbits.OCM = 0b110;     // pwm with w/o faults
    
        // SETUP TIMER 3
    T3CON = 0;                  // reset bits
    T3CONbits.TCKPS = 0b01;     // 1:8 pre-scaler
    TMR3 = 0;
    PR3 = 40000 -1;             // 20ms overflow
    
    T3CONbits.TON = 1;          // start timer
    
    
// SETUP BUTTON:
    TRISBbits.TRISB8 = 1;       // input
    CNPU2bits.CN22PUE = 1;      // pull up resistor
    
    T2CON = 0;
    T2CONbits.TCKPS = 0b11;     // 1:256 pre-scale
    TMR2 = 0;                   
    PR2 = 62500 - 1;            // 1 second rollover
    
    T2CONbits.TON = 1;          // timer on
    
    
    IC1CON = 0;               
    IC1CONbits.ICTMR = 1;       // user timer 2 for capture source
    IC1CONbits.ICM = 0b011;     // turn on and capture every rising edge
    IFS0bits.IC1IF = 0;         // clear flag
    IEC0bits.IC1IE = 1;         // enable IC1 interrupt
}

volatile unsigned long int debounce_time = 0;
volatile unsigned long int state = 0;
void __attribute__((interrupt, auto_psv)) _IC1Interrupt(void)
{
    _IC1IF = 0;

    if(TMR3 > 5000)
    {
        TMR3 = 0;
        if (state == 0)
        {
            state = 1;
        }
        else
        {
            state = 0;
        }
    }
    
}

void setServo(int Val)
{
    OC1RS = Val;    // change the shadow register (for safety)
}

// blocking delay
void delay_ms(unsigned int ms)
{
    while(ms-- > 0)
    {
        asm("repeat #15998");
        asm("nop");
    }
}

int main(void) {
    setup();

    while(1)
    {
        if (state == 1)
        {
            setServo(2000);
            delay_ms(1000);
            setServo(3000);
            delay_ms(1000);
        }
        
    }
    return 0;
}
