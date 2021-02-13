.orig 0x3000

	GETC ; ascii val into R0
	ADD R3 , R0 , x0
	ADD R3 , R3 , #-16
	ADD R3 , R3 , #-16
	ADD R3 , R3 , #-16
	halt

.end