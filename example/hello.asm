	.ORIG x3000
	LEA R0, HW	; carrega a string para o registrador
	PUTS       	; printa a string no console
	HALT       	; finaliza o programa

HW	.STRINGZ "Hello world!"
	.END
