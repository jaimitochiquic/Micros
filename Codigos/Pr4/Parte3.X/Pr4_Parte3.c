/*
 * File:   Pr4_Parte3.c
 * Author: JAIME CHIQUI
 * Created on 4 de junio de 2026, 20:29
 */

//  PRACTICA 3 - PARTE 3: Operacion Microondas
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
//                  VARIABLES GLOBALES
// ============================================================
volatile char datos[4] = {0x00, 0x00, 0x00, 0x00}; //bits ON los segmentos
volatile char multiplex = 0;
volatile char digitos[4] = {0, 0, 0, 0};   // Numero a ON display

// Flags, HH:MM y contadores de ISR
volatile unsigned int cntISR = 0; // contador de interrupciones TMR0
unsigned char Hora = 0;
unsigned char Minutos = 0;
char reloj_activo = 1; // 0 = ajuste (parpadea), 1 = fija (reloj)
char modo_display = 0; // 1 =  rEL
unsigned char cuenta_Minutos = 0;
    
// Operación Microondas
unsigned char Seg_Micro = 0; // Seg regresivo
unsigned char Min_Micro = 0; // Min regresivo
char op_Micro = 0; // 0 = Reposo/Ajuste, 1 = ON
char ing_micro = 0; // 1 ingresando tiempo
char reloj_no_configurado = 1;

// Control del Bloqueo para Niños y Puerta (Interlock)
unsigned char bloqueo_ninos = 0;   // 0 = desbloqueado, 1 = bloqueado
unsigned char puerta_abierta = 0;  // 0 = close, 1 = open puerta: #
unsigned int  timer_stop_3s = 0;   // Cont. 3 seg
unsigned char conteo_paradas = 0;  // Cont. pulsaciones tecla *
 // 1 *: Pausa, 2*: Clear time, 3seg *: Bloquear y 3seg*: desbloquear

// Codigo de los numeros 0-9 (cátodo común)
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
    0b10001110 // F
};
//Mostrar rEL
const char rEL[4] = {
    0b00001010, // D1 'r' 
    0b10011110, // D2 'E' 
    0b00011100, // D3 'L'
    0b00000000 // D4 OFF
};
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

    T0CONbits.TMR0ON = 1; // ON/OFF Timer0
}
// ============================================================
//              SETEAR VALORES (0)
// ============================================================
// Pause Microondas
void DetenerMicroondas(void){
    op_Micro = 0;
    ing_micro = 0;
    LATDbits.LATD0 = 0; // Apaga de inmediato el magnetrón
}
// ==== Clear Microondas
void LimpiarMicroondas(void){
    DetenerMicroondas();
    
    Min_Micro = 0;
    Seg_Micro = 0;
    for(char i = 0; i < 4; i++) digitos[i] = 0;
}
void ReiniciarReloj(void) {
    Hora = 0;
    Minutos = 0;
    cntISR = 0;
    cuenta_Minutos = 0;
    reloj_activo = 0;
    reloj_no_configurado = 1;
}
// ajustar Hora desde donde esta conf.
void AjustarReloj(void){
    reloj_activo = 0;  
    cntISR = 0;
    cuenta_Minutos = 0;
}

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
void ApagarDisplays(void)
{
    for(char i=0;i<4;i++) datos[i]=0;
}
// ============================================================
//          ACTUALIZAR DISPLAYS + TECLA INGRESADA
// ============================================================
void AsignarTiempoDisplay(unsigned char valor_izq, unsigned char valor_der) {
    // Valor 2310: izq=23 derecha=10
    // ejemplo: 37 / 10 = 3.7 (int en c: 3)
    //          37 % 10 = 7 
    datos[0] = codigosNum[valor_izq / 10]; // Decena izq (HH o Min Micro)
    datos[1] = codigosNum[valor_izq % 10]; // Unidad izq (HH o Min Micro)
    datos[2] = codigosNum[valor_der / 10]; // Decena derecha (Min o Seg Micro)
    datos[3] = codigosNum[valor_der % 10]; // Unidad derecha (Min o Seg Micro)
}

