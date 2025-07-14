#include <msp430.h>
#include "LiquidCrystal_I2C.h"
#include <stdio.h>
#include <string.h>


#define keyport P3OUT

/* Column pins
   Column pins are P3.0, P3.1, P3.2, and P3.3.*/
#define COL1 (P3IN & 0x10)
#define COL2 (P3IN & 0x20)
#define COL3 (P3IN & 0x40)
#define COL4 (P3IN & 0x80)


unsigned char key_press;
unsigned i, k, key = 0;

unsigned char Key_Val[] = {' ','1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};

// User-defined functions prototypes:
unsigned char get_key(void);
void DelayMs(unsigned int Ms);

// main start
void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop WDT

    unsigned int count = 0;     // Count set to 0 initially.
    unsigned char Value = 0;

    P3DIR = 0x0F;               // Output registers for the ROWS. Pins: P3.4, P3.5, P3.6, P3.7.

    unsigned int pwm_duty = 0;
    unsigned int entered_value = 0;
    unsigned int is_new_entry = 1;

    // Pull-up resistor initialization
    P3REN = 0xFF;               // Should correlate with input pins (columns).
    P3OUT = 0xF0;               // Set P3 with pull-up resistor type

    PM5CTL0 &= ~LOCKLPM5;       // Disable high-impedance mode.

    // LCD function call and setup
    I2C_Init(0x27);             // 0x27 lcd address
    LCD_Setup();
    LCD_SetCursor(0, 0);        // Initial position for the cursor at row 1, column 1.

    // DC motor set-up
    P2DIR |= BIT0 | BIT1;       // Motor direction
    P2OUT &= ~(BIT0 | BIT1);    // Default = 0
    P6DIR |= BIT1;              // Set P6.1 as output (ENA pin on L298)
    P6SEL0 |= BIT1;             // Set primary peripheral module function

    TB3CCR0 = 655;                      // PWM Period
    TB3CCTL2 = OUTMOD_7;                // Reset/set
    TB3CCR2 = 0;                        // start with 0% duty
    TB3CTL = TBSSEL_1 | MC_1 | TBCLR;   // ACLK, up mode

    P2OUT |= BIT0;                      // IN1 pin = 1 (motor control)
    P2OUT &= ~BIT1;                     // IN2 pin = 0


    while (1) {
        while ((count = get_key()) == 0);  // Wait for key
        Value = Key_Val[count];

        if (Value == '*') {
            LCD_ClearDisplay();
            LCD_SetCursor(0, 0);
            LCD_Write("Speed: ");
            entered_value = 0;
            is_new_entry = 1;
        }
        // Controls the motor
        else if (Value == '#') {
            if (entered_value > 100) entered_value = 100;
            pwm_duty = entered_value;
            TB3CCR2 = pwm_duty * 6.55;

            // Write the motor speed to the lcd
            LCD_ClearDisplay();
            LCD_SetCursor(0, 0);
            LCD_Write("Speed: ");
            LCD_WriteNum(pwm_duty);
            LCD_Send('%', Rs | LCD_BACKLIGHT);
            is_new_entry = 1;
        }
        else if (Value >= '0' && Value <= '9') {
            if (is_new_entry) {
                    entered_value = Value - '0';
                    is_new_entry = 0;
                } else {
                    entered_value = entered_value * 10 + (Value - '0');
                }

                if (entered_value > 100) entered_value = 100; // Clamp max speed
                pwm_duty = entered_value;
                TB3CCR2 = pwm_duty * 6.55; // Update PWM immediately

                LCD_SetCursor(0, 1);
                LCD_WriteNum(entered_value);
            }


    }   // End of main.

}

unsigned char get_key(void) {            // A number associated with the key they pressed.

    k=1;                                 // k is just a number to increment by 4, so there will be a different return value for every key.
                                         // We start initializing k as 1.

    for(i = 0; i < 4; i++) {
        keyport = ((0x01 << i) ^ 0xff);  // Shift left is setting the nth bit to 1, then it inverts the bit to set it to 0 and other bits to 1.

        if(!COL1) {
            key = k+0;
            while(!COL1);
            DelayMs(50);                 // Add a small delay of 0.05s. Every time we check a row, the button debouncing concept is utilized.
            return key;
        }

        if(!COL2) {
            key = k+1;
            while(!COL2);
            DelayMs(50);
            return key;
        }

        if(!COL3) {
            key = k+2;
            while(!COL3);
            DelayMs(50);
            return key;
        }

        if(!COL4) {
            key = k+3;
            while(!COL4);
            DelayMs(50);
            return key;
        }

    k+=4;
    keyport |= (0x01 << i);

    } // End of for loop.

  return 0;     // Return the unsigned char to main.

 }  // End of UDF get_key.

void DelayMs(unsigned int Ms) {
    while(Ms) {
        __delay_cycles(1000);
        Ms--;
    }
}   // End of DelayMs user-defined function.
