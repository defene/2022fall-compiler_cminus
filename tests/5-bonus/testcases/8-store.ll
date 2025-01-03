; ModuleID = 'cminus'
source_filename = "8-store.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @store(i32* %arg0, i32 %arg1, i32 %arg2) {
label_entry:
  %op4 = icmp slt i32 %arg1, 0
  br i1 %op4, label %label5, label %label6
label5:                                                ; preds = %label_entry
  call void @neg_idx_except()
  ret i32 0
label6:                                                ; preds = %label_entry
  %op7 = getelementptr i32, i32* %arg0, i32 %arg1
  store i32 %arg2, i32* %op7
  ret i32 %arg2
}
define i32 @main() {
label_entry:
  %op0 = alloca [10 x i32]
  br label %label1
label1:                                                ; preds = %label_entry, %label6
  %op2 = phi i32 [ 0, %label_entry ], [ %op10, %label6 ]
  %op3 = icmp slt i32 %op2, 10
  %op4 = zext i1 %op3 to i32
  %op5 = icmp ne i32 %op4, 0
  br i1 %op5, label %label6, label %label11
label6:                                                ; preds = %label1
  %op7 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 0
  %op8 = mul i32 %op2, 2
  %op9 = call i32 @store(i32* %op7, i32 %op2, i32 %op8)
  %op10 = add i32 %op2, 1
  br label %label1
label11:                                                ; preds = %label1
  br label %label12
label12:                                                ; preds = %label11, %label22
  %op13 = phi i32 [ 0, %label11 ], [ %op25, %label22 ]
  %op14 = phi i32 [ 0, %label11 ], [ %op26, %label22 ]
  %op15 = icmp slt i32 %op14, 10
  %op16 = zext i1 %op15 to i32
  %op17 = icmp ne i32 %op16, 0
  br i1 %op17, label %label18, label %label20
label18:                                                ; preds = %label12
  %op19 = icmp slt i32 %op14, 0
  br i1 %op19, label %label21, label %label22
label20:                                                ; preds = %label12
  call void @output(i32 %op13)
  ret i32 0
label21:                                                ; preds = %label18
  call void @neg_idx_except()
  ret i32 0
label22:                                                ; preds = %label18
  %op23 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 %op14
  %op24 = load i32, i32* %op23
  %op25 = add i32 %op13, %op24
  %op26 = add i32 %op14, 1
  br label %label12
}
