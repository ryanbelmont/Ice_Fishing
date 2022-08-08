/*
 * Ryan Belmont
 * 11-14-2021
 * function definitions for the library "belmo057_LCD"
 */


#include "xc.h"
#include "belmo057_LCD.h"

/*
 * This function sends a 1 byte command to the LCD display
 * It inputs the 8 bit command (command)
 * returns nothing
 */
void lcd_cmd(char command)
{
    IFS3bits.MI2C2IF = 0; // flag to 0
    I2C2CONbits.SEN = 1; // start
    while(I2C2CONbits.SEN == 1); // once start sequence has completed
    
    // send Address and Write Command
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100;
    while(IFS3bits.MI2C2IF == 0); // wait for IF to set
    
    // send high byte (always 0x00)
    IFS3bits.MI2C2IF = 0; // reset flag
    I2C2TRN = 0b00000000; // 8-bits consisting of control byte
    while(IFS3bits.MI2C2IF == 0); // wait for IF to set
    
    // send Package
    IFS3bits.MI2C2IF = 0; // reset flag
    I2C2TRN = command; // send command
    while(IFS3bits.MI2C2IF == 0); // wait for IF to set
    
    
    // send stop
    IFS3bits.MI2C2IF = 0; // clear flag
    I2C2CONbits.PEN = 1; // send stop bit
    while(I2C2CONbits.PEN == 1); // wait for stop bit to return to 0
}


/*
 * function prints a single char to the LCD display at the current 
 * cursor position. 
 * 
 * inputs char package (hex char). 
 * returns nothing
 */
void lcd_printChar(char Package)
{
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.SEN = 1; // start
    while(I2C2CONbits.SEN == 1); // once start sequence has completed
    
    // send Address and Write Command
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100;
    while(IFS3bits.MI2C2IF == 0);
    
    // send high byte (always 0x00)
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01000000; // 8-bits consisting of control byte w/ RS = 1
    while(IFS3bits.MI2C2IF == 0);
    
    // send Package
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = Package; // sends the char
    while(IFS3bits.MI2C2IF == 0);
    
    
    // send stop
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1);
}

/* function sets the cursor in the LCD. Based on an algorithm, we can set the
 * cursor to the x and y locations. it then sends the information to set it.
 * 
 * inputs char'd x and y locations (x,y)
 *  returns nothing
 */
void lcd_setCursor(char x, char y)
{
    char cursor = (0x40 * y) + x; // algorithm to find command
    cursor |= 0b10000000; // sets first bit to 1
    
    // start sending
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.SEN = 1;
    while(I2C2CONbits.SEN == 1); // once start sequence has completed
    
    // send Address and Write Command
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100;
    while(IFS3bits.MI2C2IF == 0);
    
     // send high byte (always 0x00)
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b00000000; // 8-bits consisting of control byte w/ RS = 1
    while(IFS3bits.MI2C2IF == 0);
    
    // send cursor
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = cursor;
    while(IFS3bits.MI2C2IF == 0);
    
    
    // send stop
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1);
}

/* function to delay for a specified amount of milliseconds.
 * inputs int number of milliseconds (ms)
 * returns nothing
*/
void delay_ms(unsigned int ms) 
{
    while (ms-- > 0)
    {
        asm("repeat #15998"); // 1 ms delay
        asm("nop");
    }
}

/*
 * function to initialize the correct settings on a pic 24 to communicate with
 * the LCD display. This includes sending all the commands needed to setup the
 * Switch Science I2C LCD Display (8x2 character)
 * 
 * inputs nothing
 * returns nothing
 */
void lcd_init(void)
{
    _RCDIV = 0; // set frequency to 16 MHz
    AD1PCFG = 0xffff; // set all pins digital
    I2C2BRG = 157; // 100 KHz frequency
    IFS3bits.MI2C2IF = 0; // clear interrupt flag
    I2C2CONbits.I2CEN = 1; // enable I2C
    
    delay_ms(40);
    
    lcd_cmd(0b00111000); // function set
    lcd_cmd(0b00111001); // function set, advance instruction mode
    lcd_cmd(0b00010100); // interval osc
    lcd_cmd(0b01110000); // contrast Low
    lcd_cmd(0b01010110); // power/ICON/contract control
    lcd_cmd(0b01101100); // follower control
    
    delay_ms(200); // let VDD become stable
    
    lcd_cmd(0b00111000); // function set
    lcd_cmd(0b00001100); // Display On
    lcd_cmd(0b00000001); // clear Display
    
    delay_ms(1); // wait
}

/*
 * function to print a string on the LCD display starting at the cursor location
 * 
 * inputs char array string (s[])
 * returns nothing
 */
void lcd_printStr(const char s[])
{
    int position = 0; // tracks digit on string
    
    // start sending
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.SEN = 1;
    while(I2C2CONbits.SEN == 1); // once start sequence has completed
            
    // send Address
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b01111100;
    while(IFS3bits.MI2C2IF == 0);
    
    // while there is a digit in the string to send
    while(s[position] != '\0') // if current digit in string is not null
    {
        IFS3bits.MI2C2IF = 0;

        if(s[position+1] != '\0') // if last bit
        {
            I2C2TRN = 0b11000000; // command send last bit
        }
        else
        {
            I2C2TRN = 0b01000000; // command going to send another bit after   
        }
        while(IFS3bits.MI2C2IF == 0); // wait for response
       
        
        IFS3bits.MI2C2IF = 0; // 
        I2C2TRN = s[position]; // send char
        while(IFS3bits.MI2C2IF == 0); // wait
        position++; // increase position
    }
    // send stop bit
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1);
    
}