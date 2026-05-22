	; PRACTICA 2 - MICROCONTROLADORES
	; PARTE 2: Timer0 en led Piloto (500 ms)
	; MARZO-AGOSTO 2026
	; Jaime Chiqui
	
	;******************************************* ; Program Header ***********************************************************
		LIST p=18f4550			;Especifica el dispositivo
		#include "p18f4550.inc"	;traducción de variables a memoria
	;*********************************** 
	;***** DECLARACIÓN VARIABLES ******
	Contador1 		equ	0x00
	Contador2		equ 0x01
	ContaDelay100	equ 0x02
	Xveces			equ 0x03
	ContadorInicio  equ 0x04
	Sentido         equ 0x05
	;**********************************
	
	;----- Definición de Vectores --------------
		org		0x00    ; Vector de Inicio / reset
		goto 	Inicio
	;-------------------------------------------
		org 	0x08	; Vect. interr. HP
		goto    InterrupcionHP	
	; ++++ INICIO DEL PROGRAMA +++++++++++++++++
		org 	0x20	; Dónde comienza el programa 
	Inicio:
	
	;##############################################
	; Configuración de periféricos
	; --------------------------------------------
		; CONFIGURACION DE ENTRADA/SALIDA
		;........................................
		BCF		TRISC,0 	; Apaga LED piloto
		CLRF	TRISD		; Puerto D como salida
		BCF     TRISC, 0    ; RC0 salida (LED piloto)
		CLRF    LATD        ; Apaga LEDs
		BCF		UCON,3		;RC4 Y RC5 ENTRADAS
		BSF		UCFG,3	    ;ESPECIAL DE RC4-RC5
		; ---------------------------------------
		; CONFIGURACION DEL TIMER0
		;........................................
			; Configurar bit a bit (no recomendable)
		BCF		T0CON, TMR0ON   ; Apaga TMRO (USA TMR0ON O 7: T0CON, 7)
		BCF		T0CON, T08BIT	; TMR0 A 16 Bits
		BCF		T0CON, T0CS		; TMR0 - MODO TIMER (no pull up)
	
		BCF		T0CON, PSA		; Prescaler SI
		BSF		T0CON, T0PS2	; Prescaler 256
		BSF		T0CON, T0PS1	;
		BSF		T0CON, T0PS0	; 
	
		; ------------------------------------------
		; CONFIGURACION DE INTERRUPCIONES
		;........................................
		BSF		INTCON, GIE		; Enceneder todas las interrupciones
		BSF		INTCON, PEIE	; Prendo Interr. PERIFERICOS
		BSF		INTCON, TMR0IE	; Prendi Interr. T0
		
	;--- INICIALIZACIÓN DE VARIABLES ------
		MOVLW	d'198'			;W = 198
		MOVWF	Contador1		;Contador1 = 10
		MOVLW	d'21'			;W = 21
		MOVWF	Contador2		;Contador2 = 10
		MOVLW   d'20'            
    	MOVWF   ContadorInicio  ;RC0 parpadea 20 veces / 50 ms
		MOVLW 	d'100'
		MOVWF 	ContaDelay100
		
		MOVLW   0xD9 		; 65535 (55769)  ---> CAMBIA HEXADECIMAL
		MOVWF 	TMR0H		; PARTRE ALTA 16 BITS
		MOVLW   0xDA		; AL CAMBIAR CF A D5 ESTE CUENTA MENOS YA SI
		MOVWF	TMR0L 		; PARTRE BAJA 16 BITS

		BSF     T0CON, TMR0ON   ; Enciende el Timer0

;------- SECUENCIA DE INICIO -------
	BucleInicio:
	    BSF     LATC, 0          ; Enciende LED RC0-Led_piloto
	    MOVLW   d'50'
	    CALL    DelayXms
	    BCF     LATC, 0          ; Apaga LED RC0-Led_piloto
	    MOVLW   d'50'
	    CALL    DelayXms
	    DECFSZ  ContadorInicio, 1 ; Decrementa y salta si es cero
	    GOTO    BucleInicio
	    
	    BSF      LATC, 0          ; RC0-Led_piloto ENCENDIDO
