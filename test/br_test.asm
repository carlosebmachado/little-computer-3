.ORIG x3000
	
	; testes feitos até o momento:
	; ADD
	; AND

; BRn LABEL
; BRz LABEL
; BRp LABEL
; BR LABEL
; BRzp LABEL
; BRnp LABEL
; BRnz LABEL
; BRnzp LABEL


	ADD R3, R3, #1
	BRP POK
	HALT
	
POK:	
	ADD R3, R3, #-2
	BRN NOK
	HALT	; finaliza o programa

NOK:	ADD R3, R3, #1
	BRZ ZOK
	HALT	; finaliza o programa

ZOK:	ADD R3, R3, #3
	HALT	; finaliza o programa

.END
