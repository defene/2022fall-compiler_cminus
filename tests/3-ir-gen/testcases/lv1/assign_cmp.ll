; ModuleID = 'cminus'
source_filename = "assign_cmp.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  call void @output(i32 1)
  call void @output(i32 0)
  call void @output(i32 0)
  ret void
}
