fib.1:
	add	sp, sp, -12
	sw	ra, 0(sp)
	li	t1, 1
	bgt	a0, t1, ble_else.17
	lw	ra, 0(sp)
	add	sp, sp, 12
	ret
ble_else.17:
	sub	a1, a0, 1
	sw	a0, 4(sp)
	mv	a0, a1
	call	fib.1
	lw	a1, 4(sp)
	sub	a1, a1, 2
	sw	a0, 8(sp)
	mv	a0, a1
	call	fib.1
	lw	a1, 8(sp)
	lw	ra, 0(sp)
	add	a0, a1, a0
	add	sp, sp, 12
	ret
_min_caml_start: # main entry point
	li	a0, 6
	call	fib.1
	call	min_caml_print_int
