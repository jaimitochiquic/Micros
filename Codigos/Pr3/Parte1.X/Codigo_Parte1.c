/*
 * File:   Codigo_Parte1.c
 * Author: JAIME CHIQUI
 *
 * Created on May 26, 2026, 3:56 PM
 */
//  PRACTICA 3 - PARTE 1 
// -- HoLA cor TMR0 e interrupciones
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
volatile char datos[4]    = {0x00, 0x00, 0x00, 0x00}; // 4 DISPLAYS OFF
volatile char multiplex   = 0;   // digito activo 0-3
volatile unsigned char visDisplay  = 1;   // 1=mostrar  0=apagar displays
volatile char estado = 0; // 0 = Fijo (S3), 1 = Parpadeo (S2)

// Matriz de números 0-9 
// Orden de bits: [a b c d e f g dp] - CAT COMUN
const char codigosNum[10] = {
    0b11111100, // 0
    0b01100000, // 1
    0b11011010, // 2
    0b11110010, // 3
    0b01100110, // 4
    0b10110110, // 5
    0b10111110, // 6
    0b11100000, // 7
    0b11111110, // 8
    0b11110110  // 9
};
// Matriz de letras (A, b, C, d, E, F, G, H, h, i, J, L, n, o, P, q, r, S, t, U, u, y)
const char codigosLetras[] = {
    0b11101110, // A
    0b00111110, // b
    0b10011100, // C
    0b01111010, // d
    0b10011110, // E
    0b10001110, // F
    0b10111110, // G
    0b01101110, // H  <- ind 7
    0b00101110, // h
    0b00100000, // i
    0b01111000, // J
    0b00011100, // L  <- ind 11
    0b00101010, // n
    0b00111010, // o  <- ind 13
    0b11001110, // P
    0b11110110, // q
    0b00001010, // r
    0b10110110, // S
    0b00011110, // t
    0b01111100, // U
    0b00111000, // u
    0b01110110  // y
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
void MostrarSecuencia(void){
    int d, n;
    // Recorre los 4 displays: 0=D1, 1=D2, 2=D3, 3=D4
    for(d = 0; d < 4; d++){
        for(n = 0; n <= 9; n++){
            // Pone el numero en el display activo, el resto apagado
            datos[0] = 0x00;
            datos[1] = 0x00;
            datos[2] = 0x00;
            datos[3] = 0x00;
            datos[d] = codigosNum[n];   // solo el display d muestra n
            DelayXms(250);              // 500ms por numero
        }
        // Display se apaga al terminar (datos[d] queda en 9, luego se limpia)
        datos[d] = 0x00;
        DelayXms(100);                  // pausa apagado entre displays
    }
}

void __interrupt(high_priority) InterrupcionHP(void){
// ============================================================
// ISR ALTA PRIORIDAD - TMR0 16Bits
// ============================================================
    if(INTCONbits.TMR0IF){
        if(estado == 0){
            // MODO S3: Carga 2.5ms (HoLA se ve sólido y fijo sin parpadear)
            TMR0H = 0xF1; TMR0L = 0x5A;
        } else{
            // -----------------------------------------------------
            //              2.5 [ms]
            // -----------------------------------------------------
            // TMR0H = 0xF1; TMR0L = 0x5A;

            //-----------------------------------------------------
            //              5 [ms]
            // -----------------------------------------------------
            // TMR0H = 0xE2; TMR0L = 0xB4;

            // -----------------------------------------------------
            //              10 [ms] 
            // -----------------------------------------------------
            // TMR0H = 0xC5; TMR0L = 0x68;

            //======================================================
            //              25[ms]  
            //======================================================
            // TMR0H = 0x6D; TMR0L = 0x84;
            //======================================================
            //              50 [ms]  
            //======================================================
            TMR0H = 0xB6; TMR0L = 0x82;
        }
        // Evita superposición
        LATCbits.LATC7 = 0; 
        LATCbits.LATC6 = 0; 
        LATCbits.LATC1 = 0; 
        LATCbits.LATC0 = 0;
        // Multiplexacion
        if(visDisplay){
            LATD = (datos[multiplex] & 0xFE) | (LATDbits.LATD0);
            // 0xFE = 11111110 -> limpia bit0 (dp) pero conserva RD0 del LED

            switch(multiplex){
                case 0: LATCbits.LATC7 = 1; break; // D1
                case 1: LATCbits.LATC6 = 1; break; // D2
                case 2: LATCbits.LATC1 = 1; break; // D3
                case 3: LATCbits.LATC0 = 1; break; // D4
            }
        } else {
            LATD = 0x00 | (LATDbits.LATD0);
        }
        multiplex++;
        if(multiplex > 3) multiplex = 0;
        
        INTCONbits.TMR0IF = 0;
        return;
    }
}

// ============================================================
//                  MAIN
// ============================================================
void main(void){    
    // --- CONFIGURACIÓN CRÍTICA PARA USAR RC4 y RC5 ---
    UCONbits.USBEN = 0;   // Deshabilita el módulo USB
    UCFGbits.UTRDIS = 1;  // Convierte a E/S digital
    // -------------------------------------------------

    // Configuracion de entrada / salida
    TRISD = 0;                  // Puerto D todo salida
    TRISCbits.TRISC0 = 0;       // RC0=D4 salida
    TRISCbits.TRISC1 = 0;       // RC1=D3 salida
    TRISCbits.TRISC6 = 0;       // RC6=D2 salida
    TRISCbits.TRISC7 = 0;       // RC7=D1 salida
    TRISC |= 0b00110000;        // RC4=S2 y RC5=S3 entradas(en 1)
    
    // OFF Display (evita ghosting: int. repentina)
    LATD = 0;
    LATCbits.LATC7 = 0; // D1
    LATCbits.LATC6 = 0; // D2
    LATCbits.LATC1 = 0; // D3
    LATCbits.LATC0 = 0; // D4
    
        if(estado == 0){
            // MODO S3: Carga 2.5ms (HoLA se ve sólido y fijo sin parpadear)
            TMR0H = 0xF1; TMR0L = 0x5A;
        } else{
            //======================================================
            // Recargar TMR0 segun el tiempo elegido
            //======================================================
            // Fosc=48MHz  Tcy=1/12MHz=83.33ns
            // casos 1,2,3,4 -> Prescaler 1:8  Tpre=666.67ns
            // caso 5        -> Prescaler 1:32 Tpre=2666.67ns
            // Carga = 65536 - (Tiempo / Tpre)

            // -----------------------------------------------------
            //              2.5 [ms]
            // -----------------------------------------------------
            // Conteo = 2.5ms  / 666.67ns = 3750
            // Carga  = 65536  - 3750     = 61786 = 0xF15A
            // TMR0H = 0xF1; TMR0L = 0x5A;

            //-----------------------------------------------------
            //              5 [ms]
            // -----------------------------------------------------
            // Instrucciones = 5ms / 666.67ns = 7500
            // Carga  = 65536  - 7500 = 58036 = 0xE2B4
            //TMR0H = 0xE2; TMR0L = 0xB4;

            // -----------------------------------------------------
            //              10 [ms] 
            // -----------------------------------------------------
            // Conteo = 10ms   / 666.67ns = 15000
            // Carga  = 65536  - 15000    = 50536 = 0xC568
            //TMR0H = 0xC5; TMR0L = 0x68;

            //======================================================
            //              25[ms]  
            //======================================================
            // Conteo = 25ms   / 666.67ns = 37500
            // Carga  = 65536  - 37500    = 28036 = 0x6D84
            // TMR0H = 0x6D; TMR0L = 0x84;
            //======================================================
            //              50 [ms]  
            //======================================================
            // Conteo = 50ms   / 2666.67ns = 18750
            // Carga  = 65536  - 18750     = 46786 = 0xB682
            TMR0H = 0xB6; TMR0L = 0x82;
        }

    // =========================================================
    // INICIALIZACION: RD0 -> 10 pulsos de 50ms, luego ON
    // =========================================================
    for(int i=0; i<10; i++){
        LATDbits.LATD0 = !LATDbits.LATD0;  // toggle
        DelayXms(50);
    }
    LATDbits.LATD0 = 1; // ON Led

    // =========================================================
    // CARGA LA PALABRA HoLA EN EL ARREGLO DATOS
    // codigosLetras[7]=H  [13]=o  [11]=L  [0]=A
    // datos[0]=D1=H  datos[1]=D2=o  datos[2]=D3=L  datos[3]=D4=A
    // =========================================================
    datos[0] = codigosLetras[7];  // H
    datos[1] = codigosLetras[13]; // o
    datos[2] = codigosLetras[11]; // L
    datos[3] = codigosLetras[0];  // A
    
    // =========================================================
    // SECUENCIA 0-9 
    // =========================================================
    // T0CONbits.TMR0ON = 1;   // Encender TMR0 primero
    // MostrarSecuencia();

    // =========================================================
    // Configuracion de TMR0 - MODO 16 BITS
    //
    // T0CON:
    //   bit7: TMR0ON = 0  (apagado por ahora)
    //   bit6: T08BIT = 0  (modo 16-bit) <- 0 en 16bit, 1 en 8bit
    //   bit5: T0CS   = 0  (reloj interno Fosc/4)
    //   bit4: T0SE   = 0  (no importa en interno)
    //   bit3: PSA    = 0  (prescaler activo)
    //   bit2-0: T0PS     prescaler:
    //     000=1:2   001=1:4   010=1:8   011=1:16
    //     100=1:32  101=1:64  110=1:128 111=1:256
    // =========================================================
    // ----------------------------------------------------------
    //   Casos 1, 2, 3, 4 -> Prescaler 1:8   T0PS=010
    // ----------------------------------------------------------
    // T0CON = 0b00000010;

    // ----------------------------------------------------------
    //   caso 5          -> Prescaler 1:32  T0PS=100
    // ----------------------------------------------------------
    T0CON = 0b00000100;

    // Configuracion de interrupciones
    RCONbits.IPEN      = 1;  // Prioridades habilitadas
    INTCON2bits.TMR0IP = 1;  // TMR0 alta prioridad
    INTCONbits.TMR0IE  = 1;  // Interrupcion TMR0 ON
    INTCONbits.TMR0IF  = 0;  // Limpiar bandera
    INTCONbits.GIEH    = 1;  // Global alta ON
    INTCONbits.GIEL    = 1;  // Global baja ON

    T0CONbits.TMR0ON   = 1;  // Encender TMR0
    while(1){
        // MODO S2: TEST DE VELOCIDAD LENTO
        if(PORTCbits.RC4 == 0){
            DelayXms(20);            
            if(PORTCbits.RC4 == 0){
                estado = 1;          // La interrupción empezará a usar el Test lento
                while(PORTCbits.RC4 == 0); 
            }
        }
        
        // MODO S3: FIJO Y PERFECTO
        if(PORTCbits.RC5 == 0){
            DelayXms(20);            
            if(PORTCbits.RC5 == 0){
                estado = 0;          // La interrupción regresa a 2.5ms
                while(PORTCbits.RC5 == 0); 
            }
        }
    }
}