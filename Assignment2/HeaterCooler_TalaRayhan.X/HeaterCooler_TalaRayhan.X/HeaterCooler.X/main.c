/*
 * File:   main.c
 * Author: Tala&Rayhan
 *
 * Created on 30 November 2021, 12:51
 */

// PIC18F4620 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#define _XTAL_FREQ   4000000UL     // needed for the delays, set to 4 MH= your crystal frequency

#pragma config OSC = XT         // Oscillator Selection bits (XT oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "my_adc.h"
#include "my_pwm.h"
#include "lcd_x8.h"
void setupPorts(void);
void __interrupt(high_priority) highIsr(void); //new syntax
void initialization(void);
void reloadTimer3(void);
void pwd_Timer3(int limit);

#define WINTER_T 40
#define SUMMER_T 60
#define STARTVALUE  3036

int percentHeatCounter = 0; // its value 0-10
int percentCoolCounter = 0; // its value 0-10
int operationModes = 0; // 0-4
int timer3Counter = 0; // 0-10
int HS = 0; // 0-5
float sp, ot, temp;
bool heaterON = true;
bool coolerON = false;
int RPS_count = 0;

void initTimers01(void) {
    T0CON = 0;
    //T0CONbits.T0CS = 0;
    //T0CONbits.PSA = 0;
    //T0CONbits.T08BIT = 1;
    INTCONbits.T0IF = 0;
    T0CONbits.T0PS0 = 1; // 16 prescalar
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS2 = 0;
    TMR0H = (unsigned char) ((STARTVALUE >> 8) & 0x00FF);
    TMR0L = (unsigned char) (STARTVALUE & 0x00FF);
    T1CONbits.TMR1CS = 1; //external clock ,emasuring the speed of the fan in RPS
    T1CONbits.T1CKPS1 = 0;
    T1CONbits.T1CKPS0 = 0;
    TMR1H = 0;
    TMR1L = 0;
    INTCONbits.GIE = 1; //enable only timer 0 interrupt
    INTCONbits.T0IE = 1;
    T1CONbits.TMR1ON = 1;
    T0CONbits.TMR0ON = 1;

}

void reloadTimer3(void) {
    TMR3H = 0x9E;   //B1E0
    TMR3L = 0x58; //E0
    PIR2bits.TMR3IF = 0;
}

void initialization(void) {
    PIR1 = 0;
    PIR2 = 0;
    PIE2bits.TMR3IE = 0;
    T3CON = 0;
    PIE2 = 0;
    PIE2bits.TMR3IE = 1; // enable TIMER 3 interrupt 
    T3CONbits.TMR3ON = 1;
    RCONbits.IPEN = 0;
    INTCON3 = 0;
    INTCON3bits.INT1E = 1;
    INTCON3bits.INT2E = 1;
    INTCON2 = 0;
    INTCON = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.INT0E = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.INT0F = 0;
}

void setupPorts(void) {
    ADCON0 = 0;
    ADCON1 = 0b00001100; //3 analog channels
    TRISB = 0xFF; // all pushbuttons are inputs
    TRISC = 0x80; // RX input , others output
    TRISA = 0xFF; // All inputs
    TRISD = 0x00; // All outputs
    TRISE = 0x00; // All outputs
}

