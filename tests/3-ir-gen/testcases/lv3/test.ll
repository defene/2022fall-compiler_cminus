; ModuleID = 'cminus'
source_filename = "test.cminus"

@x = global [10 x float] zeroinitializer
declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @minloc(float* %arg0, float %arg1, i32 %arg2) {
label_entry:
  %op4 = fptosi float %arg1 to i32
  %op6 = icmp slt i32 %op4, 0
  br i1 %op6, label %label7, label %label8
label7:                                                ; preds = %label_entry
  call void @neg_idx_except()
  ret i32 0
label8:                                                ; preds = %label_entry
  %op9 = getelementptr float, float* %arg0, i32 %op4
  %op10 = load float, float* %op9
  %op11 = fptosi float %op10 to i32
  %op13 = fadd float %arg1, 0x3ff0000000000000
  %op14 = fptosi float %op13 to i32
  br label %label15
label15:                                                ; preds = %label8, %label35
  %op16 = phi i32 [ %op4, %label8 ], [ %op36, %label35 ]
  %op17 = phi i32 [ %op11, %label8 ], [ %op37, %label35 ]
  %op18 = phi i32 [ %op14, %label8 ], [ %op38, %label35 ]
  %op19 = icmp slt i32 %op18, %arg2
  %op20 = zext i1 %op19 to i32
  %op21 = icmp ne i32 %op20, 0
  br i1 %op21, label %label22, label %label24
label22:                                                ; preds = %label15
  %op23 = icmp slt i32 %op18, 0
  br i1 %op23, label %label25, label %label26
label24:                                                ; preds = %label15
  ret i32 %op16
label25:                                                ; preds = %label22
  call void @neg_idx_except()
  ret i32 0
label26:                                                ; preds = %label22
  %op27 = getelementptr float, float* %arg0, i32 %op18
  %op28 = load float, float* %op27
  %op29 = sitofp i32 %op17 to float
  %op30 = fcmp ult float %op28,%op29
  %op31 = zext i1 %op30 to i32
  %op32 = icmp ne i32 %op31, 0
  br i1 %op32, label %label33, label %label35
label33:                                                ; preds = %label26
  br i1 %op23, label %label39, label %label40
label35:                                                ; preds = %label26, %label40
  %op36 = phi i32 [ %op16, %label26 ], [ %op18, %label40 ]
  %op37 = phi i32 [ %op17, %label26 ], [ %op43, %label40 ]
  %op38 = add i32 %op18, 1
  br label %label15
label39:                                                ; preds = %label33
  call void @neg_idx_except()
  ret i32 0
label40:                                                ; preds = %label33
  %op42 = load float, float* %op27
  %op43 = fptosi float %op42 to i32
  br label %label35
}
