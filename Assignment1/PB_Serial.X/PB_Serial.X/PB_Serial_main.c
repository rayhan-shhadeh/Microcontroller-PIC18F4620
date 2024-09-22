/*
 * File:   PB_Serial_main.c
 * Author: HP
 *
 * Created on June 3, 2024
 */
// CONFIG1H
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

#define _XTAL_FREQ   4000000UL 

void delay_ms(unsigned int n)
{
    int i;
    for (i=0; i < n; i++){
         __delaywdt_ms(1) ; 
    }
}

void setupSerial(void) {
    unsigned char dummy;
    BAUDCONbits.BRG16 = 0;
    TXSTA = 0;
    SPBRG = 25;
    SPBRGH = 0;
    TXSTAbits.BRGH = 1; //baud rate high speed option
    TXSTAbits.TXEN = 1; //	;enable transmission

    RCSTA = 0; // ;SERIAL RECEPTION W/ 8 BITS,
    RCSTAbits.CREN = 1; //;enable reception
    RCSTAbits.SPEN = 1;
    ; //enable serial port
    dummy = RCREG; //, W        ; clear the receiver buffer      
    dummy = RCREG; //,W         ; clear the receiver buffer
    return;
}

// This function is used to prevent busy waiting
unsigned char is_byte_available(void)
{
    if (RCSTAbits.FERR || RCSTAbits.OERR)//check for error
    {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }
    // wait until byte available
    if (PIR1bits.RCIF) return 1;
    else return 0;
    
}

unsigned char read_byte_no_lib(void) {
    unsigned char c;
    c = RCREG;
    return c;
}

void send_byte_no_lib(unsigned char c) {
    while (!TXSTAbits.TRMT)//fixed 
    {
        CLRWDT(); //if enabled  
    }
    TXREG = c;
}

void send_string_no_lib(unsigned char *p) {
    while (*p) {
        send_byte_no_lib(*p); //or use the send_byte_no_lib()
        p++;
    }
}
void initPorts(void)
{
    ADCON1 = 0x0C;
    LATA = LATB = LATC = LATD = LATE =0;
    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISD = TRISE =0;
    TRISC = 0b10000000;//0x80
    
}

#include <xc.h>