void __interrupt(high_priority) highIsr(void)//new syntax
{
    if (PIR2bits.TMR3IF) {
        reloadTimer3();
        if (operationModes == 0) {
            pwd_Timer3(percentHeatCounter);
        }
        //        else if (operationModes == 2) {
        //            pwd_Timer3(5);
        //
        //        }
    } else if (INTCONbits.INT0IF) {
        INTCONbits.INT0IF = 0;
        operationModes = (operationModes + 1) % 3;
        timer3Counter = 0;
        percentCoolCounter = percentHeatCounter = HS = 0;
    } else if (INTCON3bits.INT1IF) {
        INTCON3bits.INT1IF = 0;
        if (operationModes == 0) {
            if (percentHeatCounter != 0) {
                percentHeatCounter = (percentHeatCounter - 1) % 11;
            }
        } else if (operationModes == 1) {
            if (percentCoolCounter != 0) {
                percentCoolCounter = (percentCoolCounter - 1) % 11;
            } else {
                coolerON = false;
            }
        } else if (operationModes == 2) {
            if (HS != 0) {
                HS = (HS - 1) % 6;
            }
        }
        //        if(operationModes > 1){
        //            HS = (HS+1) % 6;
        //        }
    } else if (INTCON3bits.INT2IF) {
        INTCON3bits.INT2IF = 0;
        if (operationModes == 0) {
            if (percentHeatCounter != 10) {
                percentHeatCounter = (percentHeatCounter + 1) % 11;
            }
        } else if (operationModes == 1) {
            if (percentCoolCounter != 10) {
                percentCoolCounter = (percentCoolCounter + 1) % 11;
            }
        } else if (operationModes == 2) {
            if (HS != 5) {
                HS = (HS + 1) % 6;
            }
        }
    } else if (INTCONbits.T0IF) {
        RPS_count = ((unsigned int) TMR1H << 8) | (TMR1L); //

        TMR0H = (unsigned char) ((STARTVALUE >> 8) & 0x00FF);
        TMR0L = (unsigned char) (STARTVALUE & 0x00FF);
        TMR1H = 0;
        TMR1L = 0;
        INTCONbits.T0IF = 0;
    }
}

void pwd_Timer3(int limit) {

    timer3Counter = (timer3Counter) % 11;
    if ((timer3Counter <= limit) && (limit != 0)) {
        PORTCbits.RC5 = 1;
        heaterON = true;
        timer3Counter++;

    } else {
        PORTCbits.RC5 = 0;
        timer3Counter++;
    }
    if (limit == 0) {
        heaterON = false;
    }

}

