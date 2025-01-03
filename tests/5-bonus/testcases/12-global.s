	.text
	.comm seed, 4
	.globl randomLCG
	.type randomLCG, @function
randomLCG:
	addi.d $sp, $sp, -32
	st.d $ra, $sp, 24
	st.d $fp, $sp, 16
	addi.d $fp, $sp, 32
.randomLCGlabel_entry:
	la.local $r21, seed
	ld.d $t0, $r21, 0
	lu12i.w $r21, 1103511552 >> 12
	ori $r21, $r21, 3693
	mul.w $t0, $t0, $r21
	lu12i.w $r24, 12288 >> 12
	ori $r24, $r24, 57
	add.w $t0, $t0, $r24
	la.local $r21, seed
	st.d $t0, $r21, 0
	la.local $r21, seed
	ld.d $t0, $r21, 0
	or $a0, $zero, $t0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
	.globl randBin
	.type randBin, @function
randBin:
	addi.d $sp, $sp, -32
	st.d $ra, $sp, 24
	st.d $fp, $sp, 16
	addi.d $fp, $sp, 32
.randBinlabel_entry:
	bl randomLCG
	or $t0, $r4, $zero 
	or $r23, $zero, $t0
	addi.w $r21, $zero, 0
	ori $t0, $zero, 1
	bge $r23, $r21, .randBinlabel4
	ori $t0, $zero, 0
	b .randBinlabel6
.randBinlabel4:
	ori $a0, $zero, 1
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.randBinlabel5:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.randBinlabel6:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
	.globl returnToZeroSteps
	.type returnToZeroSteps, @function
returnToZeroSteps:
	addi.d $sp, $sp, -48
	st.d $ra, $sp, 40
	st.d $fp, $sp, 32
	addi.d $fp, $sp, 48
.returnToZeroStepslabel_entry:
	ori $t1, $zero, 0
	ori $t0, $zero, 0
	b .returnToZeroStepslabel0
.returnToZeroStepslabel0:
	or $r23, $zero, $t0
	addi.w $r21, $zero, 20
	ori $t2, $zero, 1
	blt $r23, $r21, .returnToZeroStepslabel6
	ori $t2, $zero, 0
	b .returnToZeroStepslabel9
.returnToZeroStepslabel6:
	st.d $t0, $fp, -24
	st.d $t1, $fp, -32
	bl randBin
	or $t2, $r4, $zero 
	ld.d $t0, $fp, -24
	ld.d $t1, $fp, -32
	or $r23, $zero, $t2
	addi.w $r21, $zero, 0
	ori $t2, $zero, 1
	bne $r23, $r21, .returnToZeroStepslabel10
	ori $t2, $zero, 0
	b .returnToZeroStepslabel18
.returnToZeroStepslabel9:
	ori $a0, $zero, 20
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.returnToZeroStepslabel10:
	addi.w $t2, $t1, 1
	or $t2, $zero, $t2
	b .returnToZeroStepslabel12
.returnToZeroStepslabel12:
	addi.w $t0, $t0, 1
	addi.w $r21, $zero, 0
	ori $t3, $zero, 1
	beq $r21, $t2, .returnToZeroStepslabel20
	ori $t3, $zero, 0
	b .returnToZeroStepslabel21
.returnToZeroStepslabel18:
	addi.w $t1, $t1, -1
	or $t2, $zero, $t1
	b .returnToZeroStepslabel12
.returnToZeroStepslabel20:
	or $a0, $zero, $t0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.returnToZeroStepslabel21:
	or $t1, $zero, $t2
	or $t0, $zero, $t0
	b .returnToZeroStepslabel0
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -32
	st.d $ra, $sp, 24
	st.d $fp, $sp, 16
	addi.d $fp, $sp, 32
.mainlabel_entry:
	la.local $r21, seed
	lu12i.w $r23, 0 >> 12
	ori $r23, $r23, 3407
	st.d $r23, $r21, 0
	ori $t0, $zero, 0
	b .mainlabel0
.mainlabel0:
	or $r23, $zero, $t0
	addi.w $r21, $zero, 20
	ori $t1, $zero, 1
	blt $r23, $r21, .mainlabel5
	ori $t1, $zero, 0
	b .mainlabel8
.mainlabel5:
	st.d $t0, $fp, -24
	bl returnToZeroSteps
	or $t1, $r4, $zero 
	ld.d $t0, $fp, -24
	st.d $t0, $fp, -24
	or $a0, $zero, $t1
	bl output
	ld.d $t0, $fp, -24
	addi.w $t0, $t0, 1
	or $t0, $zero, $t0
	b .mainlabel0
.mainlabel8:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
