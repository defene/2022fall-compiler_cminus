; ModuleID = 'cminus'
source_filename = "pure_func.cminus"

@a = global [10000 x i32] zeroinitializer
@b = global [10000 x i32] zeroinitializer
declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @max(i32 %arg0, i32 %arg1) {
label_entry:
  %op2 = icmp sgt i32 %arg0, %arg1
  %op3 = zext i1 %op2 to i32
  %op4 = icmp ne i32 %op3, 0
  br i1 %op4, label %label5, label %label6
label5:                                                ; preds = %label_entry
  ret i32 %arg0
label6:                                                ; preds = %label_entry
  ret i32 %arg1
}
define void @inputarray(i32* %arg0, i32 %arg1) {
label_entry:
  br label %label3
label3:                                                ; preds = %label_entry, %label13
  %op4 = phi i32 [ 0, %label_entry ], [ %op15, %label13 ]
  %op5 = icmp slt i32 %op4, %arg1
  %op6 = zext i1 %op5 to i32
  %op7 = icmp ne i32 %op6, 0
  br i1 %op7, label %label8, label %label11
label8:                                                ; preds = %label3
  %op9 = call i32 @input()
  %op10 = icmp slt i32 %op4, 0
  br i1 %op10, label %label12, label %label13
label11:                                                ; preds = %label3
  ret void
label12:                                                ; preds = %label8
  call void @neg_idx_except()
  ret void
label13:                                                ; preds = %label8
  %op14 = getelementptr i32, i32* %arg0, i32 %op4
  store i32 %op9, i32* %op14
  %op15 = add i32 %op4, 1
  br label %label3
}
define i32 @main() {
label_entry:
  %op0 = call i32 @input()
  %op1 = getelementptr [10000 x i32], [10000 x i32]* @a, i32 0, i32 0
  call void @inputarray(i32* %op1, i32 %op0)
  %op2 = getelementptr [10000 x i32], [10000 x i32]* @b, i32 0, i32 0
  call void @inputarray(i32* %op2, i32 %op0)
  br label %label3
label3:                                                ; preds = %label_entry, %label27
  %op6 = phi i32 [ 0, %label_entry ], [ %op29, %label27 ]
  %op7 = icmp slt i32 %op6, %op0
  %op8 = zext i1 %op7 to i32
  %op9 = icmp ne i32 %op8, 0
  br i1 %op9, label %label10, label %label12
label10:                                                ; preds = %label3
  %op11 = icmp slt i32 %op6, 0
  br i1 %op11, label %label13, label %label14
label12:                                                ; preds = %label3
  br label %label30
label13:                                                ; preds = %label10
  call void @neg_idx_except()
  ret i32 0
label14:                                                ; preds = %label10
  %op15 = getelementptr [10000 x i32], [10000 x i32]* @a, i32 0, i32 %op6
  %op16 = load i32, i32* %op15
  br i1 %op11, label %label18, label %label19
label18:                                                ; preds = %label14
  call void @neg_idx_except()
  ret i32 0
label19:                                                ; preds = %label14
  %op20 = getelementptr [10000 x i32], [10000 x i32]* @b, i32 0, i32 %op6
  %op21 = load i32, i32* %op20
  %op22 = call i32 @max(i32 %op16, i32 %op21)
  %op24 = mul i32 %op22, %op22
  br i1 %op11, label %label26, label %label27
label26:                                                ; preds = %label19
  call void @neg_idx_except()
  ret i32 0
label27:                                                ; preds = %label19
  store i32 %op24, i32* %op15
  %op29 = add i32 %op6, 1
  br label %label3
label30:                                                ; preds = %label12, %label39
  %op31 = phi i32 [ 0, %label12 ], [ %op42, %label39 ]
  %op32 = icmp slt i32 %op31, %op0
  %op33 = zext i1 %op32 to i32
  %op34 = icmp ne i32 %op33, 0
  br i1 %op34, label %label35, label %label37
label35:                                                ; preds = %label30
  %op36 = icmp slt i32 %op31, 0
  br i1 %op36, label %label38, label %label39
label37:                                                ; preds = %label30
  ret i32 0
label38:                                                ; preds = %label35
  call void @neg_idx_except()
  ret i32 0
label39:                                                ; preds = %label35
  %op40 = getelementptr [10000 x i32], [10000 x i32]* @a, i32 0, i32 %op31
  %op41 = load i32, i32* %op40
  call void @output(i32 %op41)
  %op42 = add i32 %op31, 1
  br label %label30
}
