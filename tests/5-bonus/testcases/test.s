	.text
	.comm n, 4
	.comm m, 4
	.comm w, 20
	.comm v, 20
	.comm dp, 264
	.globl max
	.type max, @function
max:
	addi.d $sp, $sp, -32
	st.d $ra, $sp, 24
	st.d $fp, $sp, 16
	addi.d $fp, $sp, 32
.maxlabel_entry:
	or $r24, $zero, $a1
	or $r23, $zero, $a0
	ori $t0, $zero, 1
	bge $r23, $r24, .maxlabel5
	ori $t0, $zero, 0
	b .maxlabel7
.maxlabel5:
	or $a0, $zero, $a0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.maxlabel6:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.maxlabel7:
	or $a0, $zero, $a1
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
	.globl knapsack
	.type knapsack, @function
knapsack:
	addi.d $sp, $sp, -48
	st.d $ra, $sp, 40
	st.d $fp, $sp, 32
	addi.d $fp, $sp, 48
.knapsacklabel_entry:
	or $r23, $zero, $a1
	addi.w $r21, $zero, 0
	ori $t0, $zero, 1
	blt $r23, $r21, .knapsacklabel5
	beq $r23, $r21, .knapsacklabel5
	ori $t0, $zero, 0
	b .knapsacklabel6
.knapsacklabel5:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.knapsacklabel6:
	addi.w $r21, $zero, 0
	ori $t0, $zero, 1
	beq $r21, $a0, .knapsacklabel10
	ori $t0, $zero, 0
	b .knapsacklabel11
.knapsacklabel10:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.knapsacklabel11:
	ori $t0, $zero, 11
	mul.w $t0, $a0, $t0
	add.w $t0, $t0, $a1
	or $r23, $zero, $t0
	addi.w $r21, $zero, 0
	ori $t1, $zero, 1
	blt $r23, $r21, .knapsacklabel15
	ori $t1, $zero, 0
	b .knapsacklabel16
.knapsacklabel15:
	st.w $t0, $fp, -28
	st.w $t1, $fp, -32
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	bl neg_idx_except
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.w $t0, $fp, -28
	ld.w $t1, $fp, -32
	ori $a0, $zero, 0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.knapsacklabel16:
	la.local $r23, dp
	slli.d $r24, $t0, 2
	add.d $t0, $r23, $r24
	ld.w $t2, $t0, 0
	or $r23, $zero, $t2
	addi.w $r21, $zero, 0
	ori $t2, $zero, 1
	bge $r23, $r21, .knapsacklabel22
	beq $r23, $r21, .knapsacklabel22
	ori $t2, $zero, 0
	b .knapsacklabel26
.knapsacklabel22:
	bnez $t1, .knapsacklabel29
	b .knapsacklabel30
.knapsacklabel26:
	addi.w $t2, $a0, -1
	or $r23, $zero, $t2
	addi.w $r21, $zero, 0
	ori $t3, $zero, 1
	blt $r23, $r21, .knapsacklabel33
	ori $t3, $zero, 0
	b .knapsacklabel34
.knapsacklabel29:
	st.d $t0, $fp, -32
	st.w $t1, $fp, -36
	st.w $t2, $fp, -40
	st.w $t3, $fp, -44
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	bl neg_idx_except
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -36
	ld.w $t2, $fp, -40
	ld.w $t3, $fp, -44
	ori $a0, $zero, 0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.knapsacklabel30:
	ld.w $t4, $t0, 0
	or $a0, $zero, $t4
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.knapsacklabel33:
	st.d $t0, $fp, -32
	st.w $t1, $fp, -36
	st.w $t2, $fp, -40
	st.w $t3, $fp, -44
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	bl neg_idx_except
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -36
	ld.w $t2, $fp, -40
	ld.w $t3, $fp, -44
	ori $a0, $zero, 0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.knapsacklabel34:
	la.local $r23, w
	slli.d $r24, $t2, 2
	add.d $t4, $r23, $r24
	ld.w $t5, $t4, 0
	or $r23, $zero, $a1
	or $r24, $zero, $t5
	ori $t5, $zero, 1
	blt $r23, $r24, .knapsacklabel40
	ori $t5, $zero, 0
	b .knapsacklabel48
