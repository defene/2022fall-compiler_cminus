; ModuleID = 'cminus'
source_filename = "6-array.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  %op0 = alloca [10 x i32]
  br i1 false, label %label2, label %label3
label2:                                                ; preds = %label_entry
  call void @neg_idx_except()
  ret i32 0
label3:                                                ; preds = %label_entry
  %op4 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 0
  store i32 11, i32* %op4
  br i1 false, label %label6, label %label7
label6:                                                ; preds = %label3
  call void @neg_idx_except()
  ret i32 0
label7:                                                ; preds = %label3
  %op8 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 4
  store i32 22, i32* %op8
  br i1 false, label %label10, label %label11
label10:                                                ; preds = %label7
  call void @neg_idx_except()
  ret i32 0
label11:                                                ; preds = %label7
  %op12 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 9
  store i32 33, i32* %op12
  br i1 false, label %label14, label %label15
label14:                                                ; preds = %label11
  call void @neg_idx_except()
  ret i32 0
label15:                                                ; preds = %label11
  %op17 = load i32, i32* %op4
  call void @output(i32 %op17)
  br i1 false, label %label19, label %label20
label19:                                                ; preds = %label15
  call void @neg_idx_except()
  ret i32 0
label20:                                                ; preds = %label15
  %op22 = load i32, i32* %op8
  call void @output(i32 %op22)
  br i1 false, label %label24, label %label25
label24:                                                ; preds = %label20
  call void @neg_idx_except()
  ret i32 0
label25:                                                ; preds = %label20
  %op27 = load i32, i32* %op12
  call void @output(i32 %op27)
  ret i32 0
}
