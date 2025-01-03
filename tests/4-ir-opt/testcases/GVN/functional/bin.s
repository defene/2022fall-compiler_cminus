	.text
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -16
	st.d $ra, $sp, 8
	addi.d $fp, $sp, 16
label_entry:
	bl input
	bl input
	bge $t0, $t1, .label5
	b .label14
label14:
	b .label9
label9:
	addi.w $a0, $r0, $t2
	bl output
	addi.w $a0, $r0, $t2
	bl output
	addi.w $a0, $zero, 0
	ld.d $ra, $sp, 8
	addi.d $sp, $sp, 16
	jr $ra
label5:
	b .label9