void ActualizarDisplays(void) {
    if (bloqueo_ninos) { // Si está bloqueado, dibuja "----"
        datos[0] = 0b10011100; // D1: Forma '['
        datos[1] = 0b10010000; // D2: segmentos a, d        
        datos[2] = 0b10010000; // D3: segmentos a, d
        datos[3] = 0b11110000; // D4: Forma ']'
    } else if (modo_display) { //Mostrar palabra rEL
        datos[0] = rEL[0]; // D1: r
        datos[1] = rEL[1]; // D2: E
        datos[2] = rEL[2]; // D3: L
        datos[3] = rEL[3]; // D4: OFF
    } else if (op_Micro || ing_micro || (Min_Micro > 0 || Seg_Micro > 0)) {
        // Envía los Min y Seg del microondas a los displays
        AsignarTiempoDisplay(Min_Micro, Seg_Micro);
    } else {
        // Envía la Hora y los Min del reloj a los displays
        AsignarTiempoDisplay(Hora, Minutos);
    }
     // Aviso de puerta abierta
    if (puerta_abierta) {
        datos[0] = 0b10011100; // [
    }
}
// ============================================================
//      DESPLAZAR VALORES A LA IZQUIERDA + TECLA INGRESADA
// ============================================================
void Desp_izquierda(char tecla_ingresada) {
    digitos[0] = digitos[1]; // D1
    digitos[1] = digitos[2]; // D2
    digitos[2] = digitos[3]; // D3
    digitos[3] = tecla_ingresada; //D4
}

void IncrementarTiempo(void) {
    Minutos++;
    if (Minutos > 59) {
        Minutos = 0;
        Hora++;
    }
    if (Hora > 23) Hora = 0; // Horas
    ActualizarDisplays();
}

