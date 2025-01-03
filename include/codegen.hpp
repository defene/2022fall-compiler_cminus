#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include "Module.h"
#include "logging.hpp"
#include <algorithm> 
#include <iostream>
#include <memory>
#include <sstream>

using std::string;
using std::vector;

struct postion {
    BasicBlock *bb;
    int insnum;
};

struct duChain {
    Value *leader_;
    postion *def_;
    std::set<postion *> use_;
    int regnum;
    int offset;
    int realoffset;
    int end;
    bool is_arg;
    bool is_global;

    duChain() : leader_{}, def_{}, use_{} {}

    bool operator==(const duChain &other) const {return this->leader_ == other.leader_;}
};

class CodeGen {
  public:
    using partitions = std::set<Value *>;
    CodeGen(Module *module) : m(module) {}

    string print() {
        string result;
        for (auto line : output) {
            if (line.find(":") == string::npos and line != "")
                result += "\t"; // 添加缩进
            result += line + "\n";
        }
        return result;
    }

    void run();
    void dfs(BasicBlock *bsb);
    void detectDUchain();
    std::vector<duChain *> clone(const std::vector<duChain *> &p);
    void calcUse(Instruction *instr, BasicBlock *bb);
    void LinearScan();
    void ExpireOldIntervals(duChain *du, bool type); 
    void SpillAtInterval(duChain *du, bool type); 
    void initPerFunction();
    void emitcode();
    void binarygen(Instruction *instr);
    void callgen(Instruction *instr);
    void cmpgen(Instruction *instr, Instruction *binstr);

  private:
    int maxreg, maxfreg;
    long unsigned int regsize_ = 8;
    bool fregisters[8];
    int registers[8]; 
    Module *m;
    Function *func_;
    std::list<BasicBlock *> dfs_basic_blocks_;
    std::vector<duChain *> duChain_list_, active_list_, factive_list_;
    std::vector<ConstantFP *> fcon_list_;
    vector<string> output;
    int insnum_, offset_, flabel_;
};

#endif
