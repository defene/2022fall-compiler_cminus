; ModuleID = 'cminus'
source_filename = "/labs/bylab/2022fall-compiler_cminus/tests/4-ir-opt/testcases/GVN/functional/new.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  br label %label0
label0:                                                ; preds = %label_entry, %label28
  %op1 = phi i32 [ 2, %label_entry ], [ %op31, %label28 ]
  %op2 = phi i32 [ 2, %label_entry ], [ %op14, %label28 ]
  %op3 = phi i32 [ 2, %label_entry ], [ %op18, %label28 ]
  %op7 = phi i32 [ 1, %label_entry ], [ %op30, %label28 ]
  %op8 = icmp slt i32 %op7, 10
  %op9 = zext i1 %op8 to i32
  %op10 = icmp ne i32 %op9, 0
  br i1 %op10, label %label11, label %label15
label11:                                                ; preds = %label0
  %op12 = call i32 @input()
  %op13 = call i32 @input()
  %op14 = add i32 %op12, %op13
  br label %label17
label15:                                                ; preds = %label0
  %op16 = add i32 %op3, %op2
  call void @output(i32 %op1)
  call void @output(i32 %op16)
  ret i32 0
label17:                                                ; preds = %label11, %label24
  %op18 = phi i32 [ %op14, %label11 ], [ %op14, %label24 ]
  %op19 = phi i32 [ 1, %label11 ], [ %op26, %label24 ]
  %op20 = phi i32 [ %op7, %label11 ], [ %op25, %label24 ]
  %op21 = icmp slt i32 %op19, 10
  %op22 = zext i1 %op21 to i32
  %op23 = icmp ne i32 %op22, 0
  br i1 %op23, label %label24, label %label28
label24:                                                ; preds = %label17
  %op25 = add i32 %op20, %op19
  %op26 = add i32 %op25, %op19
  br label %label17
label28:                                                ; preds = %label17
  %op30 = add i32 %op20, 1
  %op31 = add i32 %op18, %op14
  call void @output(i32 %op18)
  call void @output(i32 %op14)
  br label %label0
}
