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
#define _XTAL_FREQ   4000000UL     // needed for the delays, set to 4 MH= your crystal frequency
#define TRIG_PIN LATCbits.LATC0
#define ECHO_PIN PORTCbits.RC1





// CONFIG1H
#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "my_adc.h"
#include "my_ser.h"
#include "lcd_x8.h"

void delay_ms(unsigned int n) 
{
    int i;
    for (i=0; i < n; i++){
         __delaywdt_ms(1) ; 
    }
}
void initPorts(void)
{
    ADCON1 = 0x0C; 
    LATA = LATB = LATC = LATD = LATE =0;
    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISD = TRISE =0;
    TRISC = 0x82;
    
}
unsigned int getDistance(){
    unsigned int distance = 0 ;
    unsigned int time =0;
    //send trigger pulse
    TRIG_PIN=1;
    __delay_us(10);
    TRIG_PIN= 0;
    
    while(!ECHO_PIN);
    TMR1=0;
    T1CONbits.TMR1ON = 1;
    while(ECHO_PIN);
    T1CONbits.TMR1ON = 0;
    time = TMR1;
    distance = (time/ 58.82);
    return distance;

    
    
}
void main(void) {
    // Variables
    float voltage;
    int Column_For_LCD=1;
    char Buffer[100];
    unsigned char LastReceivedChar;
    
    // Declaration
    INTCON = 0;
    initPorts();
    setupSerial();
    lcd_init();
    init_adc_no_lib();
 
    
    lcd_putc('\f');//clear lcd
    lcd_gotoxy(1, 1);
    lcd_puts("Tala&Rayhan");
    delay_ms(20000); //20sec
    send_string_no_lib("Tala&Rayhan"); 
    lcd_putc('\f');
   
   
    while(1){ 
        CLRWDT();
        
        lcd_gotoxy(1, 1);
        voltage = read_adc_voltage(0);
        sprintf(Buffer, "V1=%4.2fV ",voltage);
        voltage = read_adc_voltage(1);
        sprintf(Buffer + strlen(Buffer) , "T=%4.2f",((voltage*100)/5));
        lcd_puts(Buffer);

 
        if(is_byte_available()){
           LastReceivedChar = read_byte_no_lib();
           lcd_gotoxy(Column_For_LCD, 2);
           lcd_putc(LastReceivedChar);
           Column_For_LCD++;
           if(Column_For_LCD==16) {
           Column_For_LCD=1;
           lcd_putc('\f'); //clears the display
           }
           
        }
         
    }
    
    return;
}




