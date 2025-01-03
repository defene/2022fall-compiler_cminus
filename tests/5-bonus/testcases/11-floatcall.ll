; ModuleID = 'cminus'
source_filename = "11-floatcall.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define float @mod(float %arg0, float %arg1) {
label_entry:
  %op2 = fdiv float %arg0, %arg1
  %op3 = fptosi float %op2 to i32
  %op4 = sitofp i32 %op3 to float
  %op5 = fmul float %op4, %arg1
  %op6 = fsub float %arg0, %op5
  ret float %op6
}
define i32 @main() {
label_entry:
  %op0 = call float @mod(float 0x4026666660000000, float 0x40019999a0000000)
  call void @outputFloat(float %op0)
  ret i32 0
}
