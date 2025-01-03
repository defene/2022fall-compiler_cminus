; ModuleID = 'cminus'
source_filename = "num_comp1.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  call void @outputFloat(float 0xc0465c2900000000)
  ret void
}
