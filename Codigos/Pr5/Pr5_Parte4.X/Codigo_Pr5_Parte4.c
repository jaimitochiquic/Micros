/*File:   Codigo_Pr5_Parte4.c
 * Author: JAIME CHIQUI
 * Created on 19 de junio de 2026, 1:18
 */
//  PRACTICA 5 - PARTE 4: ADC + parte2
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
// Apagar LVP para no tener corriente parasita ******************

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
//              MODOS DE FUNCIONAMIENTO
// ============================================================
#define MODO_ADC_AUTO      0
#define MODO_LED_MANUAL    1
#define MODO_JUEGO         2
#define MODO_MENU          3
// ============================================================
//              VARIABLES GLOBALES
// ============================================================
volatile unsigned int adcValue = 0;   // 0 - 1023
unsigned char temp = 0;
volatile unsigned int ledPeriodo = 25;
volatile unsigned char flagADC = 0; // isr

volatile unsigned char modo = MODO_MENU;
volatile unsigned char holdADC = 0;
volatile unsigned int ledManual = 100; // OPC. B
volatile unsigned int puntaje = 0; // OPC C VAR. JUEGO
unsigned int numeroIngresado = 0;
unsigned char digitosIngresados = 0;
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
//          CONFIGURACION INICIAL DEL PIC
// ===========================================================
void Configuracion_inicialPIC(void){
    TRISD = 0x00;       // PORTD como salida
    TRISAbits.TRISA0 = 1; // AN0 entrada ADC
    TRISAbits.TRISA1 = 0; // A1,..,A4 = 0 SALIDAS
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    LATA = 0x00;        // Limpiar PORTA
    LATD = 0x00;        // Limpiar PORTD
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
    // LCD_Comando(0x04); // Decremento cursor,
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
//             lED PARPADEANDO (Blink)
// ============================================================
void led_blink(int tiempo_ms){
    LATDbits.LATD0 = !LATDbits.LATD0; // toggle led
    DelayXms(tiempo_ms);  // delay 
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
//              CONFIGURACION ADC
// ============================================================
void InitADC(void){

    //0x0E=1110 : AN0 analogico, AN1...AN12 digitales
    ADCON1 = 0x0E;

    // ADFM=1 : Right justified
    // ACQT=001 = 2TAD: A/D adquisition Time select bits
    // ADCS=110 = Fosc/64: A/D Clock select bits
    ADCON2 = 0b10001110; // ***  ADCON2: A/D CONTROL REGISTER 2

    // **** ADCON0: A/D CONTROL REGISTER 0
    CHS3 = 0; // Canal AN0
    CHS2 = 0;
    CHS1 = 0;
    CHS0 = 0;

    ADON = 1; // ADC ON(ENABLE)
}
// ============================================================
//              LEER ADC 10 BITS
// ============================================================
void LeerADC(void){
    Delay1ms();   // tiempo de adquisición (carga del capacitor)
    
    // Convertir el voltaje en AN0 a un número digital
    GO_DONE = 1;  // A/D Conversion Status bit
    while(GO_DONE);  //termina conversion ?GO_DONE = 0
    
    // Conversion obtenemos adc 10 bits
    adcValue = ADRESL; //LEE 8 bits bajos
    temp = ADRESH;  // 6 ceros + 2 bits faltantes 
    
    adcValue = adcValue | (temp << 8); // Mover bits 8 posiciones a la izq.
}
// ============================================================
//          VISUALIZAR VALOR ADC EN LCD
// ============================================================
void MostrarADC(void){

    char miles;
    char centenas;
    char decenas;
    char unidades;

    miles    = adcValue / 1000;
    centenas = (adcValue % 1000)/100;
    decenas  = (adcValue % 100)/10;
    unidades = adcValue % 10;

    LCD_Comando(0x80);

    LCD_Texto("ADC = ");

    LCD_Caracter(miles + '0');
    LCD_Caracter(centenas + '0');
    LCD_Caracter(decenas + '0');
    LCD_Caracter(unidades + '0');
    LCD_Texto("    ");
}
// ============================================================
//          VELOCIDAD LED SEGUN ADC
// ============================================================
void ActualizarVelocidadLED(void){

    if(adcValue < 256)
        ledPeriodo = 25;
    else if(adcValue < 512)
        ledPeriodo = 50;
    else if(adcValue < 768)
        ledPeriodo = 100;
    else
        ledPeriodo = 200;
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
// ============================================================
//              PANTALLA ADC NORMAL
// ============================================================
void PantallaADC(void){
    MostrarADC();
}
// ============================================================
//      VELOCIDAD LED MANUAL SEGUN TECLA
// ============================================================
void AjustarLEDManual(char tecla){
    switch(tecla){
        case 1:
            ledManual = 25; break;
        case 2:
            ledManual = 50; break;
        case 3:
            ledManual = 100; break;
        case 4:
            ledManual = 200; break;
    }
}
// ============================================================
//          ADC COMO BARRA DE PROGRESO
// ============================================================
void MostrarBarraADC(void){
    unsigned char barras;

    barras = (adcValue * 16)/1023;
    LCD_Linea2();
    for(char i=0;i<16;i++){
        if(i < barras)
            LCD_Caracter(255);
        else
            LCD_Caracter(' ');
    }
}
// ============================================================
//              MODO LED MANUAL
// ============================================================
void PantallaLEDManual(void){
    MostrarADC();
    MostrarBarraADC();
}
// ============================================================
//      MOSTRAR NUMERO INGRESADO
// ============================================================
void MostrarNumeroIngresado(void){
    char miles;
    char centenas;
    char decenas;
    char unidades;

    miles = numeroIngresado/1000;
    centenas = (numeroIngresado%1000)/100;
    decenas = (numeroIngresado%100)/10;
    unidades = numeroIngresado%10;

    LCD_Comando(0x80); //cursor al inicio primera línea.
    LCD_Texto("ADIVINE ADC");
    LCD_Linea2();
    LCD_Caracter(miles+'0');
    LCD_Caracter(centenas+'0');
    LCD_Caracter(decenas+'0');
    LCD_Caracter(unidades+'0');
}
// ============================================================
//          MOSTRAR SCORE
// ============================================================
void MostrarScore(char correcto){
    LCD_Comando(0x01);

    if(correcto)
        LCD_Texto("CORRECTO!");
    else
        LCD_Texto("INCORRECTO");

    LCD_Linea2();
    LCD_Texto("PTS:");
    LCD_Caracter((puntaje/10)+'0');
    LCD_Caracter((puntaje%10)+'0');
}
// ============================================================
//                  CONFIGURACIÓN DE INTERRUPCIONES
// ============================================================
void ConfiguracionInterrupciones_TMR0(void) {
    RCONbits.IPEN = 1; // Prioridades de interrupción
    INTCON2bits.TMR0IP = 1; // Timer0 con HP
    INTCONbits.TMR0IE = 1; // Interrupción Timer0
    INTCONbits.TMR0IF = 0; // Limpiar bandera del Timer0
    INTCONbits.GIEH = 1; // Interrupciones globales de HP
    INTCONbits.GIEL = 1; // Interrupciones globales de LP
    
    // TMR0ON=1, T08BIT=0, T0CS=0, PSA=0, T0PS=111 (1:256)
    T0CON = 0b10000111;  
    // Primera carga 200 ms
    TMR0H = 0xDB;
    TMR0L = 0x61;
}
// ============================================================
//          ISR ALTA PRIORIDAD - TMR0 16 bits
// ============================================================
void __interrupt(high_priority) InterrupcionHP(void) {
    if (INTCONbits.TMR0IF) {
        // ==========================================================
        // TMR0: 200ms (Fosc=48MHz, 16-bit, Prescaler 1:256)
        // =========================================================
        // Tcy = 1 / (48MHz / 4) = 83.33ns
        // Tick = Tcy * 256 (Prescaler) = 21.33us
        // Conteos = 200ms / 21.33us = 9375
        // Carga = 65536 - 9375 = 56161 = 0xDB61
        TMR0H = 0xDB;
        TMR0L = 0x61;
        
        if(!holdADC){
            LeerADC(); // ADC en la ISR
        }
        flagADC = 1;
        INTCONbits.TMR0IF = 0;
    }
}
// ============================================================
//              RESET LCD
// ============================================================
void ResetLCD(void){

    LCD_Comando(0x01);
    LCD_Texto("REINICIANDO");
    DelayXms(1000);
    LCD_Comando(0x01);
}
// ============================================================
//              MENU PRINCIPAL
// ============================================================
void MostrarMenuScroll(void){

    LCD_Comando(0x01);
    //LCD_Texto("A:ADC B:LED C:JUEGO *:RST #:HOLD");
    LCD_Comando(0x01);
    LCD_Texto("A:ADC B:LED");
    LCD_Linea2();
    LCD_Texto("C:JUEGO *,#:RST");
}
// =====================================================
// MAIN
// =====================================================
void main(void){
    // ============= Port B - Teclado 4*4 Matricial ==========
    TRISB = 0b00001111; // 0xFF: Nada presionado
    LATB = 0b11110000; // filas en 1
    
    Configuracion_inicialPIC(); // Salidas y clear
    
    // subrutina de inicializacion
    for(char i=0;i<10;i++) led_blink(50);
    LATDbits.LATD0 = 1; // LED ON permanente
    
    ConfiguracionInterrupciones_TMR0();
    InitLCD();  // Configuracion de lcd 16*2, 4 bits
    InitADC();  // Iniciar ADC
    MostrarMenuScroll(); 
    unsigned char tecla;
        
    while(1){
        tecla = LeerTeclado();
        if(modo == MODO_MENU){
            MostrarMenuScroll();
        }
        if(modo == MODO_MENU && tecla != 0xFF){ // MOSTRAR TECLA MENU
            LCD_Comando(0xC0);
            LCD_Caracter('T');
            LCD_Caracter('=');
            LCD_Caracter(tecla + '0');
        }
        LCD_Comando(0x01);
        // MODO SEGUN TECLADO
        if(tecla == 10) modo = MODO_ADC_AUTO; // TECLA A

        if(tecla == 11) modo = MODO_LED_MANUAL; // TECLA B
        if(tecla == 12){                        // TECLA C
            modo = MODO_JUEGO;
            numeroIngresado = 0;
            digitosIngresados = 0;
        }
        if(tecla == 14)if(tecla == 14){
            ResetLCD();
            modo = MODO_MENU;
        }     // *
        if(tecla == 15){ // #
            holdADC = !holdADC;
            LCD_Comando(0x01);
            if(holdADC)
                LCD_Texto("ADC HOLD");
            else
                LCD_Texto("ADC RUN");
            DelayXms(1000);
        }
        if(modo == MODO_LED_MANUAL){
            AjustarLEDManual(tecla);
        }
        if(modo == MODO_JUEGO){ // INGRESO DE NUMEROS DEL JUEGO
            if(tecla != 0xFF && tecla <= 9){
                if(digitosIngresados < 4){
                    numeroIngresado =
                    (numeroIngresado * 10) + tecla;
                    digitosIngresados++;
                }
            }
        }
        if(modo == MODO_JUEGO && tecla == 13){

            int diferencia;
            diferencia = adcValue - numeroIngresado;

            if(diferencia < 0)
                diferencia = -diferencia;

            if(diferencia <= 20){
                puntaje++;
                MostrarScore(1);   // Correcto
            }
            else{
                MostrarScore(0);   // Incorrecto
            }

            DelayXms(2000);

            numeroIngresado = 0;
            digitosIngresados = 0;
        }
        if(flagADC){
            flagADC = 0;
            switch(modo){
                case MODO_ADC_AUTO:
                    MostrarADC();
                    ActualizarVelocidadLED();
                    break;
                case MODO_LED_MANUAL:
                    PantallaLEDManual();
                    ledPeriodo = ledManual;
                    break;
                case MODO_JUEGO:
                    MostrarNumeroIngresado();
                    break;
            }
        }
        led_blink(ledPeriodo);
    } // Usar TMR0 para adc
}