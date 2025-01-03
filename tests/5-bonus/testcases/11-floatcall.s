	.text
	.globl mod
	.type mod, @function
mod:
	addi.d $sp, $sp, -48
	st.d $ra, $sp, 40
	st.d $fp, $sp, 32
	addi.d $fp, $sp, 48
.modlabel_entry:
	fdiv.s $ft0, $fa0, $fa1
	ftint.w.s $ft8, $ft0
	movfr2gr.s $t0, $ft8
	movgr2fr.w $ft8, $t0
	ffint.s.w $ft0, $ft8
	fmul.s $ft0, $ft0, $fa1
	fsub.s $ft0, $fa0, $ft0
	fmov.s $fa0, $ft0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -32
	st.d $ra, $sp, 24
	st.d $fp, $sp, 16
	addi.d $fp, $sp, 32
.mainlabel_entry:
	la.local $r21, .LC0
	fld.s $ft8, $r21, 0
	fmov.s $fa0, $ft8
	la.local $r21, .LC1
	fld.s $ft8, $r21, 0
	fmov.s $fa1, $ft8
	bl mod
	fmov.s $ft0, $fa0
	fmov.s $fa1, $ft0
	bl outputFloat
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.LC0:
	.word 0x41333333
.LC1:
	.word 0x400ccccd
