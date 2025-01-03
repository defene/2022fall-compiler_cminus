	.text
	.globl fibonacci
	.type fibonacci, @function
fibonacci:
	addi.d $sp, $sp, -32
	st.d $ra, $sp, 24
	st.d $fp, $sp, 16
	addi.d $fp, $sp, 32
.fibonaccilabel_entry:
	addi.w $r21, $zero, 0
	ori $t0, $zero, 1
	beq $r21, $a0, .fibonaccilabel4
	ori $t0, $zero, 0
	b .fibonaccilabel6
.fibonaccilabel4:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.fibonaccilabel5:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.fibonaccilabel6:
	addi.w $r21, $zero, 1
	ori $t0, $zero, 1
	beq $r21, $a0, .fibonaccilabel10
	ori $t0, $zero, 0
	b .fibonaccilabel12
.fibonaccilabel10:
	ori $a0, $zero, 1
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.fibonaccilabel11:
	b .fibonaccilabel5
.fibonaccilabel12:
	addi.w $t0, $a0, -1
	st.w $a0, $fp, -20
	or $a0, $zero, $t0
	bl fibonacci
	or $t0, $r4, $zero 
	ld.w $a0, $fp, -20
	addi.w $t1, $a0, -2
	st.w $t0, $fp, -28
	st.w $a0, $fp, -20
	or $a0, $zero, $t1
	bl fibonacci
	or $t1, $r4, $zero 
	ld.w $a0, $fp, -20
	ld.w $t0, $fp, -28
	add.w $t0, $t0, $t1
	or $a0, $zero, $t0
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
	ori $t0, $zero, 0
	b .mainlabel0
.mainlabel0:
	or $r23, $zero, $t0
	addi.w $r21, $zero, 10
	ori $t1, $zero, 1
	blt $r23, $r21, .mainlabel5
	ori $t1, $zero, 0
	b .mainlabel8
.mainlabel5:
	st.w $t0, $fp, -28
	or $a0, $zero, $t0
	bl fibonacci
	or $t1, $r4, $zero 
	ld.w $t0, $fp, -28
	st.w $t0, $fp, -28
	or $a0, $zero, $t1
	bl output
	ld.w $t0, $fp, -28
	addi.w $t0, $t0, 1
	or $t0, $zero, $t0
	b .mainlabel0
.mainlabel8:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