void main(void) {
    ADCON1 = 0x0C;
    LATA = LATB = LATC = LATD = LATE =0;
    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISD = TRISE =0;
    TRISC = 0b10000000;//0x80
    INTCON = 0;
    INTCON2 =0 ;
    setupSerial();
    
    int i;
    
    send_string_no_lib((unsigned char *)"Rayhan Shhadeh & Tala Zayed assignment#1\r\n");
    char x;
    char buffer [200];
    
    while(1 ){ 
        //control by serial
        unsigned char *Status ;
        CLRWDT();
        if(is_byte_available()){
            x = read_byte_no_lib();
                switch(x){
                case'g': 
                    //g Read PORTD , display as hex and binary
                    sprintf(buffer, "LEDs Hex: 0x%02X\r\n", PORTD);
                    send_string_no_lib(buffer);
                    sprintf(buffer, "LEDs Bin: %d%d%d%d%d%d%d%d\r\n",
                    (PORTD & 0x80) ? 1 : 0,
                    (PORTD & 0x40) ? 1 : 0,
                    (PORTD & 0x20) ? 1 : 0,
                    (PORTD & 0x10) ? 1 : 0,
                    (PORTD & 0x08) ? 1 : 0,
                    (PORTD & 0x04) ? 1 : 0,
                    (PORTD & 0x02) ? 1 : 0,
                    (PORTD & 0x01) ? 1 : 0);
                    send_string_no_lib(buffer);        
                    break;
                case'G':
                    //G: Read Heater, Cooler(Fan), Relay1 and Relay 2
                    //Heater
                    if(PORTCbits.RC5)
                        Status="ON.\r\n";
                    else
                        Status="OFF.\r\n"; 
                    send_string_no_lib((unsigned char *)"Heater: ");
                    send_string_no_lib(Status);
                    //Cooler(Fan)
                    if(PORTCbits.RC2)
                        Status="ON.\r\n";
                    else
                        Status="OFF.\r\n";
                    send_string_no_lib((unsigned char *)"Cooler: ");
                    send_string_no_lib(Status);
                    //Relay1 
                    if(PORTCbits.RC0)
                        Status="ON.\r\n";
                    else
                        Status="OFF.\r\n"; 
                    send_string_no_lib((unsigned char *)"Relay1: ");
                    send_string_no_lib(Status);
                    //Relay 2
                    if(PORTEbits.RE0)
                        Status="ON.\r\n";
                    else
                        Status="OFF.\r\n"; 
                    send_string_no_lib((unsigned char *)"Relay2: ");
                    send_string_no_lib(Status);
                      break;
                case 'h':
                    PORTCbits.RC5 = 0 ;// Turn OFF Heater, and Turn ON Cooler
                    PORTCbits.RC2 = 1; //Cooler
                    break;
                case 'H':
                    // Turn ON Heater, and Turn OFF Cooler
                    PORTCbits.RC5 = 1 ;// Turn ON Heater, and Turn OFF Cooler
                    PORTCbits.RC2 = 0; //OFF Cooler
                    break;
                case 'c':
                    // Turn Relay1 OFF
                    PORTCbits.RC0 = 0 ;//Relay1
                    break;
                case 'C':
                    // Turn Relay1 ON
                    PORTCbits.RC0 = 1; //Relay2
                    break;
                case 'r':
                    // Turn Relay2 OFF
                    PORTEbits.RE0 = 0; //Relay2
                    break;
                case 'R':
                    // Turn Relay2 ON
                    PORTEbits.RE0 = 1; //Relay2
                    break;
                case 'd':
                    // Toggle PORTD LEDs 0,1 and 6,7 on PORTD
                    PORTDbits.RD0 = !PORTDbits.RD0;
                    PORTDbits.RD1 = !PORTDbits.RD1;
                    PORTDbits.RD6 = !PORTDbits.RD6;
                    PORTDbits.RD7 = !PORTDbits.RD7;
                    break;
                case 'D':
                    // Toggle PORTD LEDs 2,3,4,5 on PORTD
                    PORTDbits.RD2 = !PORTDbits.RD2;
                    PORTDbits.RD3 = !PORTDbits.RD3;
                    PORTDbits.RD4 = !PORTDbits.RD4;
                    PORTDbits.RD5 = !PORTDbits.RD5;
                    break;
                case 'z':
                    // Turn Everything OFF
                    PORTDbits.RD0 = 0;
                    PORTDbits.RD1 = 0;
                    PORTDbits.RD2 = 0;
                    PORTDbits.RD3 = 0;
                    PORTDbits.RD4 = 0;
                    PORTDbits.RD5 = 0;
                    PORTDbits.RD6 = 0;
                    PORTDbits.RD7 = 0;
                    
                    PORTCbits.RC5 = 0 ;// Turn OFF Heater, and Turn OFF Cooler
                    PORTCbits.RC2 = 0; //Cooler
                    
                    PORTCbits.RC0 = 0; //Relay2
                    PORTEbits.RE0 = 0; //Relay1
                    break;
                    default:
                    break;
            }
        }
        //end serial 
        
        //RB0 : Clear. Turn Everything OFF 
        //Heater, Fan, Relay1,Relay2, and all LEDs on PORTD
        if(!PORTBbits.RB0){
            //Do action
            PORTA=PORTC=PORTD= 0 ;
            //DEBOUNCE
            delay_ms(250);   
        }
        //RB1 : Turn Off Heater, and Turn ON Cooler
        else if(!PORTBbits.RB1){
            //Do action
            PORTCbits.RC5 = 0 ;//Heater
            PORTCbits.RC2 = 1; //Cooler
            //DEBOUNCE
            delay_ms(250);   
        }
        //RB2 : Turn ON Heater, and Turn OFF Cooler
        else if(!PORTBbits.RB2){
            //Do action
            PORTCbits.RC5 = 1 ;//Heater
            PORTCbits.RC2 = 0; //Cooler 
            //DEBOUNCE
            delay_ms(250);   
        }
        //RB3 : Toggle PORTD LEDs 0,1 and 6, 7 on PORTD
        else if(!PORTBbits.RB3){
            //Do action
            PORTDbits.RD0 = !PORTDbits.RD0 ;
            PORTDbits.RD1 = !PORTDbits.RD1 ;
            PORTDbits.RD6 = !PORTDbits.RD6 ;
            PORTDbits.RD7 = !PORTDbits.RD7 ;
            //DEBOUNCE
            delay_ms(250);   
        }
        //RB4 : Toggle PORTD LEDs 2,3,4,5 on PORTD
        else if(!PORTBbits.RB4){
            //Do action
            PORTDbits.RD2 = !PORTDbits.RD2 ;
            PORTDbits.RD3 = !PORTDbits.RD3 ;
            PORTDbits.RD4 = !PORTDbits.RD4 ;
            PORTDbits.RD5 = !PORTDbits.RD5 ;
            //DEBOUNCE
            delay_ms(250);   
        }
        //RB5 : Toggle Relay1 and Relay 2 ON/OFF
        else if(!PORTBbits.RB5){
            //Do action
            PORTCbits.RC0 = !PORTCbits.RC0 ;//Relay1
            PORTEbits.RE0 = !PORTEbits.RE0; //Relay2
            //DEBOUNCE
            delay_ms(250);   
        }
        
    }
       
   return;
}
