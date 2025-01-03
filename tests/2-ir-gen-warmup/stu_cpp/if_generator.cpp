#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#ifdef DEBUG // 用于调试信息,大家可以在编译过程中通过" -DDEBUG"来开启这一选项
#define DEBUG_OUTPUT std::cout << __LINE__ << std::endl; // 输出行号的简单示例
#else
#define DEBUG_OUTPUT
#endif

#define CONST_INT(num) ConstantInt::get(num, module)

#define CONST_FP(num) ConstantFP::get(num, module) 

int main() {
  auto module = new Module("Cminus code"); 
  auto builder = new IRBuilder(nullptr, module);
  Type *Int32Type = Type::get_int32_type(module);
  Type *Float32Type = Type::get_float_type(module);

  // main function
  auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                  "main", module);
  auto bb = BasicBlock::create(module, "entry", mainFun);
  builder->set_insert_point(bb);

  auto retAlloca = builder->create_alloca(Int32Type);
  builder->create_store(CONST_INT(0), retAlloca); // assign ret = 0      
  
  auto aAlloca = builder->create_alloca(Float32Type);
  builder->create_store(CONST_FP(5.555), aAlloca); // get a = 5.555

  auto aLoad = builder->create_load(aAlloca);
  auto fcmp = builder->create_fcmp_gt(aLoad, CONST_FP(1));

  auto trueBB = BasicBlock::create(module, "trueBB", mainFun);    // true br
  auto falseBB = BasicBlock::create(module, "falseBB", mainFun);  // false br
  auto retBB = BasicBlock::create(
      module, "", mainFun);
  builder->create_cond_br(fcmp, trueBB, falseBB); // branch

  builder->set_insert_point(trueBB);  // if true
  builder->create_store(CONST_INT(233), retAlloca);
  builder->create_br(retBB);  // br retBB

  builder->set_insert_point(falseBB);  // if false
  builder->create_br(retBB);  // br retBB

  builder->set_insert_point(retBB);  // ret
  auto retLoad = builder->create_load(retAlloca);
  builder->create_ret(retLoad);
  
  std::cout << module->print();
  delete module;
  return 0;
}