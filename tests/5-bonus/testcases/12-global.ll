; ModuleID = 'cminus'
source_filename = "12-global.cminus"

@seed = global i32 zeroinitializer
declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @randomLCG() {
label_entry:
  %op0 = load i32, i32* @seed
  %op1 = mul i32 %op0, 1103515245
  %op2 = add i32 %op1, 12345
  store i32 %op2, i32* @seed
  %op3 = load i32, i32* @seed
  ret i32 %op3
}
define i32 @randBin() {
label_entry:
  %op0 = call i32 @randomLCG()
  %op1 = icmp sgt i32 %op0, 0
  %op2 = zext i1 %op1 to i32
  %op3 = icmp ne i32 %op2, 0
  br i1 %op3, label %label4, label %label6
label4:                                                ; preds = %label_entry
  ret i32 1
label5:
  ret i32 0
label6:                                                ; preds = %label_entry
  ret i32 0
}
define i32 @returnToZeroSteps() {
label_entry:
  br label %label0
label0:                                                ; preds = %label_entry, %label21
  %op1 = phi i32 [ 0, %label_entry ], [ %op14, %label21 ]
  %op2 = phi i32 [ 0, %label_entry ], [ %op13, %label21 ]
  %op3 = icmp slt i32 %op1, 20
  %op4 = zext i1 %op3 to i32
  %op5 = icmp ne i32 %op4, 0
  br i1 %op5, label %label6, label %label9
label6:                                                ; preds = %label0
  %op7 = call i32 @randBin()
  %op8 = icmp ne i32 %op7, 0
  br i1 %op8, label %label10, label %label18
label9:                                                ; preds = %label0
  ret i32 20
label10:                                                ; preds = %label6
  %op11 = add i32 %op2, 1
  br label %label12
label12:                                                ; preds = %label10, %label18
  %op13 = phi i32 [ %op11, %label10 ], [ %op19, %label18 ]
  %op14 = add i32 %op1, 1
  %op15 = icmp eq i32 %op13, 0
  %op16 = zext i1 %op15 to i32
  %op17 = icmp ne i32 %op16, 0
  br i1 %op17, label %label20, label %label21
label18:                                                ; preds = %label6
  %op19 = sub i32 %op2, 1
  br label %label12
label20:                                                ; preds = %label12
  ret i32 %op14
label21:                                                ; preds = %label12
  br label %label0
}
define i32 @main() {
label_entry:
  store i32 3407, i32* @seed
  br label %label0
label0:                                                ; preds = %label_entry, %label5
  %op1 = phi i32 [ 0, %label_entry ], [ %op7, %label5 ]
  %op2 = icmp slt i32 %op1, 20
  %op3 = zext i1 %op2 to i32
  %op4 = icmp ne i32 %op3, 0
  br i1 %op4, label %label5, label %label8
label5:                                                ; preds = %label0
  %op6 = call i32 @returnToZeroSteps()
  call void @output(i32 %op6)
  %op7 = add i32 %op1, 1
  br label %label0
label8:                                                ; preds = %label0
  ret i32 0
}
