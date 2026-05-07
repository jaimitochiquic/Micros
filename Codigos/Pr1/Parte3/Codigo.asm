	; PRACTICA 1 - MICROCONTROLADORES
	; PARTE 3: Maquina de estados 3 sec.
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
	ContadorInicio  equ 0x06	; Led parpadeante
	B_Estados		equ 0x07   	; Para los 3 estados
	ContaCiclos     equ 0x08	; 3 azul, 3 rojos
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

		MOVLW   d'0'    ; Inicia con la cortina
        MOVWF   B_Estados

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

;/////////////////////////////////////
	EncuestaON:				; Espera a S2 para iniciar secuencia
		BTFSC 	PORTC, 4
		GOTO 	EncuestaON
		CALL    Delay100ms     ; controla el estado transistorio
	Boton_secuencias: 
		BTFSS   PORTC, 4
		GOTO 	Boton_secuencias
		CALL    Delay100ms 
		GOTO    Opc_fuera 
;---------------------------------------
	Opcion_sec:
		; Al hacer una resta buscamos comparar el estado del boton con W = 1
		; es decir si son iguales 1-1 = 0 (estamos en el estado 1)
		; caso contrario si tengo estado 2-1= 1 (No estamos en estado 1 probar con el 2)

		MOVLW	d'1'
		SUBWF   B_Estados, W; compara estado del boton y con w=1 (estado1)
		BTFSC 	STATUS, Z
		GOTO    Llamar_Cortina

		MOVLW	d'2'
		SUBWF   B_Estados, W; compara estado del boton y con w=2 (estado2)
		BTFSC 	STATUS, Z
		GOTO    Llamar_Ola

		MOVLW	d'3'
		SUBWF   B_Estados, W; compara estado del boton y con w=3 (estado2)
		BTFSC 	STATUS, Z
		GOTO    Llamar_Policia
		RETURN

		;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
;	------------------------
	Llamar_Cortina:
		CALL 	Cortina
		RETURN
	Llamar_Ola:
		CALL 	Ola
		RETURN
	Llamar_Policia:
		CALL 	Luces_Policia
		RETURN
; --------------------------
	Opc_fuera:
		;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
		; PARA n estados cambiar el valor n+1 (para 6 estados cambiar el 4 por 7)
		;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°° 
		INCF    B_Estados, 1
		MOVLW	d'4'        ; No hay opc 4, hacer que sea 1
		SUBWF   B_Estados, W; compara estado del boton y con w=1 (estado1)
		BTFSS 	STATUS, Z
		GOTO 	Repetir_Estado

	opcion1:
		MOVLW   d'1'
		MOVWF   B_Estados
	Repetir_Estado:
		CALL    Opcion_sec
		
		BTFSS   PORTC, 5
		GOTO    Apagar_Todo
		BTFSS   PORTC, 4
		GOTO    Boton_secuencias
		GOTO    Repetir_Estado

	;******* BUCLE CORTINA ********
	Cortina:
		MOVLW 	b'10000000'
		MOVWF 	NH
		MOVLW	b'00000001'
		MOVWF 	NL

	Bucle_Cortina:
		; --- Boton Apagar Todo (S3) ---
	    BTFSS   PORTC, 5        
	    RETURN
		BTFSS   PORTC, 4
		RETURN

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
		GOTO	Bucle_Cortina
	
	Rota1:
		RRNCF 	NH, 1
		RLNCF 	NL, 1
		GOTO 	Bucle_Cortina

	;******** OLA ***********
	Ola:
		MOVLW   b'00000001'     ; LED izq ON (rojos)
		MOVWF   LATD
	Ola_Hacia_Izq:
		MOVLW   d'50'
		CALL    DelayXms
	
		BTFSS   PORTC, 5		; verificar boton s3
		RETURN
		BTFSS   PORTC, 4
		RETURN
	
		RLNCF   LATD, 1         ; Rota la luz a la izquierda
		BTFSS   LATD, 7         ; 10000000 (bit 7 en 1)
		GOTO    Ola_Hacia_Izq   ;
		
	Ola_Hacia_Der:
		MOVLW   d'50'
		CALL    DelayXms
		
		BTFSS   PORTC, 5    	; verificar boton s3
		RETURN
		BTFSS   PORTC, 4
		RETURN
	
		RRNCF   LATD, 1         ; Rota la luz a la derecha
		BTFSS   LATD, 0         ; 00000001  (bit 0 es 1)
		GOTO    Ola_Hacia_Der   
		
		GOTO    Ola         ; Repite la Ola

	;******* Luces Policia ***********
	Luces_Policia:
		; 3 Parpadeos Rojos (Izquierda)
		MOVLW   d'3'
		MOVWF   ContaCiclos
	Rojos_Parpadean:
		MOVLW   b'00001111'
		MOVWF   LATD
		MOVLW   d'50'
		CALL    DelayXms
		
		BTFSS   PORTC, 5     	; Boton s3
		RETURN
		BTFSS   PORTC, 4
		RETURN
	
		CLRF    LATD            ; Apaga Leds
		MOVLW   d'50'
		CALL    DelayXms
		DECFSZ  ContaCiclos, 1  ; resta al contador
		GOTO    Rojos_Parpadean
	
		; 3 Parpadeos Azules (Derecha)
		MOVLW   d'3'
		MOVWF   ContaCiclos
	Azules_Parpadean:
		MOVLW   b'11110000'
		MOVWF   LATD
		MOVLW   d'50'
		CALL    DelayXms
		
		BTFSS   PORTC, 5     	; Boton s3
		RETURN
		BTFSS   PORTC, 4
		RETURN
	
		CLRF    LATD            ; Apaga Leds
		MOVLW   d'50'
		CALL    DelayXms
		DECFSZ  ContaCiclos, 1
		GOTO    Azules_Parpadean
		
		GOTO    Luces_Policia
	;°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

; ===============================================================
	Apagar_Todo: 
	    CLRF    LATD            ; Apaga todos los LEDs del puerto RD
		CLRF    B_Estados
; -------------------------------
	Boton_S3:
	    BTFSS   PORTC, 5
	    GOTO    Boton_S3
		CALL    Delay100ms     ; controla el estado transistorio
        GOTO    EncuestaON	
	
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