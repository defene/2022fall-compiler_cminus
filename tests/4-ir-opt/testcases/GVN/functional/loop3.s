	.text
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -16
	st.d $ra, $sp, 8
	addi.d $fp, $sp, 16
	addi.w $a0, $zero,0
	ld.d $ra, $sp, 8
	addi.d $sp, $sp, 16
	jr $ra
