	.text
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -80
	st.d $ra, $sp, 72
	st.d $fp, $sp, 64
	addi.d $fp, $sp, 80
.mainlabel_entry:
	b .mainlabel3
.mainlabel3:
	addi.d $t1, $fp, -56
	addi.w $r23, $zero, 11
	st.w $r23, $fp, -56
	b .mainlabel7
.mainlabel7:
	addi.d $t2, $fp, -40
	addi.w $r23, $zero, 22
	st.w $r23, $fp, -40
	b .mainlabel11
.mainlabel11:
	addi.d $t0, $fp, -20
	addi.w $r23, $zero, 33
	st.w $r23, $fp, -20
	b .mainlabel15
.mainlabel15:
	ld.w $t1, $fp, -56
	st.w $t0, $fp, -68
	st.w $t2, $fp, -72
	add.w $a0, $zero, $t1
	bl output
	ld.w $t0, $fp, -68
	ld.w $t2, $fp, -72
	b .mainlabel20
.mainlabel20:
	ld.w $t1, $fp, -40
	st.w $t0, $fp, -68
	add.w $a0, $zero, $t1
	bl output
	ld.w $t0, $fp, -68
	b .mainlabel25
.mainlabel25:
	ld.w $t0, $fp, -20
	add.w $a0, $zero, $t0
	bl output
	addi.w $a0, $zero, 0
	ld.d $ra, $sp, 72
	ld.d $fp, $sp, 64
	addi.d $sp, $sp, 80
	jr $ra
.mainlabel24:
	bl neg_idx_except
	addi.w $a0, $zero, 0
	ld.d $ra, $sp, 72
	ld.d $fp, $sp, 64
	addi.d $sp, $sp, 80
	jr $ra
.mainlabel19:
	bl neg_idx_except
	addi.w $a0, $zero, 0
	ld.d $ra, $sp, 72
	ld.d $fp, $sp, 64
	addi.d $sp, $sp, 80
	jr $ra
.mainlabel14:
	bl neg_idx_except
	addi.w $a0, $zero, 0
	ld.d $ra, $sp, 72
	ld.d $fp, $sp, 64
	addi.d $sp, $sp, 80
	jr $ra
.mainlabel10:
	bl neg_idx_except
	addi.w $a0, $zero, 0
	ld.d $ra, $sp, 72
	ld.d $fp, $sp, 64
	addi.d $sp, $sp, 80
	jr $ra
.mainlabel6:
	bl neg_idx_except
	addi.w $a0, $zero, 0
	ld.d $ra, $sp, 72
	ld.d $fp, $sp, 64
	addi.d $sp, $sp, 80
	jr $ra
.mainlabel2:
	bl neg_idx_except
	addi.w $a0, $zero, 0
	ld.d $ra, $sp, 72
	ld.d $fp, $sp, 64
	addi.d $sp, $sp, 80
	jr $ra
