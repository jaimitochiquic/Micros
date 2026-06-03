/*
 * File:   Pr3_Codigo_Parte2.c
 * Author: JAIME CHIQUI
 *
 * Created on 28 de mayo de 2026, 10:42
 */
//  PRACTICA 3 - PARTE 2:  RELOJ HH:MM
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
volatile char datos[4]          = {0x00, 0x00, 0x00, 0x00};
volatile char multiplex         = 0;
volatile unsigned char visDisplay = 1;  // 1=mostrar  0=apagar

// Flags y contadores de ISR
volatile unsigned int  cntISR      = 0;   // cuenta interrupciones para 1 s
int Hora  = 0;
int Minutos = 0;
char is_set  = 0;

// Codigo de los numeros 0-9 (cátodo común, orden [a b c d e f g dp])
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
//          ACTUALIZAR DISPLAYS CON MM:SS
// ============================================================
void ActualizarDisplays(void){
    datos[0] = codigosNum[Hora  / 10];  // D1: decenas Hora
    datos[1] = codigosNum[Hora  % 10];  // D2: unidades Hora
    datos[2] = codigosNum[Minutos / 10];  // D3: decenas Minutos
    datos[3] = codigosNum[Minutos % 10];  // D4: unidades Minutos
    
    // ejemplo: 37 / 10 = 3.7 (int en c: 3)
    //          37 % 10 = 7 
}
void IncrementarTiempo(void){
    Minutos++;
    if(Minutos > 59){ Minutos = 0; Hora++; }
    if(Hora  > 23)  Hora  = 0;  // Horas
    ActualizarDisplays();
}

void DecrementarTiempo(void){
    if(Minutos == 0){
        Minutos = 59;
        if(Hora == 0) Hora = 23;  // hras
        else             Hora--;
    } else {
        Minutos--;
    }
    ActualizarDisplays();
}
// ============================================================
//          ISR ALTA PRIORIDAD - TMR0 16 bits
// ============================================================
void __interrupt(high_priority) InterrupcionHP(void){
    if(INTCONbits.TMR0IF){
        // Para 2.5 ms debes cargar:
        TMR0H = 0xF1;  // Parte alta
        TMR0L = 0x5A;  // Parte baja
        // Apagar todos los digitos antes de cambiar (evita ghosting)
        LATCbits.LATC7 = 0; // D1
        LATCbits.LATC6 = 0; // D2
        LATCbits.LATC1 = 0; // D3
        LATCbits.LATC0 = 0; // D4
            
        // Multiplexación
        if(visDisplay){
            LATD = (datos[multiplex] & 0xFE) | (LATDbits.LATD0);
            switch(multiplex){
                case 0: LATCbits.LATC7 = 1; break;
                case 1: LATCbits.LATC6 = 1; break;
                case 2: LATCbits.LATC1 = 1; break;
                case 3: LATCbits.LATC0 = 1; break;
            }
        } else {
            LATD = 0x00 | (LATDbits.LATD0);
        }

        // multiplex incre. siempre
        multiplex++;
        if(multiplex > 3) multiplex = 0;

        // Tick: 200 interrupciones = 500ms / 5ms
        cntISR++;     // 
        
        INTCONbits.TMR0IF = 0;
    }
}

// ============================================================
//                      MAIN
// ============================================================
void main(void){
    unsigned char MedioSecCount = 0;  // 0 o 1, alterna cada 500ms: 0.5seg
    unsigned char cuenta_Minutos = 0; 
    
    int btn_s2_cnt = 0;
    int btn_s3_cnt = 0;
    
    // --- CONFIGURACIÓN CRÍTICA PARA USAR RC4 y RC5 ---
    UCONbits.USBEN = 0;   // Deshabilita el módulo USB
    UCFGbits.UTRDIS = 1;  // Deshabilita el transceptor USB (Convierte a E/S digital)
    // -------------------------------------------------

    TRISD = 0;
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 0;
    TRISC |= 0b00110000;  // Activar s2 y s3

    LATD = 0;
    LATCbits.LATC7 = 0; 
    LATCbits.LATC6 = 0;
    LATCbits.LATC1 = 0;
    LATCbits.LATC0 = 0;
  // == INTERRUPCIONES DE 2.5 ms
  // Tiempo total del ciclo = 4 displays *  2.5 ms = 10 ms
    // ==========================================================
    // TMR0: 5ms (Fosc=48MHz, 16-bit, Prescaler 1:8)
    // ==========================================================
    // Tcy = 1 / (48MHz / 4) = 83.33ns
    // Tick = Tcy * 8 (Prescaler) = 666.67ns
    // Conteos = 2.5ms / 666.67ns = 3750
    // Carga = 65536 - 3750 = $61786 = 0xF15A
    TMR0H = 0xF1;
    TMR0L = 0x5A;
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

    // Secuencia inicial LED RD0
    for(int i = 0; i < 10; i++){
        LATDbits.LATD0 = !LATDbits.LATD0;
        // delay bloqueante: L = entero largo (malinterpretado - desborde)
        for(long d = 0; d < 43000L; d++); // ~50ms a 48MHz
    }
    LATDbits.LATD0 = 1;

    ActualizarDisplays(); // 00:00

    while(1){

        // ---- Tick de 500ms : (500/2.5 = 200 int.) ----
        if(cntISR >= 200){
            cntISR = 0;
            MedioSecCount = !MedioSecCount;
            LATDbits.LATD0 = !LATDbits.LATD0;  // LED 1Hz

            if(!is_set){
                visDisplay = MedioSecCount; 
            } else {
                visDisplay = 1;
                /* ===================================================*/
                 // Cronometro
                if(MedioSecCount == 0){
                    cuenta_Minutos++; // 60 seg, incrementa 1
                    if(cuenta_Minutos >= 60){ 
                        cuenta_Minutos = 0; // Reinicia el contador de segundos
                        Minutos++;
                        if(Minutos > 59){ 
                            Minutos = 0; 
                            Hora++; 
                        }
                        if(Hora > 23) Hora = 0; // 24 horas (0 a 23)
                        ActualizarDisplays();
                    }
                }
                // ==================================================== */
            }
        }

        // ---- Boton S2: incrementar ----
        if(!PORTCbits.RC4){
            btn_s2_cnt++;
            if(btn_s2_cnt == 1){        // Presiona s2 flanco
                is_set     = 1;
                visDisplay = 1;
                IncrementarTiempo();
            }
            // cntISR: cada 2 interrupciones = 10ms : ~500ms (50 ticks x 10ms aprox)
            if(btn_s2_cnt > 50 && (btn_s2_cnt % 10) == 0){
                IncrementarTiempo();
            }
        } else {
            btn_s2_cnt = 0;
        }

        // ---- Boton S3: decrementar ----
        if(!PORTCbits.RC5){
            btn_s3_cnt++;
            if(btn_s3_cnt == 1){
                is_set     = 1;
                visDisplay = 1;
                DecrementarTiempo();
            }
            if(btn_s3_cnt > 50 && (btn_s3_cnt % 10) == 0){
                DecrementarTiempo();
            }
        } else {
            btn_s3_cnt = 0;
        }

        // Pequeña pausa no bloqueante: solo 2 interrupciones = 10ms
        DelayXms(10);
    }
    return;
}