;--------------------------------------
	EncuestaON:					; Espera a S2 para iniciar secuencia
		BTFSC 	PORTC, 4
		GOTO 	EncuestaON
		CALL    Delay100ms 
	Est_transistorio:        	; °Esperar a que suelte S2
	    BTFSS   PORTC, 4
	    GOTO    Est_transistorio
	
	;******* SECUENCIA OLA *********** ; °°°° Inicio de la secuencia solicitada
		MOVLW   b'00000011'     ; LED izq ON (rojos) siempre encendido
		MOVWF   LATD

	Ola_Hacia_Izq:
		MOVLW   d'100'           ; °°°° Carga 100ms para el delay
		CALL    DelayXms
		
		BTFSS   PORTC, 5        ; verificar boton s3
		GOTO    Detener_Secuencia
		BCF     Sentido, 0		; bandera indicar sent. izq.

		RLNCF   LATD, 1         ; Rota la luz a la izquierda
		BTFSS   LATD, 7         ; 10000000 (bit 7 en 1)
		GOTO    Ola_Hacia_Izq   ;
		
	Ola_Hacia_Der:
		MOVLW   d'100'           ; °°°° Carga 100ms para el delay
		CALL    DelayXms
		
		BTFSS   PORTC, 5        ; verificar boton s3
		GOTO    Detener_Secuencia
		BSF     Sentido, 0		; bandera indicar sent. DER.

		RRNCF   LATD, 1         ; Rota la luz a la derecha
		BTFSS   LATD, 0         ; 00000001  (bit 0 es 1)
		GOTO    Ola_Hacia_Der   
		
		GOTO    Ola_Hacia_Izq   ; Repite la Ola
; -------------------- DETENER Y NO OFF
	Detener_Secuencia:
		CALL    Delay100ms     	; Anti-rebote
	Boton_S3_D:
		BTFSS   PORTC, 5		; Espera que se suelte S3
		GOTO    Boton_S3_D


; -------------------- CONTINUA S2
	Trans_s2:
		BTFSC 	PORTC, 4
		GOTO 	Trans_s2
		CALL    Delay100ms     	; Anti-rebote
	Continuar_s2:
		BTFSS   PORTC, 4		
		GOTO 	Continuar_s2
		
		BTFSC   Sentido, 0      ; Esta en dir. derecha
		GOTO    Ola_Hacia_Der   ; va a derecha
		GOTO    Ola_Hacia_Izq   ; No, va a izquierda
	
	;++++++ DELAY DE X[ms] +++++++++++++++++++
	;Deberia llamar a Delay1ms, Xveces
	DelayXms: 
		MOVWF	Xveces
	bucleVeces:
		CALL 	Delay1ms
		DECFSZ	Xveces,1
		GOTO	bucleVeces
	
		return
	;+++++++++++++++++++++++++++++++++++++++	
		
	;++++++ DELAY DE 100MS +++++++++++++++++++
	Delay100ms:
		CALL 	Delay1ms
		DECFSZ	ContaDelay100,1
		GOTO	Delay100ms
		;Reinicio ContaDelay100 = 100
		MOVLW	d'100'	;W=100
		MOVWF	ContaDelay100
	
		return
	;+++++++++++++++++++++++++++++++++++++++	
	;++++++ DELAY DE 1MS +++++++++++++++++++
	Delay1ms:	
		DECFSZ	Contador2,1
		GOTO	bucle1	
		GOTO	salir
	bucle1:		
		DECFSZ	Contador1,1
		GOTO 	bucle1
		
		MOVLW	d'198'		;W = 198
		MOVWF	Contador1	;Contador1 = 198
		GOTO 	Delay1ms
	salir:
		MOVLW	d'21'		;W = 21
		MOVWF	Contador2	;Contador2 = 10
		return
	;+++++++++++++++++++++++++++++++++++++++

	; *************INTERRUPCION_HP***********
	InterrupcionHP:
		; Complementa LED RC0 (D9)
    	BTG     LATC, 0           ; Toggle LED piloto
		BCF		INTCON, TMR0IF 	  ;OFF bandera IT TIMER
	
		MOVLW   0xD9 		; 65536-9766 (55769=D9DA)  ---> CAMBIA HEXADECIMAL
		MOVWF 	TMR0H		; PARTRE ALTA 16 BITS
		MOVLW   0xDA		; AL CAMBIAR CF A D5 ESTE CUENTA MENOS Y ASI
		MOVWF	TMR0L 		; PARTRE BAJA 16 BITS
	
		RETFIE              ; Regresa de interrupción
	;+++++++++++++++++++++++++++++++++++++++	
	end
	;+++++++++++++++++++++++++++++++++++++++