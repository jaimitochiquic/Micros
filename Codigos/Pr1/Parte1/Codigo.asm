	; PRACTICA 1 - MICROCONTROLADORES
	; PARTE 1: Cortina con leds y reset
	; MARZO-AGOSTO 2026
	; Jaime Chiqui
	
	;*************************************************************************** 
	; Program Header ***********************************************************
		LIST p=18f4550			;Especifica el dispositivo
		#include "p18f4550.inc"	;traducción de variables a memoria
	;*************************************************************************** 
	
	
	;***** DECLARACIÓN VARIABLES ******
	Contador1 		equ	0x00
	Contador2		equ 0x01
	ContaDelay100	equ 0x02
	Xveces			equ 0x03
	NH				equ 0x04
	NL				equ 0x05
	ContadorInicio  equ 0x06
	;**********************************
	
	;----- Definición de Vectores --------------
		org		0x00   ; Vector de Inicio / reset
		goto 	Inicio
	;-------------------------------------------
	
	; ++++ INICIO DEL PROGRAMA +++++++++++++++++
		org 	0x20	; Dónde comienza el programa 
	Inicio:
	
	;-------------------------------------------
	; Configuracion de perifericos
		CLRF 	TRISD		; TRISD = 00000000
		BCF 	UCON, 3		; RC4 Y RC5 entradas (S2 Y S3)
		BSF 	UCFG, 3		; Especial RC4 Y RC5
		BCF     TRISC,0		; led_piloto
	;-------------------------------------------
	;******* Inicializacion de I/O ********
		CLRF	LATD		; Empezamos con el Puerto D apagado
		BCF     LATC, 0   	; Led_piloto OFF
	;******************************************
	
	;--- INICIALIZACIÓN DE VARIABLES ------
		MOVLW	d'198'			;W = 198
		MOVWF	Contador1		;Contador1 = 10
		MOVLW	d'21'			;W = 21
		MOVWF	Contador2		;Contador2 = 10
		MOVLW   d'20'           
    	MOVWF   ContadorInicio  ;RC0 parpadea 20 veces / 50 ms

;------- SECUENCIA DE INICIO -------
	BucleInicio:
	    BSF     LATC, 0         ; Enciende LED RC0-Led_piloto
	    MOVLW   d'50'
	    CALL    DelayXms
	    BCF     LATC, 0         ; Apaga LED RC0-Led_piloto
	    MOVLW   d'50'
	    CALL    DelayXms
	    DECFSZ  ContadorInicio, 1 ; Decrementa y salta si es cero
	    GOTO    BucleInicio
	    
	    BSF     LATC, 0         ; RC0-Led_piloto ENCENDIDO
;-------------------------------------------------
	Reset_NH_NL:
		MOVLW 	b'10000000'
		MOVWF 	NH
		MOVLW	b'00000001'
		MOVWF 	NL
		CLRF	LATD
;--------------------------------------
	EncuestaON:				; Espera a S2 para iniciar secuencia
		BTFSC 	PORTC, 4
		GOTO 	EncuestaON
	
	;******* BUCLE CORTINA ********
	Cortina:
		; --- Boton Apagar Todo (S3) ---
	    BTFSS   PORTC, 5        ; S3 es RC5
	    GOTO    Apagar_Todo     ; Si se presiona S3

		MOVF	NH, W
		IORWF	NL, W
		MOVWF	LATD
		
		MOVLW	d'50'
		CALL	DelayXms
		
		RRNCF 	NH, 1
		RLNCF 	NL, 1
		BTFSS 	NH, 0
		GOTO 	Sigue
		GOTO 	Rota1

	Sigue:   
	; sirve para cuando hace el efecto espejo no se haga 2 veces
		BTFSS 	NH, 3
		GOTO	Cortina
	
	Rota1:
		RRNCF 	NH, 1
		RLNCF 	NL, 1
		GOTO 	Cortina
; -------------------------------
	Apagar_Todo:  ; s3
	    CLRF    LATD            ; Apaga todos los LEDs del puerto RD
; -------------------------------
	Boton_S3:
	    BTFSS   PORTC, 5
	    GOTO    Boton_S3
		CALL    Delay100ms
		BTFSC 	PORTC, 5
	    GOTO    Reset_NH_NL;
	
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
	end
	;+++++++++++++++++++++++++++++++++++++++