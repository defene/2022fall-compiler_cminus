; ModuleID = 'cminus'
source_filename = "test.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  %op0 = call i32 @input()
  %op1 = call i32 @input()
  %op2 = icmp sgt i32 0, 0
  %op3 = zext i1 %op2 to i32
  %op4 = icmp ne i32 %op3, 0
  br i1 %op4, label %label5, label %label14
label5:                                                ; preds = %label_entry
  %op6 = icmp sgt i32 0, 0
  %op7 = zext i1 %op6 to i32
  %op8 = icmp ne i32 %op7, 0
  br i1 %op8, label %label18, label %label26
label9:                                                ; preds = %label22, %label34
  %op10 = phi i32 [ %op23, %label22 ], [ %op35, %label34 ]
  %op11 = phi i32 [ %op24, %label22 ], [ %op36, %label34 ]
  %op12 = phi i32 [ %op25, %label22 ], [ %op37, %label34 ]
  call void @output(i32 %op10)
  %op13 = add i32 %op12, %op11
  call void @output(i32 %op13)
  ret i32 0
label14:                                                ; preds = %label_entry
  %op15 = icmp sgt i32 0, 0
  %op16 = zext i1 %op15 to i32
  %op17 = icmp ne i32 %op16, 0
  br i1 %op17, label %label30, label %label38
label18:                                                ; preds = %label5
  %op19 = add i32 %op0, %op1
  %op20 = mul i32 %op0, %op1
  %op21 = add i32 %op19, %op20
  br label %label22
label22:                                                ; preds = %label18, %label26
  %op23 = phi i32 [ %op21, %label18 ], [ %op29, %label26 ]
  %op24 = phi i32 [ %op20, %label18 ], [ %op28, %label26 ]
  %op25 = phi i32 [ %op19, %label18 ], [ %op27, %label26 ]
  br label %label9
label26:                                                ; preds = %label5
  %op27 = sub i32 %op0, %op1
  %op28 = mul i32 %op1, %op0
  %op29 = add i32 %op27, %op28
  br label %label22
label30:                                                ; preds = %label14
  %op31 = add i32 %op1, %op0
  %op32 = mul i32 %op1, 10
  %op33 = add i32 %op31, %op32
  br label %label34
label34:                                                ; preds = %label30, %label38
  %op35 = phi i32 [ %op33, %label30 ], [ %op41, %label38 ]
  %op36 = phi i32 [ %op32, %label30 ], [ %op40, %label38 ]
  %op37 = phi i32 [ %op31, %label30 ], [ %op39, %label38 ]
  br label %label9
label38:                                                ; preds = %label14
  %op39 = sub i32 %op1, %op0
  %op40 = mul i32 %op0, 10
  %op41 = add i32 %op39, %op40
  br label %label34
}
