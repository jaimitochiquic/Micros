/*PARTE 3: Sistema de contraseña remota
 * File:   Codigo_Practica6_Parte3.c
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

#include <xc.h>
#define _XTAL_FREQ 48000000 

#define LED  LATDbits.LATD0   
#define RS   LATDbits.LATD1   
#define EN   LATDbits.LATD2   

// ====================================================================
// SUBRUTINAS DE EEPROM
// ====================================================================
unsigned char EEPROM_Read(unsigned char address) {
    EEADR = address;
    EECON1bits.EEPGD = 0; 
    EECON1bits.CFGS = 0;  
    EECON1bits.RD = 1;    
    return EEDATA;
}

void EEPROM_Write(unsigned char address, unsigned char data) {
    EEADR = address; EEDATA = data;
    EECON1bits.EEPGD = 0; EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;  
    INTCONbits.GIE = 0;   
    EECON2 = 0x55; EECON2 = 0xAA;
    EECON1bits.WR = 1;    
    while(EECON1bits.WR); 
    INTCONbits.GIE = 1;   
    EECON1bits.WREN = 0;  
}

// ====================================================================
// SUBRUTINAS DE USART
// ====================================================================
void UART_Initial(long baud_rate){
    float bps; 
    TRISCbits.RC6=1; TRISCbits.RC7=1;
    bps = (( (float) 48000000 / (float) (64*baud_rate)) - 1); 
    SPBRG=(unsigned char)bps; 
    TXSTAbits.CSRC = 0; TXSTAbits.TX9 = 0; TXSTAbits.TXEN = 1; 
    TXSTAbits.SYNC = 0; TXSTAbits.BRGH = 0; BAUDCONbits.BRG16 = 0; 
    RCSTA = 0x90; 
}

void UART_Write(char data){
    TXREG = data; 
    while(TRMT==0); 
}

char USART_Read(){
    while(RCIF==0); 
    if(RCSTAbits.OERR){ 
        CREN = 0; NOP(); CREN = 1; 
        return 255; 
    }
    return(RCREG); 
}

// ====================================================================
// SUBRUTINAS DEL LCD
// ====================================================================
void BitEN(void){ EN = 1; __delay_us(2); EN = 0; __delay_us(50); }

void Tx_DATO(unsigned char DATO, unsigned char is_char){
    unsigned char led_state = LATD & 0x01; 
    RS = is_char; LATD = (DATO & 0xF0) | led_state; RS = is_char; BitEN();
    LATD = ((DATO << 4) & 0xF0) | led_state; RS = is_char; BitEN();
    __delay_ms(2);
}

void LCD_init(void){
    __delay_ms(20); RS = 0;
    Tx_DATO(0x33, 0); Tx_DATO(0x32, 0); Tx_DATO(0x28, 0); 
    Tx_DATO(0x06, 0); Tx_DATO(0x0C, 0); Tx_DATO(0x01, 0); 
    __delay_ms(2); 
}

void LCD_clear(void){ Tx_DATO(0x01, 0); __delay_ms(2); }

void LCD_set_cursor(unsigned char row, unsigned char col){
    unsigned char cmd = (row == 1) ? (0x80 + (col - 1)) : (0xC0 + (col - 1));
    Tx_DATO(cmd, 0);
}

void LCD_print(const char *str){ while(*str) Tx_DATO(*str++, 1); }

void LCD_cursor_blink(char enable) {
    if(enable) Tx_DATO(0x0F, 0); 
    else       Tx_DATO(0x0C, 0); 
}

// ====================================================================
// SUBRUTINAS DEL TECLADO
// ====================================================================
char teclado(void) {
    LATB = 0b01110000; __delay_us(5);
    switch(PORTB) { case 0b01110111: return 1; case 0b01111011: return 2; case 0b01111101: return 3; case 0b01111110: return 10; }
    LATB = 0b10110000; __delay_us(5);
    switch(PORTB) { case 0b10110111: return 4; case 0b10111011: return 5; case 0b10111101: return 6; case 0b10111110: return 11; }
    LATB = 0b11010000; __delay_us(5);
    switch(PORTB) { case 0b11010111: return 7; case 0b11011011: return 8; case 0b11011101: return 9; case 0b11011110: return 12; }
    LATB = 0b11100000; __delay_us(5);
    switch(PORTB) { case 0b11100111: return 14; case 0b11101011: return 0; case 0b11101101: return 15; case 0b11101110: return 13; }
    return 255;
}

char mapear_tecla(char valor) {
    if (valor <= 9) return valor + '0'; 
    if (valor == 10) return 'A'; if (valor == 11) return 'B';
    if (valor == 12) return 'C'; if (valor == 13) return 'D';
    if (valor == 14) return '*'; if (valor == 15) return '#';
    return '?';
}

// ====================================================================
// PROGRAMA PRINCIPAL
// ====================================================================
void main(void){
    // Configuración I/O
    ADCON1 = 0x0F; CMCON = 0x07;
    TRISD = 0x00; LATD = 0x00;  
    TRISB = 0x0F; 
    
    LCD_init(); 
    UART_Initial(9600);
    
    unsigned char v1 = EEPROM_Read(0);
    unsigned char v2 = EEPROM_Read(1);
    unsigned char v3 = EEPROM_Read(2);
    
    if (v1 < '0' || v1 > '9' || v2 < '0' || v2 > '9' || v3 < '0' || v3 > '9') {
        EEPROM_Write(0, '1'); EEPROM_Write(1, '2'); EEPROM_Write(2, '3');
    }
    
    for (char i = 0; i < 10; i++) { LED = 1; __delay_ms(50); LED = 0; __delay_ms(50); }
    LED = 0;
    
    LCD_set_cursor(1, 2); LCD_print("Universidad de"); LCD_set_cursor(2, 6); LCD_print("Cuenca");
    __delay_ms(2000); LCD_clear(); __delay_ms(1000);
    
    LCD_set_cursor(1, 4); LCD_print("USART v1.0");
    __delay_ms(2000); LCD_clear(); __delay_ms(2000); 

    char client_state = 0; 
    char server_state = 0; 
    
    char pass_buffer[3];   
    char pass_index = 0;
    
    char rx_buffer[3];     
    char rx_index = 0;
    
    char comando_pendiente = 0; 
    char tx_index = 0; 

    LCD_set_cursor(1, 1); LCD_print("Remote Password:");
    LCD_set_cursor(2, 1); LCD_cursor_blink(1);

    while(1){
        
        // --- 1. RECEPCIÓN SERIAL ---
        if (RCIF == 1) { 
            char data = USART_Read();
            
            if (data == 255) {
                rx_index = 0; 
                continue;
            }
            
            if (data == 'O' && client_state == 0) { 
                client_state = 1; pass_index = 0;
                LCD_clear(); LCD_set_cursor(1, 1); LCD_print("OK! New Password?");
                LCD_set_cursor(2, 1);
            }
            else if (data == 'X' && client_state == 0) { 
                LCD_cursor_blink(0);
                LCD_clear(); LCD_set_cursor(1, 1); LCD_print("Wrong Password!");
                __delay_ms(2000);
                
                pass_index = 0; 
                LCD_clear(); LCD_set_cursor(1, 1); LCD_print("Remote Password:");
                LCD_set_cursor(2, 1); LCD_cursor_blink(1);
            }
            else if (data == 'S' && client_state == 1) { 
                client_state = 0; pass_index = 0;
                LCD_cursor_blink(0);
                LCD_clear(); LCD_set_cursor(1, 1); LCD_print("New Password");
                LCD_set_cursor(2, 6); LCD_print("set!");
                
                for(unsigned char i = 0; i < 5; i++){ __delay_ms(1000); }
                
                LCD_clear(); LCD_set_cursor(1, 1); LCD_print("Remote Password:");
                LCD_set_cursor(2, 1); LCD_cursor_blink(1);
            }
            else if (data >= '0' && data <= '9') {
                rx_buffer[rx_index++] = data;
                
                if (rx_index == 3) { 
                    rx_index = 0;
                    
                    if (server_state == 0) {
                        if (rx_buffer[0] == EEPROM_Read(0) && rx_buffer[1] == EEPROM_Read(1) && rx_buffer[2] == EEPROM_Read(2)) {
                            UART_Write('O'); 
                            server_state = 1; 
                        } else {
                            UART_Write('X'); 
                        }
                    } 
                    else if (server_state == 1) {
                        EEPROM_Write(0, rx_buffer[0]);
                        EEPROM_Write(1, rx_buffer[1]);
                        EEPROM_Write(2, rx_buffer[2]);
                        UART_Write('S'); 
                        server_state = 0; 
                    }
                }
            }
        }

        // --- 2. LECTURA DE TECLADO ---
        char tecla_actual = teclado();
        if (tecla_actual != 255) {
            __delay_ms(30); 
            if (tecla_actual == teclado()) {
                char tecla_ascii = mapear_tecla(tecla_actual);

                if (tecla_ascii >= '0' && tecla_ascii <= '9') {
                    if (pass_index < 3) {
                        pass_buffer[pass_index] = tecla_ascii;
                        LCD_set_cursor(2, pass_index + 1);
                        Tx_DATO(tecla_ascii, 1);
                        pass_index++;
                    }
                }
                else if (tecla_ascii == '*' && pass_index > 0) {
                    pass_index--;
                    LCD_set_cursor(2, pass_index + 1);
                    Tx_DATO(' ', 1); 
                    LCD_set_cursor(2, pass_index + 1); 
                }
                else if (tecla_ascii == 'D' && pass_index == 3) {
                    comando_pendiente = 1; 
                    tx_index = 0; 
                }
                
                while(teclado() != 255); 
            }
        }
        
        // --- 3. TRANSMISIÓN INTERCALADA (Solución Overrun) ---
        if (comando_pendiente == 1) {
            UART_Write(pass_buffer[tx_index++]);
            if (tx_index >= 3) {
                comando_pendiente = 0;
            }
        }
    }
}