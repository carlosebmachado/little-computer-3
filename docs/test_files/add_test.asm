.ORIG x3000
	
	ADD R2, R3, R4	; R2 := R3 + R4
	ADD R2, R3, #7	; R2 := R3 + 7
	
	ADD R0, R0, R2	; R0 := R0 + R2
	ADD R0, R0, #-7	; R0 := R1 - 7

	HALT	; finaliza o programa

.END
