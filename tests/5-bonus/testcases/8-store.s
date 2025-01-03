	.text
	.globl store
	.type store, @function
store:
	addi.d $sp, $sp, -48
	st.d $ra, $sp, 40
	st.d $fp, $sp, 32
	addi.d $fp, $sp, 48
.storelabel_entry:
	or $r23, $zero, $a1
	addi.w $r21, $zero, 0
	ori $t0, $zero, 1
	blt $r23, $r21, .storelabel5
	ori $t0, $zero, 0
	b .storelabel6
.storelabel5:
	st.w $t0, $fp, -48
	st.d $a0, $fp, -24
	st.w $a1, $fp, -28
	st.w $a2, $fp, -32
	bl neg_idx_except
	ld.d $a0, $fp, -24
	ld.w $a1, $fp, -28
	ld.w $a2, $fp, -32
	ld.w $t0, $fp, -48
	ori $a0, $zero, 0
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
.storelabel6:
	slli.w $r24, $a1, 2
	add.d $t0, $a0, $r24
	st.w $a2, $t0, 0
	or $a0, $zero, $a2
	ld.d $ra, $sp, 40
	ld.d $fp, $sp, 32
	addi.d $sp, $sp, 48
	jr $ra
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -80
	st.d $ra, $sp, 72
	st.d $fp, $sp, 64
	addi.d $fp, $sp, 80
.mainlabel_entry:
	ori $t1, $zero, 0
	b .mainlabel1
.mainlabel1:
	or $r23, $zero, $t1
	addi.w $r21, $zero, 10
	ori $t2, $zero, 1
	blt $r23, $r21, .mainlabel6
	ori $t2, $zero, 0
	b .mainlabel11
.mainlabel6:
	addi.d $t2, $fp, -56
	ori $t3, $zero, 2
	mul.w $t3, $t1, $t3
	st.w $t0, $fp, -68
	st.w $t1, $fp, -72
	or $a0, $zero, $t2
	or $a1, $zero, $t1
	or $a2, $zero, $t3
	bl store
	or $t2, $r4, $zero 
	ld.w $t0, $fp, -68
	ld.w $t1, $fp, -72
	addi.w $t1, $t1, 1
	or $t1, $zero, $t1
	b .mainlabel1
.mainlabel11:
	ori $t2, $zero, 0
	ori $t1, $zero, 0
	b .mainlabel12
.mainlabel12:
	or $r23, $zero, $t2
	addi.w $r21, $zero, 10
	ori $t3, $zero, 1
	blt $r23, $r21, .mainlabel18
	ori $t3, $zero, 0
	b .mainlabel20
.mainlabel18:
	or $r23, $zero, $t2
	addi.w $r21, $zero, 0
	ori $t3, $zero, 1
	blt $r23, $r21, .mainlabel21
	ori $t3, $zero, 0
	b .mainlabel22
.mainlabel20:
	st.w $t0, $fp, -68
	st.w $t1, $fp, -72
	st.w $t2, $fp, -76
	st.w $t3, $fp, -80
	or $a0, $zero, $t1
	bl output
	ld.w $t0, $fp, -68
	ld.w $t1, $fp, -72
	ld.w $t2, $fp, -76
	ld.w $t3, $fp, -80
	ori $a0, $zero, 0
	ld.d $ra, $sp, 72
	ld.d $fp, $sp, 64
	addi.d $sp, $sp, 80
	jr $ra
.mainlabel21:
	st.w $t0, $fp, -68
	st.w $t1, $fp, -72
	st.w $t2, $fp, -76
	st.w $t3, $fp, -80
	bl neg_idx_except
	ld.w $t0, $fp, -68
	ld.w $t1, $fp, -72
	ld.w $t2, $fp, -76
	ld.w $t3, $fp, -80
	ori $a0, $zero, 0
	ld.d $ra, $sp, 72
	ld.d $fp, $sp, 64
	addi.d $sp, $sp, 80
	jr $ra
.mainlabel22:
	slli.w $r24, $t2, 2
	addi.w $r24, $r24, -56
	add.d $t0, $fp, $r24
	ld.w $t0, $t0, 0
	add.w $t0, $t1, $t0
	addi.w $t1, $t2, 1
	or $t2, $zero, $t1
	or $t1, $zero, $t0
	b .mainlabel12
