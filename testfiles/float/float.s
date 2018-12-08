l.20:	 # 48.300300
	.word	1111569282
l.19:	 # 4.500000
	.word	1083179008
l.18:	 # -12.300000
	.word	-1052455731
_min_caml_start: # main entry point
	add	sp, sp, -4
	li	t1, l.18
	flw	f0, 0(t1)
	call	min_caml_abs_float
	fsqrt.s	f0, f0
	call	min_caml_cos
	call	min_caml_sin
	li	t1, l.19
	flw	f1, 0(t1)
	fadd.s	f0, f0, f1
	li	t1, l.20
	flw	f1, 0(t1)
	fsub.s	f0, f0, f1
	li	a0, 1000
	fsw	f0, 0(sp)
	call	min_caml_float_of_int
	flw	f1, 0(sp)
	fmul.s	f0, f1, f0
	call	min_caml_int_of_float
	call	min_caml_print_int
