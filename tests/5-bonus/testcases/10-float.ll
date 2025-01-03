; ModuleID = 'cminus'
source_filename = "10-float.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  call void @outputFloat(float 0x4006cccce0000000)
  ret i32 0
}
