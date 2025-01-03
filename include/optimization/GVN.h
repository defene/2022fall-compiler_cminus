#pragma once
#include "BasicBlock.h"
#include "Constant.h"
#include "DeadCode.h"
#include "FuncInfo.h"
#include "Function.h"
#include "Instruction.h"
#include "Module.h"
#include "PassManager.hpp"
#include "Value.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace GVNExpression {

// fold the constant value
class ConstFolder {
  public:
    ConstFolder(Module *m) : module_(m) {}
    Constant *compute(Instruction *instr, Constant *value1, Constant *value2);
    Constant *compute(Instruction *instr, Constant *value1);

  private:
    Module *module_;
};

/**
 * for constructor of class derived from `Expression`, we make it public
 * because `std::make_shared` needs the constructor to be publicly available,
 * but you should call the static factory method `create` instead the constructor itself to get the desired data
 */
class Expression {
  public:
    // TODO: you need to extend expression types according to testcases
    enum gvn_expr_t { e_constant, e_bin, e_phi, e_func, e_val, e_load, e_global, e_gep, e_arg, e_alloca, e_single, e_cmp, e_fcmp, e_valf};
    Expression(gvn_expr_t t) : expr_type(t) {}
    virtual ~Expression() = default;
    virtual std::string print() = 0;
    gvn_expr_t get_expr_type() const { return expr_type; }

  private:
    gvn_expr_t expr_type;
};

bool operator==(const std::shared_ptr<Expression> &lhs, const std::shared_ptr<Expression> &rhs);
bool operator==(const GVNExpression::Expression &lhs, const GVNExpression::Expression &rhs);

class ConstantExpression : public Expression {
  public:
    static std::shared_ptr<ConstantExpression> create(Constant *c, bool isbool) { return std::make_shared<ConstantExpression>(c, isbool); }
    virtual std::string print() { return c_->print(); }
    // we leverage the fact that constants in lightIR have unique addresses
    bool equiv(const ConstantExpression *other) const { 
      if (c_->get_type()->is_integer_type() && other->c_->get_type()->is_integer_type()) {
        return (c_ == other->c_ && isbool_ == other->isbool_);
      }
      
      return (c_ == other->c_ && c_->get_type() == other->c_->get_type()); }
    Constant *get_lval() const { return c_; }
    ConstantExpression(Constant *c, bool isbool) : Expression(e_constant), c_(c), isbool_(isbool) {}

  private:
    Constant *c_;
    bool isbool_;
};

class FuncExpression : public Expression {
  public:
    static std::shared_ptr<FuncExpression> create(Instruction *ins, Value *val, std::vector<std::shared_ptr<Expression>> args, bool is_pure) { return std::make_shared<FuncExpression>(ins, val, args, is_pure); }
    virtual std::string print() { return ins_->print(); }
    // we leverage the fact that constants in lightIR have unique addresses
    bool equiv(const FuncExpression *other) const {
      if (!is_pure_) {
        return ins_ == other->ins_;
      }

      if (!this->args_.size() && !other->args_.size()) {
        return val_ == other->val_;
      } else {
        if (this->args_.size() != other->args_.size()) {
          return false;
        }
        
        auto it1 = this->args_.cbegin();
        auto it2 = other->args_.cbegin();
        while (it1 != this->args_.cend() && it2 != other->args_.cend()) {
            if (!(*it1 == *it2)){
                return false;
            }
            it1++;
            it2++;
        }
        return val_ == other->val_;
      }
    }
    FuncExpression(Instruction *ins, Value *val, std::vector<std::shared_ptr<Expression>> args, bool is_pure) : Expression(e_func), ins_(ins), val_(val), args_(args), is_pure_(is_pure) {}

  private:
    Instruction *ins_;
    Value *val_;
    std::vector<std::shared_ptr<Expression>> args_;
    bool is_pure_;
};

class VarExpression : public Expression {
  public:
    static std::shared_ptr<VarExpression> create(Value *val, std::shared_ptr<Expression> lhs) { return std::make_shared<VarExpression>(val, lhs); }
    virtual std::string print() { return val_->get_name(); }
    // we leverage the fact that constants in lightIR have unique addresses
    bool equiv(const VarExpression *other) const { return val_->get_name() == other->val_->get_name() || lhs_ == other->lhs_; }
    Value *get_lval() const { return val_; }
    std::shared_ptr<Expression> get_expr() const { return lhs_; }
    VarExpression(Value *val, std::shared_ptr<Expression> lhs) : Expression(e_val), val_(val), lhs_(lhs) {}

  private:
    Value *val_;
    std::shared_ptr<Expression> lhs_;

};

