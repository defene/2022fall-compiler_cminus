source_filename = "while.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define dso_local i32 @main() #0 {
    %1 = alloca i32, align 4 ; int a
    %2 = alloca i32, align 4 ; int i
    store i32 10, i32* %1, align 4 ; a = 10
    store i32 0, i32* %2, align 4 ; i = 0
    br label %3
3:
    %4 = load i32, i32* %2, align 4
    %5 = icmp slt i32 %4, 10
    br i1 %5, label %8, label %6 ; cond i < 10
6:
    %7 = load i32, i32* %1, align 4
    ret i32 %7
8:
    %9 = load i32, i32* %2, align 4
    %10 = add i32 %9, 1
    store i32 %10, i32* %2, align 4 ; i = i + 1
    
    %11 = load i32, i32* %1, align 4
    %12 = add i32 %11, %10
    store i32 %12, i32* %1, align 4 ; a = a + i
    br label %3
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.1 "}