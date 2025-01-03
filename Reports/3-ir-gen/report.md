# lab3 实验报告
学号 姓名

## 实验要求
## 实验难点

实验中遇到哪些挑战

## 实验设计

请写明为了顺利完成本次实验，加入了哪些亮点设计，并对这些设计进行解释。
可能的阐述方向有:

1. 如何设计全局变量
2. 遇到的难点以及解决方案
3. 如何降低生成 IR 中的冗余
4. ...

### 实验总结

此次实验有什么收获

### 实验反馈 （可选 不计入评分）

对本次实验的建议
# lab3 实验报告

### 学号：PB20000024
### 姓名：陈奕衡

## 实验要求

1. 阅读[cminus-f 的语义规则](../common/cminusf.md)，我们将按照语义实现程度进行评分
2. 阅读[LightIR 核心类介绍](../common/LightIR.md)
3. 阅读[实验框架](#1-实验框架)，理解如何使用框架以及注意事项
4. 修改 `src/cminusfc/cminusf_builder.cpp` 来实现 IR 自动产生的算法，使得它能正确编译任何合法的 cminus-f 程序。
5. 在 `report.md` 中解释你的设计，遇到的困难和解决方案

总而言之，本次实验主要工作就是实现lab2中的cpp代码的自动生成

## 实验难点

### 全局变量

本次实验中最难的点就是对于自己设计的全局变量的运用，这基本上是几乎所有bug的根本原因。

比如在`ASTReturnStmt`中，如果将局部变量`ret_val`的取值放在`expression`分析的前面，那么就会取出一个完全错误的取值，导致段错误（当然这一点适用于每一个`expression`的分析）

反例：
```cpp
Type *ret_type;
Value *ret_val = val;
node.expression->accept(*this);
```

正解：
```cpp
Type *ret_type;
node.expression->accept(*this);
Value *ret_val = val;
```

之后还有对于param节点的读取，由于我把`param_types`设置成了全局变量，那么这样的话每次调用`ASTFunDeclaration`都需要进行全局变量的清空，否则参数类型也会乱了

```cpp
while (!param_types.empty()) {
    param_types.pop_back();
}
for (auto &param : node.params) {
    //!TODO: Please accomplish param_types.
    param->accept(*this);
}
```

最后还有一个十分致命的问题，就是在`Var`取左值并且左值是形如`ID1[ID2]`的时候，如果不及时更新全局变量`left`，就会在最后连续取地址而不是取`ID2`的值，这里就需要在`ASTVar`中添加一个局部变量`local_left`来代替全局变量`left`，并且及时将`left`赋值为0

```cpp
Value *var = scope.find(node.id);
bool local_left = left;
left = false;
```

### 基本块嵌套

这种嵌套问题出现在`if`或者`while`的式子中，当`while`或者`if`的中间设置的那个块`loopBB`、`trueBB`、`falseBB`中出现数组操作时，会出现如下的操作块

```
loopBB、trueBB、falseBB:
    ...
    br negBB or posBB
retBB:
    ...
negBB:    
    ...		
posBB:			
    ... 			
    br loopBB、trueBB、falseBB
```

也就是说这样的话`retBB`后面会直接进入`negBB`，导致出错。为防止这种情况发生，需要在`retBB`后再开一个块，实现对于`negBB`和`posBB`的跳过。并且在`ASTVar`中将`negBB`放为中间块（`negBB`不存在任何对于数组的操作），同样是防止上述情况。

```
loopBB、trueBB、falseBB:
    ...
    br negBB or posBB
retBB:
    ...
    br newBB
negBB:    
    ...		
posBB:			
    ... 			
    br loopBB、trueBB、falseBB
newBB:
    ...
```

最后还想说的一点就是不要给基本块使用常量命名，否则一嵌套跳转整个就乱了

```cpp
auto condBB = BasicBlock::create(module.get(), "", cur_fun);
auto loopBB = BasicBlock::create(module.get(), "", cur_fun);
auto retBB = BasicBlock::create(module.get(), "", cur_fun);
```

### 文件阅读

这里找了最久的一个东西就是在`ASTCall`函数中，对于从`scope`中读出的函数做强制类型转换。一直找到`Function.cpp`才找到相应的做法

```cpp
auto fun = static_cast<Function *>(scope.find(node.id));
```

## 实验设计

请写明为了顺利完成本次实验，加入了哪些亮点设计，并对这些设计进行解释。
可能的阐述方向有:

1. 如何设计全局变量
2. 遇到的难点以及解决方案
3. 如何降低生成 IR 中的冗余
4. ...

### 全局变量

这里想到的就是需要在函数间传递的值或者信号，首当其冲需要一个取值`val`和一个地址`var`，以及一个`cur_fun`。之后在编写过程中发现，对于函数参数，还需要一个`param_types`传回函数体以构造函数，一个信号变量`left`判断取值的时候取地址还是取地址位置的值

```cpp
Function *cur_fun = nullptr;
Value *val = nullptr;
std::vector<Type *> param_types;
Value *var_val = nullptr;
bool left = false;
bool array = false;
```

### 模块介绍

#### ASTNum

此节点需要取值，相当于为全局变量`val`赋值的操作

```cpp
if (node.type == TYPE_INT) { // INTEGER
    val = CONST_INT(node.i_val);
} else { // FLOAT
    val = CONST_FP(node.f_val);
}
```

#### ASTVarDeclaration

此节点用于处理所有数值声明（全局、局部），首先确定该声明的数值类型

```cpp
Type *var_type;
if (node.type == TYPE_INT) {
    var_type = INT32_T;
} else {
    var_type = FLOAT_T;
}
```

之后再去确定其他性质并进行相应处理，最后记得将`node.id`压入`scope`的作用域中。注意需要根据`scope.in_global()`的取值判断是否生成的是全局变量

```cpp
if (scope.in_global()) { // global variables
    auto initializer = ConstantZero::get(var_type, module.get());
    if (node.num == nullptr) {
        auto var_global = GlobalVariable::create(node.id, module.get(), var_type, false, initializer);
        scope.push(node.id, var_global);
    } else {
        auto *arrayType = ArrayType::get(var_type, node.num->i_val);
        auto array_global = GlobalVariable::create(node.id, module.get(), arrayType, false, initializer);
        scope.push(node.id, array_global);
    }
} else { // local variables
    if (node.num == nullptr) {
        auto var_local = builder->create_alloca(var_type);
        scope.push(node.id, var_local);
    } else {
        auto *arrayType = ArrayType::get(var_type, node.num->i_val);
        auto array_local = builder->create_alloca(arrayType);
        scope.push(node.id, array_local);
    }
}
```

#### ASTFunDeclaration

此节点用于处理所有函数声明，与数值声明的处理相同，首先要获取该函数的返回值类型和所有参数类型。在遍历参数时使用了全局变量`param_types`以保存所有的参数类型

```cpp
FunctionType *fun_type;
Type *ret_type;
if (node.type == TYPE_INT) {
    ret_type = INT32_T;
} else if (node.type == TYPE_FLOAT) {
    ret_type = FLOAT_T;
} else {
    ret_type = VOID_T;
}

while (!param_types.empty()) {
    param_types.pop_back();
}
for (auto &param : node.params) {
        //!TODO: Please accomplish param_types.
    param->accept(*this);
}

fun_type = FunctionType::get(ret_type, param_types);
```

之后进行函数的创建，注意进入函数块之后需要进入一个新的作用域，否则对于形参信息的存储会影响到此作用域。之后就是对函数体的遍历了

```cpp
auto fun = Function::create(fun_type, node.id, module.get());
scope.push(node.id, fun);
cur_fun = fun;
auto funBB = BasicBlock::create(module.get(), "", fun);
builder->set_insert_point(funBB);
scope.enter();
std::vector<Value *> args;
for (auto arg = fun->arg_begin(); arg != fun->arg_end(); arg++) {
    args.push_back(*arg);
}
for (long unsigned int i = 0; i < node.params.size(); ++i) {
    Value *param_alloca;
    if (node.params[i]->isarray) {
        if (node.params[i]->type == TYPE_INT) {
            param_alloca = builder->create_alloca(INT32PTR_T);
        } else {
            param_alloca = builder->create_alloca(FLOATPTR_T);
        }
    } else {
        if (node.params[i]->type == TYPE_INT) {
            param_alloca = builder->create_alloca(INT32_T);
        } else {
            param_alloca = builder->create_alloca(FLOAT_T);
        }
    }
    builder->create_store(args[i], param_alloca);
    scope.push(node.params[i]->id, param_alloca);
}
node.compound_stmt->accept(*this);
```

在函数结束之后以防末尾没有`return`语句，这里手动添加`return`的结果。最后退出函数作用域

```cpp
if (builder->get_insert_block()->get_terminator() == nullptr) {
    if (cur_fun->get_return_type()->is_void_type()) {
        builder->create_void_ret();
    } else if (cur_fun->get_return_type()->is_float_type()) {
         builder->create_ret(CONST_FP(0.));
    } else {
        builder->create_ret(CONST_INT(0));
    }
}
scope.exit();
```

#### ASTParam

这里就是把所有参数的类型都确定下来，存在全局变量`param_types`中

```cpp
if (node.type == TYPE_INT) {
    if (node.isarray) {
        param_types.push_back(INT32PTR_T);
    } else {
        param_types.push_back(INT32_T);
    }
} else {
    if (node.isarray) {
        param_types.push_back(FLOATPTR_T);
    } else {
        param_types.push_back(FLOAT_T);
    }
}
```

#### ASTCompoundStmt

这里设计的有点暴力，就是为每一个`CompoundStmt`都分配一个新的作用域并且在结束时退出

```cpp
scope.enter();
for (auto &decl : node.local_declarations) {
    decl->accept(*this);
}

for (auto &stmt : node.statement_list) {
    stmt->accept(*this);
    if (builder->get_insert_block()->get_terminator() != nullptr) {
        break;
    }
}
scope.exit();
```

#### ASTExpressionStmt

根据语法规则，这里主要用来判断`expression`存在与否

```cpp
if (node.expression != nullptr) {
        node.expression->accept(*this);
    }
```

#### ASTSelectionStmt

首先计算判断条件，并进行相应的判断

```cpp
node.expression->accept(*this);
Value *cond_val = val;

if (cond_val->get_type()->is_integer_type()) {
    cond_val = builder->create_icmp_gt(cond_val, CONST_INT(0));
} else {
    cond_val = builder->create_fcmp_gt(cond_val, CONST_FP(0.));
}
```

根据`else`模块的有无，进行不同块的创建，并且建立跳转

```cpp
auto trueBB = BasicBlock::create(module.get(), "", cur_fun);
auto retBB = BasicBlock::create(module.get(), "", cur_fun);

if (node.else_statement == nullptr) {
    builder->create_cond_br(cond_val, trueBB, retBB);

    builder->set_insert_point(trueBB);
    node.if_statement->accept(*this);
    if (builder->get_insert_block()->get_terminator() == nullptr) {
            builder->create_br(retBB);
    }
} else {
    auto falseBB = BasicBlock::create(module.get(), "", cur_fun);
    builder->create_cond_br(cond_val, trueBB, falseBB);

    builder->set_insert_point(trueBB);
    node.if_statement->accept(*this);
    if (builder->get_insert_block()->get_terminator() == nullptr) {
        builder->create_br(retBB);
    }

    builder->set_insert_point(falseBB);
    node.else_statement->accept(*this);
    if (builder->get_insert_block()->get_terminator() == nullptr) {
        builder->create_br(retBB);
    }
}
builder->set_insert_point(retBB);
```

最后还要记得对于数组情况的特殊处理

```cpp
if (array) {
    auto newBB = BasicBlock::create(module.get(), "", cur_fun);
    if (builder->get_insert_block()->get_terminator() == nullptr) {
        builder->create_br(newBB);
    }
    builder->set_insert_point(newBB);
    array = false;
}
```

#### ASTIterationStmt

与上面的情况类似，不多加赘述

#### ASTReturnStmt

这里与`ASTFunDeclaration`时对于返回值的处理基本相同，不同的点是这里需要获取后面`expression`的值

```cpp
Type *ret_type;
node.expression->accept(*this);
Value *ret_val = val;
ret_type = cur_fun->get_return_type();
```

#### ASTVar

首先是对于全局变量的处理

```cpp
Value *var = scope.find(node.id);
bool local_left = left;
left = false;
```

之后是对于变量的处理

```cpp
if (local_left) {
    var_val = var;
} else {
    if (var->get_type()->get_pointer_element_type()->is_float_type() ||
        var->get_type()->get_pointer_element_type()->is_integer_type() ||
        var->get_type()->get_pointer_element_type()->is_pointer_type()) {
        val = builder->create_load(var);
    } else {
        val = builder->create_gep(var, {CONST_INT(0), CONST_INT(0)});
    }
}
```

后面是对于数组的处理。这里需要考虑到数组越界的情况，即`expression`的值为负。因此还是需要建立不同的块进行处理，这里重点说一下关于越界的处理

```cpp
builder->set_insert_point(negBB);
auto neg_idx_except_fun = scope.find("neg_idx_except");
builder->create_call(static_cast<Function *>(neg_idx_except_fun), {});
if (cur_fun->get_return_type()->is_void_type()) {
    builder->create_void_ret();
} else if (cur_fun->get_return_type()->is_float_type()) {
    builder->create_ret(CONST_FP(0.));
} else {
    builder->create_ret(CONST_INT(0));
}
```

越界处理时直接调用`neg_idx_except`函数，并且要注意进行`return`处理，类似于`ASTFunDeclaration`中对于没有`return`语句的处理

#### ASTAssignExpression

同样是要注意对于`left`的处理，并且注意`var`是要取回地址的

```cpp
node.expression->accept(*this);
Value *assign_val = val;
left = true;
node.var->accept(*this);
Type *var_type;
```

之后就是一个平常的赋值问题（省略展示类型转换，因为这里的类型转换基本一致）。注意最后要将`val`赋值，代表该表达式的计算结果

```cpp
builder->create_store(assign_val, var_val);
val = assign_val;
```

#### ASTSimpleExpression

先判断右式是否存在（不存在就代表没有运算）。若存在进入运算阶段，即取得运算符号两边的数值，进行类型转换，然后进行计算并储存结果。

```cpp
node.additive_expression_l->accept(*this);
Value *l_val = val;
node.additive_expression_r->accept(*this);
Value *r_val = val;
//sitofp and fptosi
Value *result;
switch (node.op) {
case OP_LT:
    if (l_val->get_type()->is_integer_type()) {
        result = builder->create_icmp_lt(l_val, r_val);
    } else {
        result = builder->create_fcmp_lt(l_val, r_val);
    }
    break;
//...
}
val = builder->create_zext(result, INT32_T);
```

#### ASTAdditiveExpression

与上面的情况类似，不多加赘述

#### ASTTerm

与上面的情况类似，不多加赘述

#### ASTCall

首先找到函数，之后获取参数，使形参和实参一一对应，最后进入函数体

```cpp
auto fun = static_cast<Function *>(scope.find(node.id));
std::vector<Value *> args;
auto param_type = fun->get_function_type()->param_begin();
for (auto &arg : node.args) {
    arg->accept(*this);
    //sitofp and fptosi
    args.push_back(val);
    param_type++;
}

val = builder->create_call(fun, args);
```

### 冗余处理

这里主要做了两点处理。首先是`left`做到的冗余处理，取地址能够节省一句load指令

```cpp
if (local_left) {
    var_val = ptr;
} else {
    val = builder->create_load(ptr);
}
```

之后是`array`做到的冗余处理，能够对基本块中不存在数组访问的时候删除`newBB`的创建

```cpp
if (array) {
    auto newBB = BasicBlock::create(module.get(), "", cur_fun);
    if (builder->get_insert_block()->get_terminator() == nullptr) {
        builder->create_br(newBB);
    }
    builder->set_insert_point(newBB);
    array = false;
}
```

### 实验总结

本次实验让我更深入了解了高级语言的语法结构，以及对应语法树生成中间代码的过程。同时锻炼了我对于全局变量以及c++类的使用，收获很大。

### 实验反馈 （可选 不计入评分）

还是不会用log，希望助教可以在示例代码中体现一下。
