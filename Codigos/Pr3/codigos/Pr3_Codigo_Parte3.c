/*
 * File:   Pr3_Codigo_Parte3.c
 * Author: JAIME CHIQUI
 *
 * Created on 31 de mayo de 2026, 16:05
 */

//  PRACTICA 3 - PARTE 3
// -- Funcionalidad propia: Scroll de texto "JAIME CHIQUI"
//    con control por pulsantes S2 y S3
// PIC18F4550 Configuration Bit Settings

// CONFIG1L
#pragma config PLLDIV = 5
#pragma config CPUDIV = OSC1_PLL2
#pragma config USBDIV = 1

// CONFIG1H
#pragma config FOSC = HSPLL_HS
#pragma config FCMEN = OFF
#pragma config IESO = OFF

// CONFIG2L
#pragma config PWRT = ON
#pragma config BOR = ON
#pragma config BORV = 3
#pragma config VREGEN = OFF

// CONFIG2H
#pragma config WDT = OFF
#pragma config WDTPS = 32768

// CONFIG3H
#pragma config CCP2MX = ON
#pragma config PBADEN = OFF
#pragma config LPT1OSC = OFF
#pragma config MCLRE = ON

// CONFIG4L
#pragma config STVREN = ON
#pragma config LVP = OFF
#pragma config ICPRT = OFF
#pragma config XINST = OFF
// ** Apagar LVP para no tener corriente parasita ******************

// CONFIG5L
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF

// CONFIG5H
#pragma config CPB = OFF
#pragma config CPD = OFF

// CONFIG6L
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF

// CONFIG6H
#pragma config WRTC = OFF
#pragma config WRTB = OFF
#pragma config WRTD = OFF

// CONFIG7L
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF

// CONFIG7H
#pragma config EBTRB = OFF

#include <xc.h>

// ============================================================
//                  VARIABLES GLOBALES
// ============================================================
volatile char datos[4]          = {0x00, 0x00, 0x00, 0x00}; // Datos a mostrar en displays
volatile char multiplex         = 0;    // Digito activo 0-3
volatile unsigned char visDisplay = 1;  // 1=mostrar  0=apagar displays

// Contador de interrupciones para medir tiempo en el main
volatile unsigned int cntISR    = 0;

// Estado del scroll
// 0 = HoLA estatico (estado inicial)
// 1 = Scroll corriendo
// 2 = Scroll en pausa (congela el cuadro actual)
volatile char estadoScroll = 0;

// ============================================================
//         TABLA DE CODIGOS - CATODO COMUN
//         Orden de bits: [a b c d e f g dp]
// ============================================================

// Numeros 0-9
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

// Letras usadas en el proyecto
// Orden: A  b  C  d  E  F  G  H  h  i  J  L  n  o  P  q  r  S  t  U  u  y
const char codigosLetras[] = {
    0b11101110, // A   ind 0
    0b00111110, // b   ind 1
    0b10011100, // C   ind 2
    0b01111010, // d   ind 3
    0b10011110, // E   ind 4
    0b10001110, // F   ind 5
    0b10111110, // G   ind 6
    0b01101110, // H   ind 7
    0b00101110, // h   ind 8
    0b01100000, // i   ind 9
    0b01111000, // J   ind 10
    0b00011100, // L   ind 11
    0b00101010, // n   ind 12
    0b00111010, // o   ind 13
    0b11001110, // P   ind 14
    0b11110110, // q   ind 15
    0b00001010, // r   ind 16
    0b10110110, // S   ind 17
    0b00011110, // t   ind 18
    0b01111100, // U   ind 19
    0b00111000, // u   ind 20
    0b01110110  // y   ind 21
};

// ============================================================
//    MENSAJE DE SCROLL: "JAIME CHIQUI"//
//    J  A  i  M  E  _  C  H  I  Q  U  I
//    Los espacios (0x00) son displays apagados
// ============================================================
#define TAM_MSG 15

