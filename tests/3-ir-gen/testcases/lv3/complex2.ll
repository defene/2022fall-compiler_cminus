; ModuleID = 'cminus'
source_filename = "complex2.cminus"

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
define void @sort(float* %arg0, i32 %arg1, float %arg2) {
label_entry:
  br label %label4
label4:                                                ; preds = %label_entry, %label36
  %op7 = phi i32 [ %arg1, %label_entry ], [ %op39, %label36 ]
  %op9 = fsub float %arg2, 0x3ff0000000000000
  %op10 = sitofp i32 %op7 to float
  %op11 = fcmp ult float %op10,%op9
  %op12 = zext i1 %op11 to i32
  %op13 = icmp ne i32 %op12, 0
  br i1 %op13, label %label14, label %label19
label14:                                                ; preds = %label4
  %op16 = fptosi float %arg2 to i32
  %op17 = call i32 @minloc(float* %arg0, float %op10, i32 %op16)
  %op18 = icmp slt i32 %op17, 0
  br i1 %op18, label %label20, label %label21
label19:                                                ; preds = %label4
  ret void
label20:                                                ; preds = %label14
  call void @neg_idx_except()
  ret void
label21:                                                ; preds = %label14
  %op22 = getelementptr float, float* %arg0, i32 %op17
  %op23 = load float, float* %op22
  %op24 = fptosi float %op23 to i32
  %op25 = icmp slt i32 %op7, 0
  br i1 %op25, label %label26, label %label27
label26:                                                ; preds = %label21
  call void @neg_idx_except()
  ret void
label27:                                                ; preds = %label21
  %op28 = getelementptr float, float* %arg0, i32 %op7
  %op29 = load float, float* %op28
  br i1 %op18, label %label31, label %label32
label31:                                                ; preds = %label27
  call void @neg_idx_except()
  ret void
label32:                                                ; preds = %label27
  store float %op29, float* %op22
  br i1 %op25, label %label35, label %label36
label35:                                                ; preds = %label32
  call void @neg_idx_except()
  ret void
label36:                                                ; preds = %label32
  %op38 = sitofp i32 %op24 to float
  store float %op38, float* %op28
  %op39 = add i32 %op7, 1
  br label %label4
}
define void @main() {
label_entry:
  br label %label0
label0:                                                ; preds = %label_entry, %label12
  %op1 = phi i32 [ 0, %label_entry ], [ %op15, %label12 ]
  %op2 = icmp slt i32 %op1, 10
  %op3 = zext i1 %op2 to i32
  %op4 = icmp ne i32 %op3, 0
  br i1 %op4, label %label5, label %label8
label5:                                                ; preds = %label0
  %op6 = call i32 @input()
  %op7 = icmp slt i32 %op1, 0
  br i1 %op7, label %label11, label %label12
label8:                                                ; preds = %label0
  %op9 = getelementptr [10 x float], [10 x float]* @x, i32 0, i32 0
  call void @sort(float* %op9, i32 0, float 0x4024000000000000)
  br label %label16
label11:                                                ; preds = %label5
  call void @neg_idx_except()
  ret void
label12:                                                ; preds = %label5
  %op13 = getelementptr [10 x float], [10 x float]* @x, i32 0, i32 %op1
  %op14 = sitofp i32 %op6 to float
  store float %op14, float* %op13
  %op15 = add i32 %op1, 1
  br label %label0
label16:                                                ; preds = %label8, %label25
  %op17 = phi i32 [ 0, %label8 ], [ %op29, %label25 ]
  %op18 = icmp slt i32 %op17, 10
  %op19 = zext i1 %op18 to i32
  %op20 = icmp ne i32 %op19, 0
  br i1 %op20, label %label21, label %label23
label21:                                                ; preds = %label16
  %op22 = icmp slt i32 %op17, 0
  br i1 %op22, label %label24, label %label25
label23:                                                ; preds = %label16
  ret void
label24:                                                ; preds = %label21
  call void @neg_idx_except()
  ret void
label25:                                                ; preds = %label21
  %op26 = getelementptr [10 x float], [10 x float]* @x, i32 0, i32 %op17
  %op27 = load float, float* %op26
  %op28 = fptosi float %op27 to i32
  call void @output(i32 %op28)
  %op29 = add i32 %op17, 1
  br label %label16
}
