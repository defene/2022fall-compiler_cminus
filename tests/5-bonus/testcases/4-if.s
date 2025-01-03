	.text
	.globl main
	.type main, @function
main:
	addi.d $sp, $sp, -16
	st.d $ra, $sp, 8
	st.d $fp, $sp, 0
	addi.d $fp, $sp, 16
.mainlabel_entry:
	b .mainlabel8
.mainlabel8:
	b .mainlabel17
.mainlabel17:
	ori $a0, $zero, 33
	bl output
	b .mainlabel16
.mainlabel16:
	b .mainlabel7
.mainlabel7:
	ori $a0, $zero, 0
	ld.d $ra, $sp, 8
	ld.d $fp, $sp, 0
	addi.d $sp, $sp, 16
	jr $ra
.mainlabel15:
	ori $a0, $zero, 22
	bl output
	b .mainlabel16
.mainlabel3:
	b .mainlabel14
.mainlabel14:
	ori $a0, $zero, 33
	bl output
	b .mainlabel13
.mainlabel13:
	b .mainlabel7
.mainlabel12:
	ori $a0, $zero, 11
	bl output
	b .mainlabel13
