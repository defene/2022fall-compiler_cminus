; ModuleID = 'cminus'
source_filename = "num_add_mixed.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  call void @output(i32 1023)
  ret void
}
