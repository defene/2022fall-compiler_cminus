; ModuleID = 'cminus'
source_filename = "input.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  %op0 = call i32 @input()
  ret void
}
