	.text
	.globl _min_caml_start
	.align 2
fib.1:
	add	sp, sp, -32
	sw	ra, 24(sp)
	li	t1, 1
	bgt	a0, t1, ble_else.15
	j	fib.1_end
ble_else.15:
	sub	a1, a0, 1
	sw	a0, 0(sp)
	add	a0, a1, 0
	call	fib.1
	lw	a1, 0(sp)
	sub	a1, a1, 2
	sw	a0, 4(sp)
	add	a0, a1, 0
	call	fib.1
	lw	a1, 4(sp)
	add	a0, a1, a0
	j	fib.1_end
fib.1_end:
	lw	ra, 24(sp)
	add	sp, sp, 32
	jr	ra
_min_caml_start: # main entry point
	add	sp, sp, -16
	sw	ra, 8(sp)
#	main program starts
	li	a0, 6
	call	fib.1
	call	min_caml_print_int
#	main program ends
	lw	ra, 8(sp)
	add	sp, sp, 16
	jr	 ra