void main(void) {
    char Buffer[32];
    float analogs[3];
    float voltage;
    int AI1_Percentage;
    initTimers01();
    setupPorts();
    lcd_init();
    init_adc_no_lib();
    init_pwm1();
    reloadTimer3();
    initialization();
    lcd_putc('\f');
    TRISCbits.RC0 = 1; //Timer1 clock

    while (1) {
        CLRWDT();
        for (unsigned char channel = 0; channel < 3; channel++) {
            voltage = read_adc_voltage((unsigned char) channel);
            analogs[channel] = voltage * 100;
        }
        sp = (analogs[0] * 100 / 1023) * 2; // Scale AI0 to 0-100C
        AI1_Percentage = analogs[1] * 100 / 1023; // Scale AI1 to 0-100%
        temp = analogs[2];

        lcd_gotoxy(1, 1);
        sprintf(Buffer, "RT:%03.02fC    H C", temp);
        lcd_puts(Buffer);
        lcd_gotoxy(1, 2);
        sprintf(Buffer, "SP:%03.02fC    ", sp);
        lcd_puts(Buffer);
        lcd_gotoxy(1, 3);
        sprintf(Buffer, "HS:%03.02fC ", 0);
        lcd_puts(Buffer);

        lcd_gotoxy(14, 2);
        if (heaterON)
            lcd_putc('Y');
        else
            lcd_putc('N');
        lcd_gotoxy(16, 2);
        if (coolerON)
            lcd_putc('Y');
        else
            lcd_putc('N');

        switch (operationModes) {
            case 0:
                coolerON = false;
                PORTCbits.RC2 = 0;
                set_pwm1_raw(0);
                lcd_gotoxy(10, 3);
                sprintf(Buffer, " H:%3d", percentHeatCounter * 10);
                lcd_puts(Buffer);
                lcd_putc('%');
                lcd_gotoxy(1, 4);
                sprintf(Buffer, "MD: OFF        ");
                lcd_puts(Buffer);
                break;

            case 1://cool 
                PORTCbits.RC2 = 1;
                heaterON = false;
                if (percentCoolCounter == 0)
                    coolerON = false;
                else
                    coolerON = true;
                PORTCbits.RC5 = 0;
                lcd_gotoxy(10, 3);
                set_pwm1_raw(AI1_Percentage * 1023 / 100 / 2 );
                sprintf(Buffer, " C:%3d", AI1_Percentage* 2 );
                lcd_puts(Buffer);
                lcd_putc('%');
                lcd_gotoxy(1, 4);
                sprintf(Buffer, "MD: Cool       ");
                lcd_puts(Buffer);
                break;

            case 2:
                //PORTCbits.RC2 = 1;
                lcd_gotoxy(11, 4);
                sprintf(Buffer, "  HS=%d", 0);
                lcd_puts(Buffer);
                if (ot > SUMMER_T) {
                    lcd_gotoxy(1, 4);
                    sprintf(Buffer, "MD:Auto CL");
                    lcd_puts(Buffer);
                    int CoolError = temp - sp;
                    if (CoolError > 0) {
                        int PWMPersent = CoolError * 10;
                        if (PWMPersent < 25) {
                            set_pwm1_percent(25);
                        } else if (PWMPersent > 100) {
                            set_pwm1_percent(100);
                        } else {
                            set_pwm1_percent(PWMPersent);
                        }
                        coolerON = true;
                        pwd_Timer3(0);

                    }
                    if (temp < (sp - HS)) {
                        pwd_Timer3(5);

                        coolerON = false;
                        set_pwm1_percent(0);
                    }
                    lcd_gotoxy(10, 3);
                    int RPS = RPS_count;
                    sprintf(Buffer, " R=%05.02f\n", RPS / 7.0); // Display Speed
                    lcd_puts(Buffer); // speed = Revolution per second
                } else if (ot < WINTER_T) {
                    lcd_gotoxy(1, 4);
                    sprintf(Buffer, "MD:Auto HT");
                    lcd_puts(Buffer);
                    int HeatError = sp - temp;
                    if (HeatError > 0) {
                        coolerON = false;  
                        PORTCbits.RC2 = 0;
                        set_pwm1_percent(0);
                        
                        percentHeatCounter = HeatError;
                        if (sp > 52)
                            percentHeatCounter = 10;
                        else if (percentHeatCounter < 5)
                            percentHeatCounter = 5;
                        else if (percentHeatCounter > 10)
                            percentHeatCounter = 10;
                        pwd_Timer3(percentHeatCounter);
                    } else if (temp > (sp + HS)) {
                        heaterON = false;
                        percentHeatCounter = 0;
                        pwd_Timer3(0);
                    }
                    lcd_gotoxy(10, 3);
                    sprintf(Buffer, " H:%3d", percentHeatCounter * 10);
                    lcd_puts(Buffer);
                    lcd_putc('%');
                } else {
                    lcd_gotoxy(1, 4);
                    sprintf(Buffer, "MD:Auto HC");
                    lcd_puts(Buffer);
                    int HeatError1 = sp - temp;
                    if (HeatError1 > 0) {
                        coolerON = false;
                        RC2 = 0;
                        set_pwm1_percent(0);
                        percentHeatCounter = HeatError1;
                        if (sp > 52)
                            percentHeatCounter = 10;
                        else if (percentHeatCounter > 10)
                            percentHeatCounter = 10;
                        else if (percentHeatCounter < 5)
                            percentHeatCounter = 5;
                        pwd_Timer3(percentHeatCounter);
                    } else {
                        int CoolError1 = (temp - sp);
                        int PWMPersent1 = CoolError1 * 10;
                        if (PWMPersent1 < 25) {
                            set_pwm1_percent(25);
                        } else if (PWMPersent1 > 100) {
                            set_pwm1_percent(100);
                        } else {
                            set_pwm1_percent(PWMPersent1);
                        }
                        coolerON = true;
                        pwd_Timer3(0);
                    }
                    lcd_gotoxy(10, 3);
                    int RPS = RPS_count;
                    sprintf(Buffer, " R=%05.02f\n", RPS / 7.0); // Display Speed
                    lcd_puts(Buffer); // speed = Revolution per second
                    break;
                }
        }
    }
    return;
}