.knapsacklabel40:
	st.d $t0, $fp, -32
	st.w $t1, $fp, -36
	st.w $t2, $fp, -40
	st.w $t3, $fp, -44
	st.d $t4, $fp, -52
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	or $a0, $zero, $t2
	or $a1, $zero, $a1
	bl knapsack
	or $t5, $r4, $zero 
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -36
	ld.w $t2, $fp, -40
	ld.w $t3, $fp, -44
	ld.d $t4, $fp, -52
	or $t5, $zero, $t5
	b .knapsacklabel43
.knapsacklabel43:
	st.d $t0, $fp, -32
	st.w $t1, $fp, -36
	st.w $t2, $fp, -40
	st.w $t3, $fp, -44
	st.d $t4, $fp, -52
	st.w $t5, $fp, -56
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	or $a0, $zero, $t5
	bl output
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -36
	ld.w $t2, $fp, -40
	ld.w $t3, $fp, -44
	ld.d $t4, $fp, -52
	ld.w $t5, $fp, -56
	bnez $t1, .knapsacklabel68
	b .knapsacklabel69
.knapsacklabel48:
	st.d $t0, $fp, -32
	st.w $t2, $fp, -36
	st.w $t3, $fp, -40
	st.d $t4, $fp, -48
	st.w $t5, $fp, -52
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	or $a0, $zero, $t2
	or $a1, $zero, $a1
	bl knapsack
	or $t1, $r4, $zero 
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.d $t0, $fp, -32
	ld.w $t2, $fp, -36
	ld.w $t3, $fp, -40
	ld.d $t4, $fp, -48
	ld.w $t5, $fp, -52
	bnez $t3, .knapsacklabel54
	b .knapsacklabel55
.knapsacklabel54:
	st.d $t0, $fp, -32
	st.w $t1, $fp, -36
	st.w $t2, $fp, -40
	st.w $t3, $fp, -44
	st.d $t4, $fp, -52
	st.w $t5, $fp, -56
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	bl neg_idx_except
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -36
	ld.w $t2, $fp, -40
	ld.w $t3, $fp, -44
	ld.d $t4, $fp, -52
	ld.w $t5, $fp, -56
	ori $a0, $zero, 0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.knapsacklabel55:
	ld.w $t4, $t4, 0
	sub.w $t4, $a1, $t4
	st.d $t0, $fp, -32
	st.w $t1, $fp, -36
	st.w $t2, $fp, -40
	st.w $t3, $fp, -44
	st.w $t5, $fp, -48
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	or $a0, $zero, $t2
	or $a1, $zero, $t4
	bl knapsack
	or $t4, $r4, $zero 
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -36
	ld.w $t2, $fp, -40
	ld.w $t3, $fp, -44
	ld.w $t5, $fp, -48
	bnez $t3, .knapsacklabel62
	b .knapsacklabel63
.knapsacklabel62:
	st.d $t0, $fp, -32
	st.w $t1, $fp, -36
	st.w $t2, $fp, -40
	st.w $t4, $fp, -44
	st.w $t5, $fp, -48
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	bl neg_idx_except
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -36
	ld.w $t2, $fp, -40
	ld.w $t4, $fp, -44
	ld.w $t5, $fp, -48
	ori $a0, $zero, 0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.knapsacklabel63:
	la.local $r23, v
	slli.d $r24, $t2, 2
	add.d $t2, $r23, $r24
	ld.w $t2, $t2, 0
	add.w $t2, $t4, $t2
	st.d $t0, $fp, -32
	st.w $t5, $fp, -36
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	or $a0, $zero, $t1
	or $a1, $zero, $t2
	bl max
	or $t1, $r4, $zero 
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.d $t0, $fp, -32
	ld.w $t5, $fp, -36
	or $t5, $zero, $t1
	b .knapsacklabel43
.knapsacklabel68:
	st.d $t0, $fp, -32
	st.w $t5, $fp, -36
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
	bl neg_idx_except
	ld.w $a0, $fp, -20
	ld.w $a1, $fp, -24
	ld.d $t0, $fp, -32
	ld.w $t5, $fp, -36
	ori $a0, $zero, 0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.knapsacklabel69:
	st.w $t5, $t0, 0
	or $a0, $zero, $t5
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
	la.local $r21, n
	addi.w $r23, $zero, 5
	st.w $r23, $r21, 0
	la.local $r21, m
	addi.w $r23, $zero, 10
	st.w $r23, $r21, 0
	b .mainlabel2