void DecrementarTiempo(void) {
    if (Minutos == 0) {
        Minutos = 59;
        if (Hora == 0) Hora = 23; // Horas
        else Hora--;
    } else {
        Minutos--;
    }
    ActualizarDisplays();
}
// ============================================================
//          BUZZER 
// ============================================================
void Buzzer(void) { // Buzzer pin 2 pic
    LATAbits.LATA0 = 1; // ON buzzer
    DelayXms(100); // Delay 100ms
    LATAbits.LATA0 = 0; // OFF buzzer
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
//          ISR ALTA PRIORIDAD - TMR0 16 bits
// ============================================================
void __interrupt(high_priority) InterrupcionHP(void) {
    if (INTCONbits.TMR0IF) {
        // ==== INTERRUPCIONES DE 2.5 ms ====
        // Tiempo total del ciclo = 4 displays *  2.5 ms = 10 ms
        // ==========================================================
        // TMR0: 2.5ms (Fosc=48MHz, 16-bit, Prescaler 1:8)
        // =========================================================
        // Tcy = 1 / (48MHz / 4) = 83.33ns
        // Tick = Tcy * 8 (Prescaler) = 666.67ns
        // Conteos = 2.5ms / 666.67ns = 3750
        // Carga = 65536 - 3750 = $61786 = 0xF15A
        TMR0H = 0xF1;
        TMR0L = 0x5A;

        // OFF DISPLAY(evita ghosting)
        LATCbits.LATC7 = 0; // D1
        LATCbits.LATC6 = 0; // D2
        LATCbits.LATC1 = 0; // D3
        LATCbits.LATC0 = 0; // D4

        // Multiplexación
        LATD = (datos[multiplex] & 0xFE) | (LATDbits.LATD0);
        switch (multiplex) {
            case 0: LATCbits.LATC7 = 1;
                break;
            case 1: LATCbits.LATC6 = 1;
                break;
            case 2: LATCbits.LATC1 = 1;
                break;
            case 3: LATCbits.LATC0 = 1;
                break;
        }
        multiplex++;
        if (multiplex > 3) multiplex = 0;
        cntISR++; // +1 c/ 2.5ms

        INTCONbits.TMR0IF = 0;
    }
}

// ============================================================
//                      MAIN
// ============================================================
void main(void) {
    unsigned char MedioSecCount = 0; // 0 o 1, c/ 500ms: 0.5seg
    unsigned char btn_A_cnt = 0; // Botones col 4 Teclado
    unsigned char btn_B_cnt = 0;
    unsigned char btn_C_cnt = 0;
    unsigned char btn_D_cnt = 0;

    // --- CONFIGURACIÓN CRÍTICA PARA USAR RC4 y RC5 ---
    UCONbits.USBEN = 0; // Deshabilita el módulo USB
    UCFGbits.UTRDIS = 1; // Deshabilita el transceptor USB (E/S digital)
    ADCON1 = 0x0F; // Buzzer en RA0
    // -------------------------------------------------

    // --- Configuración de Puertos ---
    TRISD = 0;
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 0;
    // TRISC |= 0b00110000;  // Activar s2 y s3

    // ============= Port B - Teclado 4*4 Matricial ==========
    TRISB = 0b00001111; // 0xFF: Nada presionado
    LATB = 0b11110000; // filas en 1
    // INTCON2bits.RBPU = 0; // Resist. Pull-Up internas PIC(NO fisicas) °°°°

    LATD = 0;
    LATCbits.LATC7 = 0;
    LATCbits.LATC6 = 0;
    LATCbits.LATC1 = 0;
    LATCbits.LATC0 = 0;

    // Puerto A para Buzzer
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 0;

    // ==== INTERRUPCIONES DE 2.5 ms ====
    // Carga = 65536 - 3750 = 61786 = 0xF15A
    TMR0H = 0xF1;
    TMR0L = 0x5A; //calculos InterrupcionHp
    T0CON = 0b00000010;

    // Configuración del T0CON:
    // bit 7: TMR0ON = 0 (Apagado por ahora)
    // bit 6: T08BIT = 0 (Configurado en 16 bits)
    // bit 5: T0CS   = 0 (Reloj interno Fosc/4)
    // bit 4: T0SE   = 0 (No reloj interno)
    // bit 3: PSA    = 0 (Prescaler activado)
    // bit 2-0: T0PS = 010 (Prescaler 1:8)

    // ==========================================================
    ConfiguracionInterrupciones_TMR0();

    // INICIO -  LED RD0 **********************
    for (int i = 0; i < 10; i++) {
        LATDbits.LATD0 = !LATDbits.LATD0;
        // delay bloqueante: L = entero largo (desborde)
        for (long d = 0; d < 43000L; d++); // ~50ms a 48MHz
    }
    LATDbits.LATD0 = 1;

    ActualizarDisplays(); //0000
    char tecla;

    while (1) {
        // ---- Tick de 500ms : (500/2.5 = 200 int.) ----
        if (cntISR >= 200) {
            cntISR -= 200; // -200 count ISR para los seg
            MedioSecCount = !MedioSecCount;

            if (!op_Micro) {
                LATDbits.LATD0 = 0; // LED 1Hz en reposo
            }

            if (!reloj_activo) {
                if (modo_display) {
                    ActualizarDisplays(); // Muestra rEL
                } else {
                    // PARPADEO: 
                    if (MedioSecCount == 0) {
                        ApagarDisplays();
                    } else {
                        ActualizarDisplays(); // HH:MM
                    }
                }
            } else {
                //microondas NO funcionando, Reloj HHMM
                if (MedioSecCount == 0) cuenta_Minutos++; // 60 SEG = +1 MIN
                if (cuenta_Minutos >= 60) {
                    cuenta_Minutos = 0;
                    Minutos++;
                    if (Minutos > 59) {
                        Minutos = 0;
                        Hora++;
                    }
                    if (Hora > 23) Hora = 0; // 24 horas (0 a 23)
                }
                // Decrementar contador del microondas cada 1 seg
                if (op_Micro && MedioSecCount == 0) {
                    if (puerta_abierta) {
                        // SEGURIDAD
                        DetenerMicroondas();
                    }
                    else if (Min_Micro == 0 && Seg_Micro == 0) { 
                        // CASO 1: Llegó a 00:00 -> Terminar operación
                        LimpiarMicroondas();
                        reloj_activo = 1; 
                        
                        for(unsigned char b = 0; b < 3; b++) {
                            Buzzer();       //°°°°°°°°°°°°°°°°°°
                            DelayXms(1000);
                        }
                    } 
                    else if (Seg_Micro == 0) {
                        // CASO 2: Quedan min el seg esta 0 -> -1 min
                        Min_Micro--;
                        Seg_Micro = 59;
                    } 
                    else {
                        Seg_Micro--; // CASO 3: -1 seg
                    }
                }
                // ---- Control Pantalla ----
                if (reloj_no_configurado && !op_Micro && !ing_micro && MedioSecCount == 0) {
                    ApagarDisplays();
                } else {
                    ActualizarDisplays(); 
                }
            }
        }

        tecla = LeerTeclado();
        // Bloquear todas las teclas mod niños
        if (bloqueo_ninos && tecla != 14) tecla = 0xFF;
        
        // ---- Tecla A: rEL * 2 seg + 0000
        if (tecla == 10 && !op_Micro) {
            btn_A_cnt++; // Antirebote
            if (btn_A_cnt == 1) {
                Buzzer();
                modo_display = 1; // rEL
                ActualizarDisplays();
                DelayXms(2000); // rEL * 2 seg
                modo_display = 0;
                LimpiarMicroondas();
                if(reloj_no_configurado){
                    ReiniciarReloj();   // 1era conf. -> 00:00
                }
                else{
                    AjustarReloj();     // conserva HHMM
                }

                ActualizarDisplays();
            }
        } else {
            btn_A_cnt = 0; // Setear Antirebote
        }

        // ---- Teclas 0-9: En estado reposo ----
        if (tecla >= 0 && tecla <= 9 && reloj_activo && !op_Micro) {
            // Desplazamiento num.
            Desp_izquierda(tecla);
            ing_micro = 1;

            // Se pasa a minutos y segundos 
            Min_Micro = (digitos[0] * 10) + digitos[1];
            Seg_Micro = (digitos[2] * 10) + digitos[3];

            Buzzer();
            ActualizarDisplays();
            DelayXms(250);
        }

        // ---- Tecla B: Incrementar Tiempo ----
        if (tecla == 11 && !reloj_activo) {
            btn_B_cnt++; // Antirebote 
            if (btn_B_cnt == 1) {
                Buzzer();
                IncrementarTiempo();
            }
            if (btn_B_cnt > 20 && (btn_B_cnt % 3) == 0) {
                IncrementarTiempo();
            }
        } else {
            btn_B_cnt = 0; // set Antirebote
        }

        /* ------------------------------------------------------------------
         * t_espera: tiempo a tener presionado, velo: rapidez del valor a inc/dec
         * t_esp=20 y velo=3 Para inc./dec. al pulsar 
         * 1 while = (3 filas teclado * 5ms) + 50ms delay + 5ms = 70ms.
         * * 20 vueltas * 70ms = 1400ms = 1.4 seg. // modo presionado auto.
         * * % 3  : (3 vueltas * 70ms = 210ms).
         * velocidad cambia el tiempo cada 210ms.
         * ------------------------------------------------------------------ */

        // ---- Tecla C: Decrementar Tiempo ----
        if (tecla == 12 && !reloj_activo) {
            btn_C_cnt++; // Antirebote
            if (btn_C_cnt == 1) {
                Buzzer();
                DecrementarTiempo();
            }
            if (btn_C_cnt > 20 && (btn_C_cnt % 3) == 0) {
                DecrementarTiempo();
            }
        } else {
            btn_C_cnt = 0; // set Antirebote
        }

        // ---- Tecla D: Fijar / +30 seg ----
        if (tecla == 13) {
            btn_D_cnt++;
            if (btn_D_cnt == 1) {
                Buzzer();
                // Caso A: modo ajuste, fija la hora
                if (!reloj_activo && !op_Micro) {
                    reloj_activo = 1;
                    cuenta_Minutos = 0;
                    cntISR = 0;
                    reloj_no_configurado = 0;
                }// Caso B: Reposo (Reloj) + D, inicia con 30s
                else if (reloj_activo && !op_Micro) {
                    op_Micro = 1;
                    ing_micro = 0;
                    if (Min_Micro == 0 && Seg_Micro == 0) {
                        Seg_Micro = 30; //0030
                    }
                    LATDbits.LATD0 = 1; // LED RD0 ON (motor y magnetrón)
                }// Caso C: microondas FUNCIONANDO, +30S
                else if (op_Micro) {
                    Seg_Micro += 30;
                    if (Seg_Micro >= 60) {
                        Seg_Micro -= 60;
                        Min_Micro++;
                    }
                }
                ActualizarDisplays();
            }
        } else {
            btn_D_cnt = 0;
        }
                     
        // ---- Tecla *: STOP / CLEAR / BLOQUEO DE NIÑOS ----
        if (tecla == 14) {
            timer_stop_3s++; // Incrementa presionada
            
            // Cada ciclo con delays del teclado 70ms. 
            // 3000ms / 70ms = 43 ciclos para 3 seg.
            if (timer_stop_3s >= 43) {
                timer_stop_3s = 0; // Resetea contador
                
                if (!op_Micro && !ing_micro) { 
                   // Si se desactiva el bloqueo, limpiamos los datos viejos
                    bloqueo_ninos = !bloqueo_ninos;
                    if(!bloqueo_ninos) {
                        LimpiarMicroondas();
                    }
                    
                    ActualizarDisplays(); 
                    LATAbits.LATA0 = 1;
                    DelayXms(300);
                    LATAbits.LATA0 = 0;
                    
                    conteo_paradas = 0;
                    DelayXms(5); // Evita gosthing
                }
            }
            
            // Pausa / Borrar al soltar o presionar por primera vez
            if (timer_stop_3s == 1 && !bloqueo_ninos) {
                Buzzer();
                if (op_Micro) { // 1 Pulsación cocinando: PAUSA
                    DetenerMicroondas();
                    conteo_paradas = 1;
                } else {
                    // Pulsación en reposo o 2da pulsación: CLEAR
                    LimpiarMicroondas();
                    conteo_paradas = 0;
                }
                ActualizarDisplays();
            }
        } else { // Si se suelta la tecla *
            timer_stop_3s = 0;
        }
        
        // ---- Tecla #: PUERTA ABIERTA/CERRADA  ----
        if (tecla == 15 && !bloqueo_ninos) {
            Buzzer();
            puerta_abierta = !puerta_abierta; // Invierte el estado de la puerta
            
            if (puerta_abierta) {
                // Si la puerta se abre, fuerza el apagado
                DetenerMicroondas();
                Buzzer(); DelayXms(100); Buzzer();
            }
            
            ActualizarDisplays();
            DelayXms(300); // Delay de antirrebote
        }
            
        DelayXms(5);
    }
    return;
}