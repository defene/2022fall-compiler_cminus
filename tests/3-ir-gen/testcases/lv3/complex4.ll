; ModuleID = 'cminus'
source_filename = "complex4.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define float @get(float* %arg0, i32 %arg1, i32 %arg2, i32 %arg3) {
label_entry:
  %op5 = mul i32 %arg1, %arg3
  %op6 = add i32 %op5, %arg2
  %op7 = icmp slt i32 %op6, 0
  br i1 %op7, label %label8, label %label9
label8:                                                ; preds = %label_entry
  call void @neg_idx_except()
  ret float 0x0
label9:                                                ; preds = %label_entry
  %op10 = getelementptr float, float* %arg0, i32 %op6
  %op11 = load float, float* %op10
  ret float %op11
}
define float @abs(float %arg0) {
label_entry:
  %op2 = fcmp ugt float %arg0,0x0
  %op3 = zext i1 %op2 to i32
  %op4 = icmp ne i32 %op3, 0
  br i1 %op4, label %label5, label %label7
label5:                                                ; preds = %label_entry
  ret float %arg0
label6:
  ret float 0x0
label7:                                                ; preds = %label_entry
  %op9 = fsub float 0x0, %arg0
  ret float %op9
}
define float @isZero(float %arg0) {
label_entry:
  %op1 = call float @abs(float %arg0)
  %op2 = fcmp ult float %op1,0x3eb0c6f7a0000000
  %op3 = zext i1 %op2 to i32
  %op4 = sitofp i32 %op3 to float
  ret float %op4
}
define i32 @gauss(float* %arg0, float* %arg1, i32 %arg2) {
label_entry:
  %op5 = add i32 %arg2, 1
  br label %label6
label6:                                                ; preds = %label_entry, %label15
  %op7 = phi i32 [ 0, %label_entry ], [ %op18, %label15 ]
  %op8 = icmp slt i32 %op7, %arg2
  %op9 = zext i1 %op8 to i32
  %op10 = icmp ne i32 %op9, 0
  br i1 %op10, label %label11, label %label13
label11:                                                ; preds = %label6
  %op12 = icmp slt i32 %op7, 0
  br i1 %op12, label %label14, label %label15
label13:                                                ; preds = %label6
  br label %label19
label14:                                                ; preds = %label11
  call void @neg_idx_except()
  ret i32 0
label15:                                                ; preds = %label11
  %op16 = getelementptr float, float* %arg0, i32 %op7
  store float 0x0, float* %op16
  %op18 = add i32 %op7, 1
  br label %label6
label19:                                                ; preds = %label13, %label87
  %op21 = phi i32 [ 0, %label13 ], [ %op93, %label87 ]
  %op23 = phi i32 [ 0, %label13 ], [ %op92, %label87 ]
  %op26 = icmp slt i32 %op23, %arg2
  %op27 = zext i1 %op26 to i32
  %op28 = icmp ne i32 %op27, 0
  br i1 %op28, label %label29, label %label31
label29:                                                ; preds = %label19
  %op30 = add i32 %op23, 1
  br label %label33
label31:                                                ; preds = %label19
  %op32 = sub i32 %arg2, 1
  br label %label142
label33:                                                ; preds = %label29, %label52
  %op34 = phi i32 [ %op23, %label29 ], [ %op53, %label52 ]
  %op35 = phi i32 [ %op30, %label29 ], [ %op54, %label52 ]
  %op36 = icmp slt i32 %op35, %arg2
  %op37 = zext i1 %op36 to i32
  %op38 = icmp ne i32 %op37, 0
  br i1 %op38, label %label39, label %label47
label39:                                                ; preds = %label33
  %op40 = call float @get(float* %arg1, i32 %op35, i32 %op21, i32 %op5)
  %op41 = call float @abs(float %op40)
  %op42 = call float @get(float* %arg1, i32 %op34, i32 %op21, i32 %op5)
  %op43 = call float @abs(float %op42)
  %op44 = fcmp ugt float %op41,%op43
  %op45 = zext i1 %op44 to i32
  %op46 = icmp ne i32 %op45, 0
  br i1 %op46, label %label51, label %label52
label47:                                                ; preds = %label33
  %op48 = icmp ne i32 %op34, %op23
  %op49 = zext i1 %op48 to i32
  %op50 = icmp ne i32 %op49, 0
  br i1 %op50, label %label55, label %label56
label51:                                                ; preds = %label39
  br label %label52
label52:                                                ; preds = %label39, %label51
  %op53 = phi i32 [ %op34, %label39 ], [ %op35, %label51 ]
  %op54 = add i32 %op35, 1
  br label %label33
label55:                                                ; preds = %label47
  br label %label62
label56:                                                ; preds = %label47, %label74
  %op59 = call float @get(float* %arg1, i32 %op23, i32 %op21, i32 %op5)
  %op60 = call float @isZero(float %op59)
  %op61 = fcmp une float %op60,0x0
  br i1 %op61, label %label85, label %label94
label62:                                                ; preds = %label55, %label82
  %op64 = phi i32 [ %op23, %label55 ], [ %op84, %label82 ]
  %op65 = icmp slt i32 %op64, %op5
  %op66 = zext i1 %op65 to i32
  %op67 = icmp ne i32 %op66, 0
  br i1 %op67, label %label68, label %label74
label68:                                                ; preds = %label62
  %op69 = call float @get(float* %arg1, i32 %op23, i32 %op64, i32 %op5)
  %op70 = call float @get(float* %arg1, i32 %op34, i32 %op64, i32 %op5)
  %op71 = mul i32 %op23, %op5
  %op72 = add i32 %op71, %op64
  %op73 = icmp slt i32 %op72, 0
  br i1 %op73, label %label75, label %label76
label74:                                                ; preds = %label62
  br label %label56
label75:                                                ; preds = %label68
  call void @neg_idx_except()
  ret i32 0
label76:                                                ; preds = %label68
  %op77 = getelementptr float, float* %arg1, i32 %op72
  store float %op70, float* %op77
  %op78 = mul i32 %op34, %op5
  %op79 = add i32 %op78, %op64
  %op80 = icmp slt i32 %op79, 0
  br i1 %op80, label %label81, label %label82
label81:                                                ; preds = %label76
  call void @neg_idx_except()
  ret i32 0
label82:                                                ; preds = %label76
  %op83 = getelementptr float, float* %arg1, i32 %op79
  store float %op69, float* %op83
  %op84 = add i32 %op64, 1
  br label %label62
label85:                                                ; preds = %label56
  %op86 = sub i32 %op23, 1
  br label %label87
label87:                                                ; preds = %label85, %label109
  %op89 = phi i32 [ %op86, %label85 ], [ %op23, %label109 ]
  %op92 = add i32 %op89, 1
  %op93 = add i32 %op21, 1
  br label %label19
label94:                                                ; preds = %label56
  br label %label96
label96:                                                ; preds = %label94, %label114
  %op99 = phi i32 [ %op30, %label94 ], [ %op117, %label114 ]
  %op100 = icmp slt i32 %op99, %arg2
  %op101 = zext i1 %op100 to i32
  %op102 = icmp ne i32 %op101, 0
  br i1 %op102, label %label103, label %label109
label103:                                                ; preds = %label96
  %op104 = call float @get(float* %arg1, i32 %op99, i32 %op21, i32 %op5)
  %op105 = call float @isZero(float %op104)
  %op107 = fsub float 0x3ff0000000000000, %op105
  %op108 = fcmp une float %op107,0x0
  br i1 %op108, label %label110, label %label114
label109:                                                ; preds = %label96
  br label %label87
label110:                                                ; preds = %label103
  %op111 = call float @get(float* %arg1, i32 %op99, i32 %op21, i32 %op5)
  %op112 = call float @get(float* %arg1, i32 %op23, i32 %op21, i32 %op5)
  %op113 = fdiv float %op111, %op112
  br label %label118
label114:                                                ; preds = %label103, %label127
  %op117 = add i32 %op99, 1
  br label %label96
label118:                                                ; preds = %label110, %label139
  %op119 = phi i32 [ %op21, %label110 ], [ %op141, %label139 ]
  %op120 = icmp slt i32 %op119, %op5
  %op121 = zext i1 %op120 to i32
  %op122 = icmp ne i32 %op121, 0
  br i1 %op122, label %label123, label %label127
label123:                                                ; preds = %label118
  %op124 = mul i32 %op99, %op5
  %op125 = add i32 %op124, %op119
  %op126 = icmp slt i32 %op125, 0
  br i1 %op126, label %label128, label %label129
label127:                                                ; preds = %label118
  br label %label114
label128:                                                ; preds = %label123
  call void @neg_idx_except()
  ret i32 0
label129:                                                ; preds = %label123
  %op130 = getelementptr float, float* %arg1, i32 %op125
  %op131 = load float, float* %op130
  %op132 = call float @get(float* %arg1, i32 %op23, i32 %op119, i32 %op5)
  %op133 = fmul float %op132, %op113
  %op134 = fsub float %op131, %op133
  br i1 %op126, label %label138, label %label139
label138:                                                ; preds = %label129
  call void @neg_idx_except()
  ret i32 0
label139:                                                ; preds = %label129
  store float %op134, float* %op130
  %op141 = add i32 %op119, 1
  br label %label118
label142:                                                ; preds = %label31, %label182
  %op145 = phi i32 [ %op32, %label31 ], [ %op184, %label182 ]
  %op146 = icmp sge i32 %op145, 0
  %op147 = zext i1 %op146 to i32
  %op148 = icmp ne i32 %op147, 0
  br i1 %op148, label %label149, label %label152
label149:                                                ; preds = %label142
  %op150 = call float @get(float* %arg1, i32 %op145, i32 %arg2, i32 %op5)
  %op151 = add i32 %op145, 1
  br label %label153
label152:                                                ; preds = %label142
  ret i32 0
label153:                                                ; preds = %label149, %label172
  %op154 = phi float [ %op150, %label149 ], [ %op173, %label172 ]
  %op155 = phi i32 [ %op151, %label149 ], [ %op174, %label172 ]
  %op156 = icmp slt i32 %op155, %arg2
  %op157 = zext i1 %op156 to i32
  %op158 = icmp ne i32 %op157, 0
  br i1 %op158, label %label159, label %label165
label159:                                                ; preds = %label153
  %op160 = call float @get(float* %arg1, i32 %op145, i32 %op155, i32 %op5)
  %op161 = call float @isZero(float %op160)
  %op163 = fsub float 0x3ff0000000000000, %op161
  %op164 = fcmp une float %op163,0x0
  br i1 %op164, label %label169, label %label172
label165:                                                ; preds = %label153
  %op166 = call float @get(float* %arg1, i32 %op145, i32 %op145, i32 %op5)
  %op167 = fdiv float %op154, %op166
  %op168 = icmp slt i32 %op145, 0
  br i1 %op168, label %label181, label %label182
label169:                                                ; preds = %label159
  %op170 = call float @get(float* %arg1, i32 %op145, i32 %op155, i32 %op5)
  %op171 = icmp slt i32 %op155, 0
  br i1 %op171, label %label175, label %label176
label172:                                                ; preds = %label159, %label176
  %op173 = phi float [ %op154, %label159 ], [ %op180, %label176 ]
  %op174 = add i32 %op155, 1
  br label %label153
label175:                                                ; preds = %label169
  call void @neg_idx_except()
  ret i32 0
label176:                                                ; preds = %label169
  %op177 = getelementptr float, float* %arg0, i32 %op155
  %op178 = load float, float* %op177
  %op179 = fmul float %op170, %op178
  %op180 = fsub float %op154, %op179
  br label %label172
label181:                                                ; preds = %label165
  call void @neg_idx_except()
  ret i32 0
label182:                                                ; preds = %label165
  %op183 = getelementptr float, float* %arg0, i32 %op145
  store float %op167, float* %op183
  %op184 = sub i32 %op145, 1
  br label %label142
}
define void @main() {
label_entry:
  %op0 = alloca [3 x float]
  %op1 = alloca [12 x float]
  br i1 false, label %label3, label %label4
label3:                                                ; preds = %label_entry
  call void @neg_idx_except()
  ret void
label4:                                                ; preds = %label_entry
  %op5 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 0
  store float 0x3ff0000000000000, float* %op5
  br i1 false, label %label8, label %label9
label8:                                                ; preds = %label4
  call void @neg_idx_except()
  ret void
label9:                                                ; preds = %label4
  %op10 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 1
  store float 0x4000000000000000, float* %op10
  br i1 false, label %label13, label %label14
label13:                                                ; preds = %label9
  call void @neg_idx_except()
  ret void
label14:                                                ; preds = %label9
  %op15 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 2
  store float 0x3ff0000000000000, float* %op15
  br i1 false, label %label18, label %label19
label18:                                                ; preds = %label14
  call void @neg_idx_except()
  ret void
label19:                                                ; preds = %label14
  %op20 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 3
  store float 0x3ff0000000000000, float* %op20
  br i1 false, label %label25, label %label26
label25:                                                ; preds = %label19
  call void @neg_idx_except()
  ret void
label26:                                                ; preds = %label19
  %op27 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 4
  store float 0x4000000000000000, float* %op27
  br i1 false, label %label32, label %label33
label32:                                                ; preds = %label26
  call void @neg_idx_except()
  ret void
label33:                                                ; preds = %label26
  %op34 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 5
  store float 0x4008000000000000, float* %op34
  br i1 false, label %label39, label %label40
label39:                                                ; preds = %label33
  call void @neg_idx_except()
  ret void
label40:                                                ; preds = %label33
  %op41 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 6
  store float 0x4010000000000000, float* %op41
  br i1 false, label %label46, label %label47
label46:                                                ; preds = %label40
  call void @neg_idx_except()
  ret void
label47:                                                ; preds = %label40
  %op48 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 7
  store float 0x4008000000000000, float* %op48
  br i1 false, label %label53, label %label54
label53:                                                ; preds = %label47
  call void @neg_idx_except()
  ret void
label54:                                                ; preds = %label47
  %op55 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 8
  store float 0x3ff0000000000000, float* %op55
  br i1 false, label %label60, label %label61
label60:                                                ; preds = %label54
  call void @neg_idx_except()
  ret void
label61:                                                ; preds = %label54
  %op62 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 9
  store float 0x3ff0000000000000, float* %op62
  br i1 false, label %label68, label %label69
label68:                                                ; preds = %label61
  call void @neg_idx_except()
  ret void
label69:                                                ; preds = %label61
  %op70 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 10
  store float 0xc000000000000000, float* %op70
  br i1 false, label %label75, label %label76
label75:                                                ; preds = %label69
  call void @neg_idx_except()
  ret void
label76:                                                ; preds = %label69
  %op77 = getelementptr [12 x float], [12 x float]* %op1, i32 0, i32 11
  store float 0x0, float* %op77
  %op79 = getelementptr [3 x float], [3 x float]* %op0, i32 0, i32 0
  %op81 = call i32 @gauss(float* %op79, float* %op5, i32 3)
  br label %label82
label82:                                                ; preds = %label76, %label91
  %op83 = phi i32 [ 0, %label76 ], [ %op94, %label91 ]
  %op84 = icmp slt i32 %op83, 3
  %op85 = zext i1 %op84 to i32
  %op86 = icmp ne i32 %op85, 0
  br i1 %op86, label %label87, label %label89
label87:                                                ; preds = %label82
  %op88 = icmp slt i32 %op83, 0
  br i1 %op88, label %label90, label %label91
label89:                                                ; preds = %label82
  ret void
label90:                                                ; preds = %label87
  call void @neg_idx_except()
  ret void
label91:                                                ; preds = %label87
  %op92 = getelementptr [3 x float], [3 x float]* %op0, i32 0, i32 %op83
  %op93 = load float, float* %op92
  call void @outputFloat(float %op93)
  %op94 = add i32 %op83, 1
  br label %label82
}
