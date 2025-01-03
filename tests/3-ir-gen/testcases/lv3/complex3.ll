; ModuleID = 'cminus'
source_filename = "complex3.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @gcd(i32 %arg0, i32 %arg1) {
label_entry:
  %op2 = icmp eq i32 %arg1, 0
  %op3 = zext i1 %op2 to i32
  %op4 = icmp ne i32 %op3, 0
  br i1 %op4, label %label5, label %label7
label5:                                                ; preds = %label_entry
  ret i32 %arg0
label6:
  ret i32 0
label7:                                                ; preds = %label_entry
  %op8 = sdiv i32 %arg0, %arg1
  %op9 = mul i32 %op8, %arg1
  %op10 = sub i32 %arg0, %op9
  %op11 = call i32 @gcd(i32 %arg1, i32 %op10)
  ret i32 %op11
}
define void @main() {
label_entry:
  %op0 = call i32 @input()
  %op1 = call i32 @input()
  %op2 = icmp slt i32 %op0, %op1
  %op3 = zext i1 %op2 to i32
  %op4 = icmp ne i32 %op3, 0
  br i1 %op4, label %label5, label %label6
label5:                                                ; preds = %label_entry
  br label %label6
label6:                                                ; preds = %label_entry, %label5
  %op8 = phi i32 [ %op1, %label_entry ], [ %op0, %label5 ]
  %op9 = phi i32 [ %op0, %label_entry ], [ %op1, %label5 ]
  %op10 = call i32 @gcd(i32 %op9, i32 %op8)
  call void @output(i32 %op10)
  ret void
}
