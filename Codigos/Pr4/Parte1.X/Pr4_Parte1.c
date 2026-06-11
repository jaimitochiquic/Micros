/*
 * File:   Pr3_Codigo_Parte2.c
 * Author: JAIME CHIQUI
 *
 * Created on 28 de mayo de 2026, 10:42
 */
//  PRACTICA 3 - PARTE 1: Teclado Matricial 4x4 y Mult. Displays
// simulando el ingreso de datos de una calculadora/microondas
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
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
// ** Apagar LVP para no tener corriente parasita ******************

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

// ============================================================
//                  VARIABLES GLOBALES
// ============================================================
volatile char datos[4]   = {0x00, 0x00, 0x00, 0x00};
volatile char multiplex  = 0;
volatile char digitos[4]   = {0, 0, 0, 0};

// Codigo de los numeros 0-9 (cátodo común, [a b c d e f g dp])
const char codigosNum[12] = {
    0b11111100, // 0
    0b01100000, // 1
    0b11011010, // 2
    0b11110010, // 3
    0b01100110, // 4
    0b10110110, // 5
    0b10111110, // 6
    0b11100000, // 7
    0b11111110, // 8
    0b11110110, // 9
    0b00101010, // n
    0b10001110  // F
};

// ============================================================
//                  DELAYS
// ============================================================
void Delay1ms(){
    int veces; 
    for ( veces=0; veces <= 860; veces++ );        
}

void DelayXms(int Milis){
    for(int i=0; i<= Milis; i++){
        Delay1ms();
    }    
}

// ============================================================
//          ACTUALIZAR DISPLAYS TECLA INGRESADA
// ============================================================
void ActualizarDisplays(void){
    datos[0] = codigosNum[(int)digitos[0]]; // D1 
    datos[1] = codigosNum[(int)digitos[1]]; // D2
    datos[2] = codigosNum[(int)digitos[2]]; // D3
    datos[3] = codigosNum[(int)digitos[3]]; // D4
}
// ============================================================
//      DESPLAZAR VALORES A LA IZQUIERDA + TECLA INGRESADA
// ============================================================
void Desp_izquierda(char tecla_ingresada){
    digitos[0] = digitos[1];    // D1
    digitos[1] = digitos[2];    // D2
    digitos[2] = digitos[3];    // D3
    digitos[3] = tecla_ingresada; //D4
    ActualizarDisplays();
}
// ============================================================
//          BUZZER 
// ============================================================
void Buzzer(void){  // Buzzer pin 2 pic
    LATAbits.LATA0 = 1; // ON buzzer
    DelayXms(100);      // Delay 100ms
    LATAbits.LATA0 = 0; // OFF buzzer
} 
// ============================================================
//          LEER TECLADO MATRICIAL (Filas RB4-RB7, Col RB0-RB3)
// ============================================================
char LeerTeclado(void){
    char tecla = 0xFF; // 0xFF: Nada presionado

    // --------------------------------------------------------
    // Fila 1: RB7=0, RB6=1, RB5=1, RB4=1 -> 0b01110000
    // --------------------------------------------------------
    LATB = 0b01110000;
    DelayXms(5); // Tiempo espera
    switch (PORTB) {
        // f1_input: 0111  f1_out: xxxx
        case 0b01110111: tecla = 1; break;
        case 0b01111011: tecla = 2; break;
        case 0b01111101: tecla = 3; break;
        case 0b01111110: tecla = 10; break; // Tecla 'A'
    }
    // Anti-rebote de 50ms
    if(tecla != 0xFF) { // presiona algo
        DelayXms(50); 
        return tecla; 
    }
    // --------------------------------------------------------
    // Fila 2: RB7=1, RB6=0, RB5=1, RB4=1 -> 0b10110000
    // --------------------------------------------------------
    LATB = 0b10110000;
    DelayXms(5);
    switch (PORTB) {
        // f2_input: 1011  f2_out: xxxx
        case 0b10110111: tecla = 4; break;
        case 0b10111011: tecla = 5; break;
        case 0b10111101: tecla = 6; break;
        case 0b10111110: tecla = 11; break; // Tecla 'B'
    }
    // Anti-rebote de 50ms
    if(tecla != 0xFF) { // presiona algo
        DelayXms(50); 
        return tecla; 
    }

    // --------------------------------------------------------
    // Fila 3: RB7=1, RB6=1, RB5=0, RB4=1 -> 0b11010000
    // --------------------------------------------------------
    LATB = 0b11010000;
    DelayXms(5);
    switch (PORTB) {
        // f3_input: 1101  f3_out: xxxx
        case 0b11010111: tecla = 7; break;
        case 0b11011011: tecla = 8; break;
        case 0b11011101: tecla = 9; break;
        case 0b11011110: tecla = 12; break; // Tecla 'C'
    }
    // Anti-rebote de 50ms
    if(tecla != 0xFF) { // presiona algo
        DelayXms(50); 
        return tecla; 
    }

    // --------------------------------------------------------
    // Fila 4: RB7=1, RB6=1, RB5=1, RB4=0 -> 0b11100000
    // --------------------------------------------------------
    LATB = 0b11100000;
    DelayXms(5);
    switch (PORTB) {
        // f4_input: 1110  f4_out: xxxx
        case 0b11100111: tecla = 14; break; // Tecla '*'
        case 0b11101011: tecla = 0; break;  // 0
        case 0b11101101: tecla = 15; break; // Tecla '#'
        case 0b11101110: tecla = 13; break; // Tecla 'D'
    }
    // Anti-rebote de 50ms
    if(tecla != 0xFF) { // presiona algo
        DelayXms(50); 
        return tecla; 
    }
    return 0xFF; // Ninguna tecla ingresada
}
// ============================================================
//          ISR ALTA PRIORIDAD - TMR0 16 bits
// ============================================================
void __interrupt(high_priority) InterrupcionHP(void){
    if(INTCONbits.TMR0IF){
        // == INTERRUPCIONES DE 5 ms ==
    // Tiempo total del ciclo = 4 displays *  5 ms = 20 ms
    // ==========================================================
    // TMR0: 5ms (Fosc=48MHz, 16-bit, Prescaler 1:8)
    // ==========================================================
    // Tcy = 1 / (48MHz / 4) = 83.33ns
    // Tick = Tcy * 8 (Prescaler) = 666.67ns
    // Conteos = 5ms / 666.67ns = 7500
    // Carga = 65536 - 7500 = 58036 = 0xE2B4
        TMR0H = 0xE2;  // Parte alta
        TMR0L = 0xB4;  // Parte baja
        
        // OFF DISPLAY(evita ghosting)
        LATCbits.LATC7 = 0; // D1
        LATCbits.LATC6 = 0; // D2
        LATCbits.LATC1 = 0; // D3
        LATCbits.LATC0 = 0; // D4
            
        // Multiplexación
        LATD = (datos[multiplex] & 0xFE) | (LATDbits.LATD0);
        switch(multiplex){
            case 0: LATCbits.LATC7 = 1; break;
            case 1: LATCbits.LATC6 = 1; break;
            case 2: LATCbits.LATC1 = 1; break;
            case 3: LATCbits.LATC0 = 1; break;
        }
        multiplex++;
        if(multiplex > 3) multiplex = 0;
        
        INTCONbits.TMR0IF = 0;
    }
}

