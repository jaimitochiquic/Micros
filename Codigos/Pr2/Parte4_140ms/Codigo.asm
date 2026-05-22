	; PRACTICA 2 - MICROCONTROLADORES
	; PARTE 4: Secuencia Ola con Temporizador de 140 Segundos
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
	Xveces			equ 0x03	; Timers manuales
	ContadorInicio  equ 0x04	; Cuenta 20 veces ON/OFF led piloto 
	Sentido         equ 0x05	; Sentido izq. o der. de la ola de leds
	Contador15s     equ 0x06    ; Cuenta 30 interrupciones (15 seg)
	Flag	        equ 0x07    ; 0=Conteo ON, 1=OFF(Ya esta los 15 seg)
	ContadorMedioSeg equ 0x08   ;
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
		
		;--- Valores del TMR0 --
		MOVLW   0xD9 		; 65535 (55769)  ---> CAMBIA HEXADECIMAL
		MOVWF 	TMR0H		; PARTRE ALTA 16 BITS
		MOVLW   0xDA		; AL CAMBIAR CF A D5 ESTE CUENTA MENOS YA SI
		MOVWF	TMR0L 		; PARTRE BAJA 16 BITS

		BSF     T0CON, TMR0ON   ; Enciende el TMR0

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
	Reiniciar_Todo:
		CLRF    LATD            ; Apaga todos los LEDs del Puerto D
		CLRF    Flag	        ; Limpia 
		MOVLW   d'140'
		MOVWF   Contador15s     ; 30 interrupciones para los 15s
		MOVLW   d'2'            ; 2 INTERRUPCIONES (1 Segundo)
		MOVWF   ContadorMedioSeg
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
		BSF     Flag, 0     	; 0: ISR empieza a contar

	Ola_Hacia_Izq:
		BTFSC   Flag, 1     
		GOTO    Reiniciar_Todo  
		
		MOVLW   d'100'          
		MOVWF   Xveces          
	Espera_Izq:
	
		CALL    Delay1ms        ; Espera 1ms
		BTFSS   PORTC, 5        ; Revisa S3 cada milisegundo
		GOTO    Detener_Secuencia 
		DECFSZ  Xveces, 1       
		GOTO    Espera_Izq

		BCF     Sentido, 0		; bandera indicar sent. izq.

		RLNCF   LATD, 1         ; Rota la luz a la izquierda
		BTFSS   LATD, 7         ; 10000000 (bit 7 en 1)
		GOTO    Ola_Hacia_Izq   ;
		
	Ola_Hacia_Der:
		BTFSC   Flag, 1     
		GOTO    Reiniciar_Todo  

		MOVLW   d'100'          
		MOVWF   Xveces          
	Espera_Der:
		CALL    Delay1ms        ; Espera 1ms
		BTFSS   PORTC, 5        ; Revisa S3 cada milisegundo
		GOTO    Detener_Secuencia 
		DECFSZ  Xveces, 1       
		GOTO    Espera_Der

		BSF     Sentido, 0		; bandera indicar sent. DER.

		RRNCF   LATD, 1         ; Rota la luz a la derecha
		BTFSS   LATD, 0         ; 00000001  (bit 0 es 1)
		GOTO    Ola_Hacia_Der   
		
		GOTO    Ola_Hacia_Izq   ; Repite la Ola

; -------------------- DETENER Y NO OFF
	Detener_Secuencia:
		CALL    Delay100ms     	; Anti-rebote
	Boton_S3_D:
		BTFSC   Flag, 1         ; Tiempo pasado incluso en pausa
		GOTO    Reiniciar_Todo
		BTFSS   PORTC, 5		; Espera que se suelte S3
		GOTO    Boton_S3_D


; -------------------- CONTINUA S2
	Trans_s2:
		BTFSC   Flag, 1     
		GOTO    Reiniciar_Todo
		BTFSC 	PORTC, 4
		GOTO 	Trans_s2
		CALL    Delay100ms     	; Anti-rebote
	Continuar_s2:
		BTFSC   Flag, 1     
		GOTO    Reiniciar_Todo 
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
    	BTG     LATC, 0           ; Toggle LED piloto 1 Hz
		BCF		INTCON, TMR0IF 	  ; OFF bandera IT TIMER
	
		MOVLW   0xD9 		; 65535 (55769)  ---> CAMBIA HEXADECIMAL
		MOVWF 	TMR0H		; PARTRE ALTA 16 BITS
		MOVLW   0xDA		; AL CAMBIAR CF A D5 ESTE CUENTA MENOS YA SI
		MOVWF	TMR0L 		; PARTRE BAJA 16 BITS

		; -------- 140 seg:
		BTFSS   Flag, 0       	  ; Esta On en conteo
		GOTO    Fin_ISR           ; No hay que contar: sale programa

		DECFSZ  ContadorMedioSeg, 1    ; Sí: decrementa Contador.
		GOTO    Fin_ISR           ; Aún no pasa 1 segundo entero: sale
		
		;2 interrupciones = 1 SEGUNDO COMPLETO
		MOVLW   d'2'              ; Contador de medio segundo
		MOVWF   ContadorMedioSeg
		
		DECFSZ  Contador15s, 1    
		GOTO    Fin_ISR

		; °°°°°°° Cont. en 0 = pasaron 15 segundos exactos
		BCF     Flag, 0       ; 0: detiene el conteo
		BSF     Flag, 1  	  ; Continua contando

	Fin_ISR:
		RETFIE              ; Regresa de interrupción
	;+++++++++++++++++++++++++++++++++++++++	
	end
	;+++++++++++++++++++++++++++++++++++++++