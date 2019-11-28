;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

            .text                           ; Assemble into program memory.


			.global convert					; define convert como global para ser usada
											;em c

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

convert:

               mov.w	#SREF_0+ADC10SHT_2+ADC10ON, ADC10CTL0;declarando conversor AD
               mov.w	#INCH_4+SHS_0+ADC10DIV_0+ADC10SSEL_0+CONSEQ_0, ADC10CTL1; SELECIONANDO P1.4
			   mov.w	#BIT4, ADC10AE0; selecionando p1.4
			   add.w	#ENC, ADC10CTL0
	;ADC10CTL0 = SREF_0|ADC10SHT_2|ADC10ON; // declarando conversor AD
   ; ADC10CTL1 = INCH_4|SHS_0|ADC10DIV_0|ADC10SSEL_0|CONSEQ_0; // SELECIONANDO P1.4
 ;   ADC10AE0 = BIT4;//selecionando p1.4
   ; ADC10CTL0 |= ENC;
LEITURA:
			   add.w #ADC10SC, ADC10CTL0; inicia conversão
LOOP:
			   bit.w 	#BUSY, ADC10CTL1; espera terminar conversão
			   jnz		LOOP
			   mov.w	ADC10MEM, R12; após converter, salva em R12 para ser usado na main.c
			   ret					 ; finaliza subrotina

			.end
