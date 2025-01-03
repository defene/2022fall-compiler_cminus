#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#ifdef DEBUG                                             // 用于调试信息,大家可以在编译过程中通过" -DDEBUG"来开启这一选项
#define DEBUG_OUTPUT std::cout << __LINE__ << std::endl; // 输出行号的简单示例
#else
#define DEBUG_OUTPUT
#endif

#define CONST_INT(num) ConstantInt::get(num, module)

#define CONST_FP(num) ConstantFP::get(num, module)

int main()
{
    auto module = new Module("Cminus code");
    auto builder = new IRBuilder(nullptr, module);
    Type *Int32Type = Type::get_int32_type(module);

    // main function
    auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                    "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);

    auto retAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(0), retAlloca); // assign ret = 0

    auto aAlloca = builder->create_alloca(Int32Type);
    auto iAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(10), aAlloca);
    builder->create_store(CONST_INT(0), iAlloca); // i = 0, a = 10

    auto condBB = BasicBlock::create(module, "cond", mainFun);
    auto loopBB = BasicBlock::create(module, "loop", mainFun);
    auto retBB = BasicBlock::create(module, "", mainFun);
    builder->create_br(condBB); // while branch

    // condBB
    builder->set_insert_point(condBB);
    auto iLoad = builder->create_load(iAlloca);
    auto icmp = builder->create_icmp_lt(iLoad, CONST_INT(10));
    builder->create_cond_br(icmp, loopBB, retBB);

    // loopBB
    builder->set_insert_point(loopBB);
    iLoad = builder->create_load(iAlloca);
    auto add = builder->create_iadd(iLoad, CONST_INT(1));
    builder->create_store(add, iAlloca); // i = i + 1
    
    auto aLoad = builder->create_load(aAlloca);
    iLoad = builder->create_load(iAlloca);
    add = builder->create_iadd(aLoad, iLoad);
    builder->create_store(add, aAlloca); // a = a + i
    builder->create_br(condBB);

    // retBB
    builder->set_insert_point(retBB);
    aLoad = builder->create_load(aAlloca);
    builder->create_store(aLoad, retAlloca);
    auto retLoad = builder->create_load(retAlloca);
    builder->create_ret(retLoad);

    std::cout << module->print();
    delete module;
    return 0;
}