const char mensaje[TAM_MSG] = {
    0b01111000,         // J   (codigosLetras[10])
    0b11101110,         // A   (codigosLetras[0])
    0b01100000,         // i   (codigosLetras[9])
    0b00101010,         // n   <- usada para M 
    0b00101010,         // n   <- usada para M 
    0b10011110,         // E   (codigosLetras[4])
    0x00,               // _   espacio
    0b10011100,         // C   (codigosLetras[2])
    0b01101110,         // H   (codigosLetras[7])
    0b01100000,         // I   (1)
    0b11100110,         // Q   (codigosNum[9])
    0b01111100,         // U   (codigosLetras[19])
    0b01100000,         // I   (1)
    0x00,               // espacio final
    0x00,               // espacio final
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
// ISR ALTA PRIORIDAD - TMR0 16 Bits
// Interrupcion cada 2.5ms -> multiplexacion de los 4 displays
// Ciclo completo = 4 * 2.5ms = 10ms (100Hz, sin efecto de parpadeo)
// ============================================================
void __interrupt(high_priority) InterrupcionHP(void){
    if(INTCONbits.TMR0IF){

        // =======================================================
        // Recarga del TMR0 para 2.5ms
        // Fosc=48MHz  Tcy=83.33ns  Prescaler=1:8  Tick=666.67ns
        // Conteos = 2.5ms / 666.67ns = 3750
        // Carga   = 65536 - 3750 = 61786 = 0xF15A
        // =======================================================
        TMR0H = 0xF1;
        TMR0L = 0x5A;

        // Apagar todos los digitos antes de cambiar (evita ghosting)
        LATCbits.LATC7 = 0; // D1
        LATCbits.LATC6 = 0; // D2
        LATCbits.LATC1 = 0; // D3
        LATCbits.LATC0 = 0; // D4

        // Multiplexacion: mostrar u ocultar segun visDisplay
        if(visDisplay){
            // 0xFE = 11111110 -> limpia bit0 (dp) conservando RD0 del LED
            LATD = (datos[multiplex] & 0xFE) | (LATDbits.LATD0);

            switch(multiplex){
                case 0: LATCbits.LATC7 = 1; break; // D1
                case 1: LATCbits.LATC6 = 1; break; // D2
                case 2: LATCbits.LATC1 = 1; break; // D3
                case 3: LATCbits.LATC0 = 1; break; // D4
            }
        } else {
            // Displays apagados: solo preservar el estado del LED en RD0
            LATD = 0x00 | (LATDbits.LATD0);
        }

        // Avanzar mux siempre 
        // Evita desincronizacion al volver a mostrar
        multiplex++;
        if(multiplex > 3) multiplex = 0;

        // Contador de ticks para medir tiempos en el main
        // Cada tick = 2.5ms -> 1000ms / 2.5ms = 400 ticks por segundo
        cntISR++;

        INTCONbits.TMR0IF = 0; // Limpiar bandera de interrupcion
        return;
    }
}

// ============================================================
//    CARGAR VENTANA DEL SCROLL EN EL ARREGLO datos[]
//    pos: indice del primer caracter visible en D1
// ============================================================
void CargarVentana(int pos){
    int i;
    for(i = 0; i < 4; i++){
        // Si el indice cae fuera del mensaje, mostrar espacio
        if((pos + i) < TAM_MSG){
            datos[i] = mensaje[pos + i];
        } else {
            datos[i] = 0x00; // espacio
        }
    }
}

// ============================================================
//                      MAIN
// ============================================================
void main(void){
    int posScroll   = 0;    // Posicion actual de la ventana en el mensaje
    int tickScroll  = 0;    // Acumulador de ticks para el avance del scroll
    // Velocidad del scroll: ticks entre cada avance de 1 posicion
    // 300ms / 2.5ms por tick = 120 ticks
    int velocidad   = 120;

    // --- CONFIGURACION CRITICA PARA USAR RC4 y RC5 como GPIO ---
    UCONbits.USBEN  = 0;    // Deshabilita el modulo USB
    UCFGbits.UTRDIS = 1;    // Convierte RC4/RC5 a E/S digital
    // -----------------------------------------------------------

    // Configuracion de entrada / salida
    TRISD = 0;                  // Puerto D todo salida (segmentos + LED)
    TRISCbits.TRISC0 = 0;       // RC0 = D4 salida
    TRISCbits.TRISC1 = 0;       // RC1 = D3 salida
    TRISCbits.TRISC6 = 0;       // RC6 = D2 salida
    TRISCbits.TRISC7 = 0;       // RC7 = D1 salida
    TRISC |= 0b00110000;        // RC4=S2 entrada, RC5=S3 entrada

    // Apagar displays al inicio (evita ghosting)
    LATD = 0;
    LATCbits.LATC7 = 0;
    LATCbits.LATC6 = 0;
    LATCbits.LATC1 = 0;
    LATCbits.LATC0 = 0;

    // =======================================================
    // Carga inicial del TMR0 para 2.5ms
    // =======================================================
    TMR0H = 0xF1;
    TMR0L = 0x5A;

    // =======================================================
    // Configuracion de TMR0 - MODO 16 BITS, Prescaler 1:8
    //
    // T0CON:
    //   bit7: TMR0ON = 0  (apagado por ahora, se enciende al final)
    //   bit6: T08BIT = 0  (modo 16 bits)
    //   bit5: T0CS   = 0  (reloj interno Fosc/4)
    //   bit4: T0SE   = 0  (no aplica con reloj interno)
    //   bit3: PSA    = 0  (prescaler activo)
    //   bit2-0: T0PS = 010 (prescaler 1:8)
    // =======================================================
    T0CON = 0b00000010;

    // Configuracion de interrupciones con prioridades
    RCONbits.IPEN      = 1;  // Habilitar sistema de prioridades
    INTCON2bits.TMR0IP = 1;  // TMR0 = alta prioridad
    INTCONbits.TMR0IE  = 1;  // Habilitar interrupcion de TMR0
    INTCONbits.TMR0IF  = 0;  // Limpiar bandera de TMR0
    INTCONbits.GIEH    = 1;  // Habilitar interrupciones globales alta prioridad
    INTCONbits.GIEL    = 1;  // Habilitar interrupciones globales baja prioridad

    T0CONbits.TMR0ON   = 1;  // Encender TMR0

    // =========================================================
    // INICIALIZACION: RD0 -> 10 pulsos de 50ms, luego ON
    // Sirve como verificacion visual de que el PIC arranco bien
    // =========================================================
    for(int i = 0; i < 10; i++){
        LATDbits.LATD0 = !LATDbits.LATD0;  // Toggle LED
        DelayXms(50);
    }
    LATDbits.LATD0 = 1; // LED encendido fijo

    // =========================================================
    // ESTADO INICIAL: mostrar "HoLA" fijo hasta que se pulse S2
    // codigosLetras[7]=H  [13]=o  [11]=L  [0]=A
    // =========================================================
    datos[0] = codigosLetras[7];  // H
    datos[1] = codigosLetras[13]; // o
    datos[2] = codigosLetras[11]; // L
    datos[3] = codigosLetras[0];  // A
    visDisplay  = 1;
    estadoScroll = 0; // Estado inicial = HoLA fijo

    // =========================================================
    // BUCLE PRINCIPAL
    // =========================================================
    while(1){

        // =======================================================
        // LECTURA DE PULSANTES
        // S2 (RC4): Inicia o reanuda el scroll
        // S3 (RC5): Pausa / congela el scroll en el cuadro actual
        // =======================================================

        // --- S2: INICIAR o REANUDAR scroll ---
        if(PORTCbits.RC4 == 0){
            DelayXms(20);               // Antirebote
            if(PORTCbits.RC4 == 0){
                if(estadoScroll == 0){
                    // Primer arranque: reiniciar posicion al inicio
                    posScroll  = 0;
                    tickScroll = 0;
                    CargarVentana(posScroll);
                }
                estadoScroll = 1;       // Scroll corriendo
                visDisplay   = 1;
                while(PORTCbits.RC4 == 0); // Esperar a soltar S2
            }
        }

        // --- S3: PAUSAR / CONGELAR el cuadro actual ---
        if(PORTCbits.RC5 == 0){
            DelayXms(20);               // Antirebote
            if(PORTCbits.RC5 == 0){
                if(estadoScroll == 1){
                    estadoScroll = 2;   // Pausa: el cuadro queda congelado
                } else if(estadoScroll == 2){
                    estadoScroll = 1;   // Reanudar desde donde estaba
                } else if(estadoScroll == 0){
                    // Desde HoLA: S3 no hace nada (requiere S2 primero)
                }
                while(PORTCbits.RC5 == 0); // Esperar a soltar S3
            }
        }
        // =======================================================
        // MAQUINA DE ESTADOS DEL SCROLL
        // =======================================================
        if(estadoScroll == 0){
            // ---------------------------------------------------
            // ESTADO 0: HoLA fijo (estado inicial)
            // Solo se sale de este estado con S2
            // ---------------------------------------------------
            visDisplay = 1;
        } else if(estadoScroll == 1){
            // ---------------------------------------------------
            // ESTADO 1: Scroll corriendo
            // Cada 'velocidad' ticks (= 300ms) avanza 1 posicion
            // cntISR se incrementa en la ISR cada 2.5ms
            // ---------------------------------------------------
            if(cntISR >= (unsigned int)velocidad){
                cntISR = 0;         // Resetear contador de ticks

                // Cargar la ventana actual en datos[]
                CargarVentana(posScroll);
                posScroll++;    // Avanzar posicion para la siguiente vez

                // Al llegar al final del mensaje, reiniciar (loop)
                if(posScroll >= TAM_MSG){
                    posScroll = 0;
                }
            }

        } else if(estadoScroll == 2){
            // ---------------------------------------------------
            // ESTADO 2: Scroll en pausa
            // El cuadro actual permanece congelado en los displays
            // Se reanuda con S3
            // ---------------------------------------------------
            cntISR = 0; // Evitar acumulacion
        }
    } 
    return;
}
