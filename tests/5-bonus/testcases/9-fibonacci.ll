; ModuleID = 'cminus'
source_filename = "9-fibonacci.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @fibonacci(i32 %arg0) {
label_entry:
  %op1 = icmp eq i32 %arg0, 0
  %op2 = zext i1 %op1 to i32
  %op3 = icmp ne i32 %op2, 0
  br i1 %op3, label %label4, label %label6
label4:                                                ; preds = %label_entry
  ret i32 0
label5:                                                ; preds = %label11
  ret i32 0
label6:                                                ; preds = %label_entry
  %op7 = icmp eq i32 %arg0, 1
  %op8 = zext i1 %op7 to i32
  %op9 = icmp ne i32 %op8, 0
  br i1 %op9, label %label10, label %label12
label10:                                                ; preds = %label6
  ret i32 1
label11:
  br label %label5
label12:                                                ; preds = %label6
  %op13 = sub i32 %arg0, 1
  %op14 = call i32 @fibonacci(i32 %op13)
  %op15 = sub i32 %arg0, 2
  %op16 = call i32 @fibonacci(i32 %op15)
  %op17 = add i32 %op14, %op16
  ret i32 %op17
}
define i32 @main() {
label_entry:
  br label %label0
label0:                                                ; preds = %label_entry, %label5
  %op1 = phi i32 [ 0, %label_entry ], [ %op7, %label5 ]
  %op2 = icmp slt i32 %op1, 10
  %op3 = zext i1 %op2 to i32
  %op4 = icmp ne i32 %op3, 0
  br i1 %op4, label %label5, label %label8
label5:                                                ; preds = %label0
  %op6 = call i32 @fibonacci(i32 %op1)
  call void @output(i32 %op6)
  %op7 = add i32 %op1, 1
  br label %label0
label8:                                                ; preds = %label0
  ret i32 0
}
