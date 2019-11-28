;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

            .text                           ; Assemble into program memory.


			.global delay					; define delay como global para ser usada
											;em c

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

delay:
			   mov.w	#999, 	TACCR0 	;999		define variáveis do clk
               		   add.w	#TACLR, 	TACTL 	;		para obter delay de 1 ms
               		   mov.w	#TASSEL_2+ID_0+MC_1, TACTL;

comp:			   ; função para identificar se já se passou o tempo pedido pelo usuário
			   bit.w	#65535,	R12 	; entrada do tempo de delay vem em R12 (padrão do CCS)
			   jz		end	; se for zero, vai pra função end, caso contrário vai para loop

loop:
			   bit.w	#TAIFG, TACTL ; testa se já se passou 1 ms
			   jz		loop ; fica em loop até ter rodado 1 ms
			   add.w	#65535, R12; subtrai 1, 65535 é o complemento de dois de 1‬
			   jmp		delay; após subtrair 1, volta para a comparação
end:

			   add.w 	 #MC_0, TACTL; ativa mc0 pra economizar energia
			   ret					 ; finaliza subrotina

			   .end
