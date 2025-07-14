#include <msp430.h> 

unsigned val=0x00;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer


        // Configure USCI_B0 for I2C mode
        UCB0CTLW0 |= UCSWRST;                   // Software reset enabled
        UCB0CTLW0 |= UCSSEL_3;                  //SMCLK 1MHz
        UCB0BRW = 10;                           // baud rate = SMCLK / 10  for SMCLK=100 KHz)

        UCB0CTLW0 |= UCMODE_3;                  // I2C mode
        UCB0CTLW0 |= UCMST;                     //Master mode
        UCB0CTLW0 |= UCTR;                      //Tx mode
        UCB0I2CSA = 0x0027;                     // Slave address
        UCB0CTLW1 |= UCASTP_2;                  // Automatic stop generated
                                                // after UCB0TBCNT is reached
        UCB0TBCNT = 0x0001;                     // Send 1 byte of data

        // Configure GPIO
        P1SEL1 &= ~BIT3;                         // P1.3  SCL
        P1SEL0 |= BIT3;
        P1SEL1 &= ~BIT2;                         // P1.2   SDA
        P1SEL0 |= BIT2;

        // Disable the GPIO power-on default high-impedance mode
        PM5CTL0 &= ~LOCKLPM5;

        //1 for WRST  take it out of reset
        UCB0CTL1 &= ~UCSWRST;

        UCB0IE |= UCTXIE0;                      //Interrupt enable
        __enable_interrupt();

        int i=0;
        while (1)
        {
         val=0xF0;
         UCB0CTLW0 |= UCTXSTT;
         for(i=0; i<100; i=i+1){}
         __delay_cycles(150000);

         val=0x0F;
         UCB0CTLW0 |= UCTXSTT;
         for(i=0; i<100; i=i+1){}
         __delay_cycles(150000);


        }
          return 0;
 }


    #pragma vector = EUSCI_B0_VECTOR
    __interrupt void EUSCIB0_I2C_ISR(void)
    {


        UCB0TXBUF= val;




      }
