	.text
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -16
	st.d $ra, $sp, 8
	st.d $fp, $sp, 0
	addi.d $fp, $sp, 16
.mainlabel_entry:
	la.local $r21, .LC0
	fld.s $ft8, $r21, 0
	fmov.s $fa0, $ft8
	bl outputFloat
	ori $a0, $zero, 0
	ld.d $ra, $sp, 8
	ld.d $fp, $sp, 0
	addi.d $sp, $sp, 16
	jr $ra
.LC0:
	.word 0x40366667