class ConstantfExpression : public Expression {
  public:
    static std::shared_ptr<ConstantfExpression> create(Constant *c, bool isbool, BasicBlock *bb) { return std::make_shared<ConstantfExpression>(c, isbool, bb); }
    virtual std::string print() { return "(conf " + c_->get_name() + " " + bb_->print() + ")"; }
    // we leverage the fact that constants in lightIR have unique addresses
    bool equiv(const ConstantfExpression *other) const { 
      if (c_->get_type()->is_integer_type() && other->c_->get_type()->is_integer_type()) {
        return (c_ == other->c_ && isbool_ == other->isbool_ && bb_ == other->bb_);
      }
      
      return (c_ == other->c_ && c_->get_type() == other->c_->get_type() && bb_ == other->bb_); }
    Constant *get_lval() const { return c_; }
    BasicBlock *get_bb() const { return bb_;};
    ConstantfExpression(Constant *c, bool isbool, BasicBlock *bb) : Expression(e_valf), c_(c), isbool_(isbool), bb_(bb) {}

  private:
    Constant *c_;
    bool isbool_;
    BasicBlock *bb_;
};

class GlobalExpression : public Expression {
  public:
    static std::shared_ptr<GlobalExpression> create(GlobalVariable *val) { return std::make_shared<GlobalExpression>(val); }
    virtual std::string print() { return val_->print(); }
    // we leverage the fact that constants in lightIR have unique addresses
    bool equiv(const GlobalExpression *other) const { return val_ == other->val_; }
    GlobalVariable *get_lval() const { return val_; }
    GlobalExpression(GlobalVariable *val) : Expression(e_global), val_(val) {}

  private:
    GlobalVariable *val_;
};

class ArgExpression : public Expression {
  public:
    static std::shared_ptr<ArgExpression> create(Argument *val) { return std::make_shared<ArgExpression>(val); }
    virtual std::string print() { return val_->print(); }
    // we leverage the fact that constants in lightIR have unique addresses
    bool equiv(const ArgExpression *other) const { return val_ == other->val_; }
    Argument *get_lval() const { return val_; }
    ArgExpression(Argument *val) : Expression(e_arg), val_(val) {}

  private:
    Argument *val_;
};

class GepExpression : public Expression {
  public:
    static std::shared_ptr<GepExpression> create(std::shared_ptr<Expression> ptr, std::shared_ptr<Expression> idx1, std::shared_ptr<Expression> idx2) { return std::make_shared<GepExpression>(ptr, idx1, idx2); }
    virtual std::string print() { 
      if (idx2_) {
        return "(gep " + ptr_->print() + " " + idx1_->print() + " " + idx2_->print() + ")"; 
      } else {
        return "(gep " + ptr_->print() + " " + idx1_->print() +  ")";
      }  
    }
      
    // we leverage the fact that constants in lightIR have unique addresses
    bool equiv(const GepExpression *other) const {
      if (!idx2_ && !other->idx2_) {
        return *ptr_ == *other->ptr_ && *idx1_ == *other->idx1_;
      }
      if (!idx2_ || !other->idx2_) {
        return false;
      }
      
      return *ptr_ == *other->ptr_ && *idx1_ == *other->idx1_ && *idx2_ == *other->idx2_;
    }
    GepExpression(std::shared_ptr<Expression> ptr, std::shared_ptr<Expression> idx1, std::shared_ptr<Expression> idx2) : Expression(e_gep), ptr_(ptr), idx1_(idx1), idx2_(idx2) {}

  private:
    std::shared_ptr<Expression> ptr_;
    std::shared_ptr<Expression> idx1_;
    std::shared_ptr<Expression> idx2_;
};

class LoadExpression : public Expression {
  public:
    static std::shared_ptr<LoadExpression> create(Value *val, std::shared_ptr<Expression> lhs) { return std::make_shared<LoadExpression>(val, lhs); }
    virtual std::string print() { return lhs_->print(); }
    // we leverage the fact that constants in lightIR have unique addresses
    bool equiv(const LoadExpression *other) const { return *lhs_ == *other->lhs_ && val_ == other->val_; }
    LoadExpression(Value *val, std::shared_ptr<Expression> lhs) : Expression(e_load), val_(val), lhs_(lhs) {}

  private:
    Value *val_;
    std::shared_ptr<Expression> lhs_;
};

class SingleExpression : public Expression {
  public:
    static std::shared_ptr<SingleExpression> create(std::shared_ptr<Expression> lhs) { return std::make_shared<SingleExpression>(lhs); }
    virtual std::string print() { return lhs_->print(); }
    // we leverage the fact that constants in lightIR have unique addresses
    bool equiv(const SingleExpression *other) const { return *lhs_ == *other->lhs_; }
    SingleExpression(std::shared_ptr<Expression> lhs) : Expression(e_single), lhs_(lhs) {}

