	.text
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -16
	st.d $ra, $sp, 8
	st.d $fp, $sp, 0
	addi.d $fp, $sp, 16
mainlabel_entry:
	addi.w $a0, $zero, 123
	ld.d $ra, $sp, 8
	ld.d $fp, $sp, 0
	addi.d $sp, $sp, 16
	jr $ra
