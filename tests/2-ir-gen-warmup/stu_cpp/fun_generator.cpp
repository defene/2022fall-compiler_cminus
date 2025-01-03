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

  // callee function
  std::vector<Type *> Ints(1, Int32Type);
  auto calleeFunTy = FunctionType::get(Int32Type, Ints);
  auto calleeFun = Function::create(calleeFunTy,
                                 "callee", module);
  auto bb = BasicBlock::create(module, "entry", calleeFun); 
  builder->set_insert_point(bb);
  
  auto aAlloca = builder->create_alloca(Int32Type);   
  std::vector<Value *> args;  
  for (auto arg = calleeFun->arg_begin(); arg != calleeFun->arg_end(); arg++) {
    args.push_back(*arg);   
  }
  builder->create_store(args[0], aAlloca);  // handle a
  
  auto retAlloca = builder->create_alloca(Int32Type);
  builder->create_store(CONST_INT(0), retAlloca); // assign ret = 0     
  
  auto aLoad = builder->create_load(aAlloca);
  auto mul = builder->create_imul(aLoad, CONST_INT(2)); // a * 2

  builder->create_store(mul, retAlloca);
  auto retLoad = builder->create_load(retAlloca);
  builder->create_ret(retLoad); // return a * 2
  
  // main function
  auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                  "main", module);
  bb = BasicBlock::create(module, "entry", mainFun);
  builder->set_insert_point(bb);

  retAlloca = builder->create_alloca(Int32Type);
  builder->create_store(CONST_INT(0), retAlloca);  // assign ret = 0    
  
  auto call = builder->create_call(calleeFun, {CONST_INT(110)});  
  builder->create_ret(call); //return callee(110)
  
  std::cout << module->print();
  delete module;
  return 0;
}