  private:
    std::shared_ptr<Expression> lhs_;
};

class AlloExpression : public Expression {
  public:
    static std::shared_ptr<AlloExpression> create(Value *val) { return std::make_shared<AlloExpression>(val); }
    virtual std::string print() { return val_->print(); }
    // we leverage the fact that constants in lightIR have unique addresses
    bool equiv(const AlloExpression *other) const { return val_ == other->val_; }
    AlloExpression(Value *val) : Expression(e_alloca), val_(val) {}

  private:
    Value *val_;
};

// arithmetic expression
class BinaryExpression : public Expression {
  public:
    static std::shared_ptr<BinaryExpression> create(Instruction::OpID op,
                                                    std::shared_ptr<Expression> lhs,
                                                    std::shared_ptr<Expression> rhs) {
        return std::make_shared<BinaryExpression>(op, lhs, rhs);
    }
    virtual std::string print() {
        return "(" + Instruction::get_instr_op_name(op_) + " " + lhs_->print() + " " + rhs_->print() + ")";
    }

    bool equiv(const BinaryExpression *other) const {
        if (op_ == other->op_ and *lhs_ == *other->lhs_ and *rhs_ == *other->rhs_)
            return true;
        else
            return false;
    }

    Instruction::OpID get_instr_type() const { return op_; }
    gvn_expr_t get_lexpr_type() const { return lhs_->get_expr_type(); }
    gvn_expr_t get_rexpr_type() const { return rhs_->get_expr_type(); }
    std::shared_ptr<Expression> get_lval() const { return lhs_; }
    std::shared_ptr<Expression> get_rval() const { return rhs_; }

    BinaryExpression(Instruction::OpID op, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : Expression(e_bin), op_(op), lhs_(lhs), rhs_(rhs) {}

  private:
    Instruction::OpID op_;
    std::shared_ptr<Expression> lhs_, rhs_;
};

class CmpExpression : public Expression {
  public:
    static std::shared_ptr<CmpExpression> create(CmpInst::CmpOp cmpop, Instruction::OpID op,
                                                    std::shared_ptr<Expression> lhs,
                                                    std::shared_ptr<Expression> rhs) {
        return std::make_shared<CmpExpression>(cmpop, op, lhs, rhs);
    }
    virtual std::string print() {
        return "(" + Instruction::get_instr_op_name(op_) + " " + lhs_->print() + " " + rhs_->print() + ")";
    }

    bool equiv(const CmpExpression *other) const {
        if (cmpop_ == other->cmpop_ and op_ == other->op_ and *lhs_ == *other->lhs_ and *rhs_ == *other->rhs_)
            return true;
        else
            return false;
    }

    Instruction::OpID get_instr_type() const { return op_; }
    gvn_expr_t get_lexpr_type() const { return lhs_->get_expr_type(); }
    gvn_expr_t get_rexpr_type() const { return rhs_->get_expr_type(); }
    std::shared_ptr<Expression> get_lval() const { return lhs_; }
    std::shared_ptr<Expression> get_rval() const { return rhs_; }

    CmpExpression(CmpInst::CmpOp cmpop, Instruction::OpID op, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : Expression(e_cmp), cmpop_(cmpop), op_(op), lhs_(lhs), rhs_(rhs) {}

  private:
    CmpInst::CmpOp cmpop_;
    Instruction::OpID op_;
    std::shared_ptr<Expression> lhs_, rhs_;
};

class FCmpExpression : public Expression {
  public:
    static std::shared_ptr<FCmpExpression> create(FCmpInst::CmpOp cmpop, Instruction::OpID op,
                                                    std::shared_ptr<Expression> lhs,
                                                    std::shared_ptr<Expression> rhs) {
        return std::make_shared<FCmpExpression>(cmpop, op, lhs, rhs);
    }
    virtual std::string print() {
        return "(" + Instruction::get_instr_op_name(op_) + " " + lhs_->print() + " " + rhs_->print() + ")";
    }

    bool equiv(const FCmpExpression *other) const {
        if (cmpop_ == other->cmpop_ and op_ == other->op_ and *lhs_ == *other->lhs_ and *rhs_ == *other->rhs_)
            return true;
        else
            return false;
    }

    Instruction::OpID get_instr_type() const { return op_; }
    gvn_expr_t get_lexpr_type() const { return lhs_->get_expr_type(); }
    gvn_expr_t get_rexpr_type() const { return rhs_->get_expr_type(); }
    std::shared_ptr<Expression> get_lval() const { return lhs_; }
    std::shared_ptr<Expression> get_rval() const { return rhs_; }

