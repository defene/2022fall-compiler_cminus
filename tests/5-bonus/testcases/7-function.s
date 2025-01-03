	.text
	.globl min
	.type min, @function
min:
	addi.d $sp, $sp, -32
	st.d $ra, $sp, 24
	st.d $fp, $sp, 16
	addi.d $fp, $sp, 32
.minlabel_entry:
	blt $a0, $a1, .minlabel5
	beq $a0, $a1, .minlabel5
	b .minlabel7
.minlabel7:
	add.w $a0, $zero, $a1
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.minlabel5:
	add.w $a0, $zero, $a0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -32
	st.d $ra, $sp, 24
	st.d $fp, $sp, 16
	addi.d $fp, $sp, 32
.mainlabel_entry:
	addi.w $a0, $zero, 11
	addi.w $a1, $zero, 22
	bl min
	or $t0, $r4, $zero 
	add.w $a0, $zero, $t0
	bl output
	addi.w $a0, $zero, 22
	addi.w $a1, $zero, 33
	bl min
	or $t0, $r4, $zero 
	add.w $a0, $zero, $t0
	bl output
	addi.w $a0, $zero, 33
	addi.w $a1, $zero, 11
	bl min
	or $t0, $r4, $zero 
	add.w $a0, $zero, $t0
	bl output
	addi.w $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
