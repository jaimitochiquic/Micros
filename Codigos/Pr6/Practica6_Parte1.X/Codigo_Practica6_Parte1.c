/*
 * File:   Codigo_Practica6_Parte1.c
 * Author: JAIME CHIQUI
 * Created on 25 de junio de 2026, 9:11
 */
// PIC18F4550 Configuration Bit Settings    
// 'C' source line config statements

// CONFIG1L
#pragma config PLLDIV = 5       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = HSPLL_HS  // Oscillator Selection bits (HS oscillator, PLL enabled (HSPLL))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = ON        // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOR = ON         // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#define _XTAL_FREQ 48000000                  // Fosc  frequency for _delay()  library

// ============================================================
//                  DELAYS
// ============================================================
void Delay1ms() {
    int veces;
    for (veces = 0; veces <= 860; veces++);
}

void DelayXms(int Milis) {
    for (int i = 0; i <= Milis; i++) {
        Delay1ms();
    }
}
// ============================================================
//              PULSO LCD 
// ============================================================
void BitEN(void){
    // Genera el pulso EN en el LCD
    LATDbits.LATD2 = 1;
    Delay1ms();
    LATDbits.LATD2 = 0;
    Delay1ms();
}
// ============================================================
//          ENVIO DE DATOS: MODO 4 BITS (A1,...,A4)
// ============================================================
void TxDato(char dato){
    /* ejemplo: dato = 0x28 = 00101000, dato & 0x20
     * 00101000
     * 00100000
     * --------
     * 00100000  --> 
     * (dato & 0x20 = 00100000)>>5 (cumple): 00000001  LATAbits.LATA2 = 1;
     */
    // RA1 -> D4; RA2 -> D5; RA3 -> D6; RA4 -> D7
    // -------- Nibble Alto (D7 D6 D5 D4) --------
    LATAbits.LATA1 = (dato & 0x10)>>4;   // D4: ...
    LATAbits.LATA2 = (dato & 0x20)>>5;   // D5
    LATAbits.LATA3 = (dato & 0x40)>>6;   // D6
    LATAbits.LATA4 = (dato & 0x80)>>7;   // D7: 0x80 = bit 7: 2^7

    BitEN();  // PULSO
    // -------- Nibble BAJO --------
    LATAbits.LATA1 = (dato & 0x01);      // D0: 0x01 = bit 0: 2^0
    LATAbits.LATA2 = (dato & 0x02)>>1;   // D1: 0x02 = bit 1: 2^1
    LATAbits.LATA3 = (dato & 0x04)>>2;   // D2: ..
    LATAbits.LATA4 = (dato & 0x08)>>3;   // D3

    BitEN(); // PULSO
    Delay1ms();
}
// ============================================================
//              COMANDOS LCD
// ============================================================
void LCD_Comando(char cmd){
    // RD1 -> RS = 0; 
    LATDbits.LATD1 = 0;   //RS = 0 : comando
    TxDato(cmd);          //RS = 1 : dato
}
// ============================================================
//              ENVIO DE CARACTER LCD
// ============================================================
void LCD_Caracter(char dato){
    LATDbits.LATD1 = 1;
    TxDato(dato);
}

void LCD_Texto(char *PALABRA){
    // Recorre una cadena de caracteres (PALABRAS)
    // hasta encontrar el caracter nulo '\0'
     for(int i=0; PALABRA[i]; i++){
        LCD_Caracter(PALABRA[i]);
    }
}
// ============================================================
//              INICIALIZACION LCD
// ============================================================
void InitLCD(void){
    // Inicializacion para LCD 16x2, MODO 4 BITS
    DelayXms(15);  // >=15 ms

    // lcd NO sabe si trabajará en: 8 bits o 4 bits
    LCD_Comando(0x33);  //0011 = 3 HEX 
    LCD_Comando(0x32);  //0010 = 2 HEX

    LCD_Comando(0x28);   // ES: 4 bits, 2 lineas
    LCD_Comando(0x0C);   // Display ON
    LCD_Comando(0x06);   // Incremento cursor, letra desplaza +1 a la derecha
    // LCD_Comando(0x04); // Decremento cursor
    // LCD_Comando(0x07); // Cursor avanza a la derecha y toda la pantalla se desplaza.
    LCD_Comando(0x01);   // Clear LCD

    DelayXms(5);
}
// ============================================================
//              MOSTRAR SEGUNDA FILA LCD
// ============================================================
void LCD_Linea2(void){
    LCD_Comando(0xC0); // Cursor en segunda fila
}
// ============================================================
//      MOSTRAR MENSAJE EN DOS LINEAS LCD
// ============================================================
void LCD_DosLineas(char *Linea1, char *Linea2){
    LCD_Comando(0x01);  // Clear LCD
    LCD_Texto(Linea1);
    LCD_Linea2();      // Cursor Segunda Fila LCD
    LCD_Texto(Linea2); 
}
// ============================================================
//          LEER TECLADO MATRICIAL (Filas RB4-RB7, Col RB0-RB3)
// ============================================================
char LeerTeclado(void) {
    char tecla = 0xFF; // 0xFF: Nada presionado

    // --------------------------------------------------------
    LATB = 0b01110000; // Fila 1
    DelayXms(5); // Tiempo espera
    switch (PORTB) {
            // f1_input: 0111  f1_out: xxxx
        case 0b01110111: tecla = 1; break;
        case 0b01111011: tecla = 2; break;
        case 0b01111101: tecla = 3; break;
        case 0b01111110: tecla = 10; break; // Tecla 'A'
    }

    // --------------------------------------------------------
    LATB = 0b10110000; // Fila 2:
    DelayXms(5);
    switch (PORTB) {
            // f2_input: 1011  f2_out: xxxx
        case 0b10110111: tecla = 4; break;
        case 0b10111011: tecla = 5; break;
        case 0b10111101: tecla = 6; break;
        case 0b10111110: tecla = 11; break; // Tecla 'B'
    }
    // --------------------------------------------------------
    LATB = 0b11010000; // Fila 3
    DelayXms(5);
    switch (PORTB) {
            // f3_input: 1101  f3_out: xxxx
        case 0b11010111: tecla = 7; break;
        case 0b11011011: tecla = 8; break;
        case 0b11011101: tecla = 9; break;
        case 0b11011110: tecla = 12; break; // Tecla 'C'
    }
    // -------------------------------------------------------
    LATB = 0b11100000; // Fila 4:
    DelayXms(5);
    switch (PORTB) {
            // f4_input: 1110  f4_out: xxxx
        case 0b11100111: tecla = 14; break; // Tecla '*'
        case 0b11101011: tecla = 0; break;  // 0
        case 0b11101101: tecla = 15; break; // Tecla '#'
        case 0b11101110: tecla = 13; break; // Tecla 'D'
    }
    // Anti-rebote de 50ms
    if (tecla != 0xFF) { // presiona algo
        DelayXms(50);
        return tecla;
    }
    return tecla; //tecla ingresada
}
// ************************************************************
//          COMUNICACION UART
// ************************************************************
void __interrupt(high_priority) MyHighPriorityIsr(void);
//void __interrupt(low_priority) MyLowPriorityIsr(void);