// ============================================================
//                      MAIN
// ============================================================
void main(void){
    // --- CONFIGURACIÓN CRÍTICA PARA USAR RC4 y RC5 ---
    UCONbits.USBEN = 0;   // Deshabilita el módulo USB
    UCFGbits.UTRDIS = 1;  // Deshabilita el transceptor USB (E/S digital)
    ADCON1 = 0x0F;        // Buzzer en RA0
    // -------------------------------------------------

    // --- Configuración de Puertos ---
    TRISD = 0;        
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 0;
    // TRISC |= 0b00110000;  // Activar s2 y s3
    
    // ============= PORT B - Teclado 4*4 Matricial ==========
    // RB0-RB3: Entradas (Columnas), RB4-RB7: Salidas (Filas)
    TRISB = 0b00001111; // // 0xFF: Nada presionado
    LATB = 0b11110000;    // filas en 1
    // INTCON2bits.RBPU = 0; // Resist. Pull-Up internas PIC(NO fisicas) °°°°
    
    LATD = 0;
    LATCbits.LATC7 = 0; 
    LATCbits.LATC6 = 0;
    LATCbits.LATC1 = 0;
    LATCbits.LATC0 = 0;
    
    // Port A para Buzzer
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 0;
    
       // ==== INTERRUPCIONES DE 5 ms ====
    // Tiempo total del ciclo = 4 displays *  5 ms = 20 ms
    // ==========================================================
    // TMR0: 5ms (Fosc=48MHz, 16-bit, Prescaler 1:8)
    // ==========================================================
    // Tcy = 1 / (48MHz / 4) = 83.33ns
    // Tick = Tcy * 8 (Prescaler) = 666.67ns
    // Conteos = 5ms / 666.67ns = 7500
    // Carga = 65536 - 7500 = 58036 = 0xE2B4
    TMR0H = 0xE2;  // Parte alta
    TMR0L = 0xB4;  // Parte baja
    T0CON = 0b00000010;
    
    // Configuración del T0CON:
    // bit 7: TMR0ON = 0 (Apagado por ahora)
    // bit 6: T08BIT = 0 (Configurado en 16 bits)
    // bit 5: T0CS   = 0 (Reloj interno Fosc/4)
    // bit 4: T0SE   = 0 (No reloj interno)
    // bit 3: PSA    = 0 (Prescaler activado)
    // bit 2-0: T0PS = 010 (Prescaler 1:8)

    RCONbits.IPEN      = 1;
    INTCON2bits.TMR0IP = 1;
    INTCONbits.TMR0IE  = 1;
    INTCONbits.TMR0IF  = 0;
    INTCONbits.GIEH    = 1;
    INTCONbits.GIEL    = 1;
    T0CONbits.TMR0ON   = 1;

    // INICIO -  LED RD0 **********************
    for(int i = 0; i < 10; i++){
        LATDbits.LATD0 = !LATDbits.LATD0;
        // delay bloqueante: L = entero largo (desborde)
        for(long d = 0; d < 43000L; d++); // ~50ms a 48MHz
    }
    LATDbits.LATD0 = 1;
    
    ActualizarDisplays(); //0000
    char valor_tecla;

    while(1){
        valor_tecla = LeerTeclado();
        
        if(valor_tecla != 0xFF){ // Se ingreso algo
            // Filtrar que se ingresen solo numeros 0-9
            if(valor_tecla <= 9){   
                Desp_izquierda(valor_tecla); // Nuevo valor a la izq.
                Buzzer();             // ON: 100ms
            }
            while(LeerTeclado() != 0xFF); // Antirebote Teclado 4*4
        }
    }
    return;
}