    FCmpExpression(FCmpInst::CmpOp cmpop, Instruction::OpID op, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : Expression(e_fcmp), cmpop_(cmpop), op_(op), lhs_(lhs), rhs_(rhs) {}

  private:
    FCmpInst::CmpOp cmpop_;
    Instruction::OpID op_;
    std::shared_ptr<Expression> lhs_, rhs_;
};

class PhiExpression : public Expression {
  public:
    static std::shared_ptr<PhiExpression> create(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs) {
        return std::make_shared<PhiExpression>(lhs, rhs);
    }
    virtual std::string print() { return "(phi " + lhs_->print() + " " + rhs_->print() + ")"; }
    bool equiv(const PhiExpression *other) const {
        if (*lhs_ == *other->lhs_ and *rhs_ == *other->rhs_)
            return true;
        else
            return false;
    }
    PhiExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : Expression(e_phi), lhs_(lhs), rhs_(rhs) {}
        
    gvn_expr_t get_lexpr_type() const { return lhs_->get_expr_type(); }
    gvn_expr_t get_rexpr_type() const { return rhs_->get_expr_type(); }
    std::shared_ptr<Expression> get_lval() const { return lhs_; }
    std::shared_ptr<Expression> get_rval() const { return rhs_; }
  private:
    std::shared_ptr<Expression> lhs_, rhs_;
};
} // namespace GVNExpression

/**
 * Congruence class in each partitions
 * note: for constant propagation, you might need to add other fields
 * and for load/store redundancy detection, you most certainly need to modify the class
 */
struct CongruenceClass {
    size_t index_;
    // representative of the congruence class, used to replace all the members (except itself) when analysis is done
    Value *leader_;
    // value expression in congruence class
    std::shared_ptr<GVNExpression::Expression> value_expr_;
    // value φ-function is an annotation of the congruence class
    std::shared_ptr<GVNExpression::PhiExpression> value_phi_;
    // equivalent variables in one congruence class
    std::set<Value *> members_;

    CongruenceClass(size_t index) : index_(index), leader_{}, value_expr_{}, value_phi_{}, members_{} {}

    bool operator<(const CongruenceClass &other) const { return this->index_ < other.index_; }
    bool operator==(const CongruenceClass &other) const;
};

namespace std {
template <>
// overload std::less for std::shared_ptr<CongruenceClass>, i.e. how to sort the congruence classes
struct less<std::shared_ptr<CongruenceClass>> {
    bool operator()(const std::shared_ptr<CongruenceClass> &a, const std::shared_ptr<CongruenceClass> &b) const {
        // nullptrs should never appear in partitions, so we just dereference it
        return *a < *b;
    }
};
} // namespace std

class GVN : public Pass {
  public:
    using partitions = std::set<std::shared_ptr<CongruenceClass>>;
    GVN(Module *m, bool dump_json) : Pass(m), dump_json_(dump_json) {}
    // pass start
    void run() override;
    // init for pass metadata;
    void initPerFunction();

    // fill the following functions according to Pseudocode, **you might need to add more arguments**
    void detectEquivalences();
    void copyStatment(Instruction *instr);
    partitions join(const partitions &P1, const partitions &P2);
    std::shared_ptr<CongruenceClass> intersect(std::shared_ptr<CongruenceClass>, std::shared_ptr<CongruenceClass>);
    partitions transferFunction(Instruction *x, partitions pin);
    std::shared_ptr<GVNExpression::PhiExpression> valuePhiFunc(std::shared_ptr<GVNExpression::Expression>,
                                                               const partitions &pin);
    std::shared_ptr<GVNExpression::Expression> valueExpr(Instruction *instr, partitions pin);
    std::shared_ptr<GVNExpression::Expression> getVN(const partitions &pout,
                                                     std::shared_ptr<GVNExpression::Expression> ve);

    // replace cc members with leader
    void replace_cc_members();

    // note: be careful when to use copy constructor or clone
    partitions clone(const partitions &p);

    // create congruence class helper
    std::shared_ptr<CongruenceClass> createCongruenceClass(size_t index = 0) {
        return std::make_shared<CongruenceClass>(index);
    }

  private:
    bool dump_json_;
    std::uint64_t next_value_number_ = 1;
    Function *func_;
    std::map<BasicBlock *, partitions> pin_, pout_;
    std::unique_ptr<FuncInfo> func_info_;
    std::unique_ptr<GVNExpression::ConstFolder> folder_;
    std::unique_ptr<DeadCode> dce_;
};

bool operator==(const GVN::partitions &p1, const GVN::partitions &p2);