.mainlabel1:
	bl neg_idx_except
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel2:
	la.local $r23, w
	addi.d $t0, $r23, 0
	addi.w $r23, $zero, 2
	st.w $r23, $t0, 0
	b .mainlabel6
.mainlabel5:
	bl neg_idx_except
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel6:
	la.local $r23, w
	addi.d $t0, $r23, 4
	addi.w $r23, $zero, 2
	st.w $r23, $t0, 0
	b .mainlabel10
.mainlabel9:
	bl neg_idx_except
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel10:
	la.local $r23, w
	addi.d $t0, $r23, 8
	addi.w $r23, $zero, 6
	st.w $r23, $t0, 0
	b .mainlabel14
.mainlabel13:
	bl neg_idx_except
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel14:
	la.local $r23, w
	addi.d $t0, $r23, 12
	addi.w $r23, $zero, 5
	st.w $r23, $t0, 0
	b .mainlabel18
.mainlabel17:
	bl neg_idx_except
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel18:
	la.local $r23, w
	addi.d $t0, $r23, 16
	addi.w $r23, $zero, 4
	st.w $r23, $t0, 0
	b .mainlabel22
.mainlabel21:
	bl neg_idx_except
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel22:
	la.local $r23, v
	addi.d $t0, $r23, 0
	addi.w $r23, $zero, 6
	st.w $r23, $t0, 0
	b .mainlabel26
.mainlabel25:
	bl neg_idx_except
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel26:
	la.local $r23, v
	addi.d $t0, $r23, 4
	addi.w $r23, $zero, 3
	st.w $r23, $t0, 0
	b .mainlabel30
.mainlabel29:
	bl neg_idx_except
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel30:
	la.local $r23, v
	addi.d $t0, $r23, 8
	addi.w $r23, $zero, 5
	st.w $r23, $t0, 0
	b .mainlabel34
.mainlabel33:
	bl neg_idx_except
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel34:
	la.local $r23, v
	addi.d $t0, $r23, 12
	addi.w $r23, $zero, 4
	st.w $r23, $t0, 0
	b .mainlabel38
.mainlabel37:
	bl neg_idx_except
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel38:
	la.local $r23, v
	addi.d $t0, $r23, 16
	addi.w $r23, $zero, 6
	st.w $r23, $t0, 0
	ori $t0, $zero, 0
	b .mainlabel40
.mainlabel40:
	or $r23, $zero, $t0
	addi.w $r21, $zero, 66
	ori $t1, $zero, 1
	blt $r23, $r21, .mainlabel45
	ori $t1, $zero, 0
	b .mainlabel48
.mainlabel45:
	or $r23, $zero, $t0
	addi.w $r21, $zero, 0
	ori $t1, $zero, 1
	blt $r23, $r21, .mainlabel52
	ori $t1, $zero, 0
	b .mainlabel53
.mainlabel48:
	la.local $r21, n
	ld.w $t1, $r21, 0
	la.local $r21, m
	ld.w $t2, $r21, 0
	st.w $t0, $fp, -24
	or $a0, $zero, $t1
	or $a1, $zero, $t2
	bl knapsack
	or $t1, $r4, $zero 
	ld.w $t0, $fp, -24
	st.w $t0, $fp, -24
	or $a0, $zero, $t1
	bl output
	ld.w $t0, $fp, -24
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel52:
	st.w $t0, $fp, -24
	bl neg_idx_except
	ld.w $t0, $fp, -24
	ori $a0, $zero, 0
	ld.d $ra, $sp, 24
	ld.d $fp, $sp, 16
	addi.d $sp, $sp, 32
	jr $ra
.mainlabel53:
	la.local $r23, dp
	slli.d $r24, $t0, 2
	add.d $t1, $r23, $r24
	addi.w $r23, $zero, -1
	st.w $r23, $t1, 0
	addi.w $t0, $t0, 1
	or $t0, $zero, $t0
	b .mainlabel40
