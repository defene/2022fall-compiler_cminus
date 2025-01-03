	.text
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -32
	st.d $ra, $sp, 24
	st.d $fp, $sp, 16
	addi.d $fp, $sp, 32
.mainlabel_entry:
	ori $t0, $zero, 0
	b .mainlabel0
.mainlabel0:
	addi.w $r21, $zero, 10
	blt $t0, $r21, .mainlabel5
	b .mainlabel7
.mainlabel7:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel5:
	st.w $t0, $fp, -28
	or $a0, $zero, $t0
	bl output
	ld.w $t0, $fp, -28
	addi.w $t0, $t0, 1
	or $t0, $zero, $t0
	b .mainlabel0