void UART_Initial(long baud_rate){
    float bps; 
    TRISCbits.RC6=1;
    TRISCbits.RC7=1;
    bps = (( (float) 48000000 / (float) (64*baud_rate)) - 1); // sets baud rate
    SPBRG=(int)bps; // store value for 9600 baud rate
    TXSTAbits.CSRC = 0; // Don't care for asynchronous mode
    TXSTAbits.TX9 = 0; // Selects 8-bit data transmission
    TXSTAbits.TXEN = 1; // Enable Data tranmission on RC6 pin
    TXSTAbits.SYNC = 0; // Selects Asynchronous Serial Communication Mode
    TXSTAbits.BRGH = 0; // Default Baud rate speed 
    BAUDCONbits.BRG16 = 0; // selects only 8 bit register for baud rate 
    RCSTA = 0x90; // Enables UART Communication PORT
}
void UART_Write( char data ){
    TXREG = data; 
    while(TRMT==0); // wait until transmit register not empty
}
char USART_Read(){
    while(RCIF==0); // see if data is available 
    if(RCSTAbits.OERR){ 
        CREN = 0;
        NOP();
        CREN = 1;
    }
    return(RCREG); //read the byte from receive register and return value
}


void main(void) {
    // ============= Port B - Teclado 4*4 Matricial ==========
    TRISB = 0b00001111; // 0xFF: Nada presionado
    LATB = 0b11110000; // filas en 1
    
    ADCON1 = 0x0F;
    
    //Configure I/O ports
    TRISDbits.RD0 = 0;
    LATDbits.LATD0 = 0;    
    
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;

    TRISDbits.RD1 = 0;
    TRISDbits.RD2 = 0;
    
    for (int i = 1; i<10;i++){
        LATDbits.LATD0 = ~LATDbits.LATD0;
        DelayXms(50);        
    }
    LATDbits.LATD0 = 0; // led off
    LATDbits.LATD0 = 0;    
    UART_Initial(9600); 
    InitLCD();
    
    // Universidad de Cuenca >> LONGUIUD DE 16 (USA 2 FILAS)
    LCD_DosLineas("Universidad de", "Cuenca");
    DelayXms(2000);     // Espera 2 [seg]
    LCD_Comando(0x01);  // Clear LCD
    DelayXms(1000);     // Espera 1 [seg]
    
    LCD_Linea2(); LCD_Texto("USART v1.0");
    DelayXms(2000);     // Espera 2 [seg] 
    LCD_Comando(0x01);  // Clear LCD
    DelayXms(1000);     // Espera 1 [seg]
    
    //--------------------------------------------------------------------------    
    //Fin de Configuraci?n de Perif?ricos	
    //***********************************************************          
    while(1){    
        
        UART_Write('A'); // Send character A   
        __delay_ms(1000);        
 
        
        char data= USART_Read();
        if(data=='A'){
            LATDbits.LATD0 = 1;
        }
        else{
            LATDbits.LATD0 = 0;
        }        
    
    }
    
    return;
}

void __interrupt(high_priority) MyHighPriorityIsr(void){        // High priority interrupt
    //stuff  
    PIR1bits.TMR2IF = 0;
                                    
}

void __interrupt(low_priority) MyLowPriorityIsr(void){
    // stuff
}