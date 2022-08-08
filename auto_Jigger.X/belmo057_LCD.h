/* 
 * File:   belmo057_LCD.h
 * Author: Ryan Belmont
 * Created on November 14, 2021, 4:41 PM
 * 
 * This library is used to initialize and use the Switch Science I2C LCD
 * Display (8x2 character) with a pic24Fj64FA002 microcontroller. The SDA pin is 
 * connected to pin 6 on the pic24 and the SCL pin is connected to pin 7
 * 
 * when using this library, first call lcd_init to setup the function.
 * Then set the curser with the desired x and y value of the first value in a
 * string.
 * Next use the lcd_printChar or lcd_printStrn to send print the data to the LCD
 * 
 * Ex:      lcd_init();
 *          lcd_setCursor(0,0);
 *          lcd_printStr("HELLO");
 */

#ifndef BELMO057_LCD_H
#define	BELMO057_LCD_H

#include <xc.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * This function sends a 1 byte command to the LCD display
 * It inputs the 8 bit command (command)
 * returns nothing
 */
void lcd_cmd(char command);

/*
 * function prints a single char to the LCD display at the current 
 * cursor position. 
 * 
 * inputs char package (hex char). 
 * returns nothing
 */
void lcd_printChar(char Package);

/* function sets the cursor in the LCD. Based on an algorithm, we can set the
 * cursor to the x and y locations. it then sends the information to set it.
 * 
 * inputs char'd x and y locations (x,y)
 *  returns nothing
 */
void lcd_setCursor(char x, char y);

/* function to delay for a specified amount of milliseconds.
 * inputs int number of milliseconds (ms)
 * returns nothing
*/
void delay_ms(unsigned int ms);

/*
 * function to initialize the correct settings on a pic 24 to communicate with
 * the LCD display. This includes sending all the commands needed to setup the
 * Switch Science I2C LCD Display (8x2 character)
 * 
 * It utilizes the lcd_cmd function to send each send each command
 * 
 * inputs nothing
 * returns nothing
 */
void lcd_init(void);


/*
 * function to print a string on the LCD display starting at the cursor location
 * 
 * inputs char array string (s[])
 * returns nothing
 */
void lcd_printStr(const char s[]);



#ifdef	__cplusplus
}
#endif

#endif	/* BELMO057_LCD_H */

