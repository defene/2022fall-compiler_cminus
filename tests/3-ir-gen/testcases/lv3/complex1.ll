; ModuleID = 'cminus'
source_filename = "complex1.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @mod(i32 %arg0, i32 %arg1) {
label_entry:
  %op2 = sdiv i32 %arg0, %arg1
  %op3 = mul i32 %op2, %arg1
  %op4 = sub i32 %arg0, %op3
  ret i32 %op4
}
define void @printfour(i32 %arg0) {
label_entry:
  %op1 = call i32 @mod(i32 %arg0, i32 10000)
  %op2 = call i32 @mod(i32 %op1, i32 10)
  %op3 = sdiv i32 %op1, 10
  %op4 = call i32 @mod(i32 %op3, i32 10)
  %op5 = sdiv i32 %op3, 10
  %op6 = call i32 @mod(i32 %op5, i32 10)
  %op7 = sdiv i32 %op5, 10
  call void @output(i32 %op7)
  call void @output(i32 %op6)
  call void @output(i32 %op4)
  call void @output(i32 %op2)
  ret void
}
define void @main() {
label_entry:
  %op0 = alloca [2801 x i32]
  br label %label1
label1:                                                ; preds = %label_entry, %label10
  %op2 = phi i32 [ 0, %label_entry ], [ %op12, %label10 ]
  %op3 = icmp slt i32 %op2, 2800
  %op4 = zext i1 %op3 to i32
  %op5 = icmp ne i32 %op4, 0
  br i1 %op5, label %label6, label %label8
label6:                                                ; preds = %label1
  %op7 = icmp slt i32 %op2, 0
  br i1 %op7, label %label9, label %label10
label8:                                                ; preds = %label1
  br label %label13
label9:                                                ; preds = %label6
  call void @neg_idx_except()
  ret void
label10:                                                ; preds = %label6
  %op11 = getelementptr [2801 x i32], [2801 x i32]* %op0, i32 0, i32 %op2
  store i32 2000, i32* %op11
  %op12 = add i32 %op2, 1
  br label %label1
label13:                                                ; preds = %label8, %label31
  %op14 = phi i32 [ %op23, %label31 ], [ undef, %label8 ]
  %op15 = phi i32 [ 0, %label8 ], [ %op34, %label31 ]
  %op16 = phi i32 [ %op24, %label31 ], [ undef, %label8 ]
  %op17 = phi i32 [ 2800, %label8 ], [ %op35, %label31 ]
  %op18 = phi i32 [ %op25, %label31 ], [ undef, %label8 ]
  %op19 = icmp ne i32 %op17, 0
  br i1 %op19, label %label20, label %label21
label20:                                                ; preds = %label13
  br label %label22
label21:                                                ; preds = %label13
  ret void
label22:                                                ; preds = %label20, %label56
  %op23 = phi i32 [ 0, %label20 ], [ %op57, %label56 ]
  %op24 = phi i32 [ %op16, %label20 ], [ %op43, %label56 ]
  %op25 = phi i32 [ %op17, %label20 ], [ %op50, %label56 ]
  %op26 = icmp ne i32 %op25, 0
  %op27 = zext i1 %op26 to i32
  %op28 = icmp ne i32 %op27, 0
  br i1 %op28, label %label29, label %label31
label29:                                                ; preds = %label22
  %op30 = icmp slt i32 %op25, 0
  br i1 %op30, label %label36, label %label37
label31:                                                ; preds = %label22
  %op32 = sdiv i32 %op23, 10000
  %op33 = add i32 %op15, %op32
  call void @printfour(i32 %op33)
  %op34 = call i32 @mod(i32 %op23, i32 10000)
  %op35 = sub i32 %op17, 14
  br label %label13
label36:                                                ; preds = %label29
  call void @neg_idx_except()
  ret void
label37:                                                ; preds = %label29
  %op38 = getelementptr [2801 x i32], [2801 x i32]* %op0, i32 0, i32 %op25
  %op39 = load i32, i32* %op38
  %op40 = mul i32 %op39, 10000
  %op41 = add i32 %op23, %op40
  %op42 = mul i32 2, %op25
  %op43 = sub i32 %op42, 1
  %op44 = call i32 @mod(i32 %op41, i32 %op43)
  %op45 = icmp slt i32 %op25, 0
  br i1 %op45, label %label46, label %label47
label46:                                                ; preds = %label37
  call void @neg_idx_except()
  ret void
label47:                                                ; preds = %label37
  %op48 = getelementptr [2801 x i32], [2801 x i32]* %op0, i32 0, i32 %op25
  store i32 %op44, i32* %op48
  %op49 = sdiv i32 %op41, %op43
  %op50 = sub i32 %op25, 1
  %op51 = icmp ne i32 %op50, 0
  %op52 = zext i1 %op51 to i32
  %op53 = icmp ne i32 %op52, 0
  br i1 %op53, label %label54, label %label56
label54:                                                ; preds = %label47
  %op55 = mul i32 %op49, %op50
  br label %label56
label56:                                                ; preds = %label47, %label54
  %op57 = phi i32 [ %op49, %label47 ], [ %op55, %label54 ]
  br label %label22
}
