#include "GVN.h"

#include "BasicBlock.h"
#include "Constant.h"
#include "DeadCode.h"
#include "FuncInfo.h"
#include "Function.h"
#include "Instruction.h"
#include "logging.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <memory>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

using namespace GVNExpression;
using std::string_literals::operator""s;
using std::shared_ptr;

static auto get_const_int_value = [](Value *v) { return dynamic_cast<ConstantInt *>(v)->get_value(); };
static auto get_const_fp_value = [](Value *v) { return dynamic_cast<ConstantFP *>(v)->get_value(); };
// Constant Propagation helper, folders are done for you
Constant *ConstFolder::compute(Instruction *instr, Constant *value1, Constant *value2) {
    auto op = instr->get_instr_type();
    switch (op) {
    case Instruction::add: return ConstantInt::get(get_const_int_value(value1) + get_const_int_value(value2), module_);
    case Instruction::sub: return ConstantInt::get(get_const_int_value(value1) - get_const_int_value(value2), module_);
    case Instruction::mul: return ConstantInt::get(get_const_int_value(value1) * get_const_int_value(value2), module_);
    case Instruction::sdiv: return ConstantInt::get(get_const_int_value(value1) / get_const_int_value(value2), module_);
    case Instruction::fadd: return ConstantFP::get(get_const_fp_value(value1) + get_const_fp_value(value2), module_);
    case Instruction::fsub: return ConstantFP::get(get_const_fp_value(value1) - get_const_fp_value(value2), module_);
    case Instruction::fmul: return ConstantFP::get(get_const_fp_value(value1) * get_const_fp_value(value2), module_);
    case Instruction::fdiv: return ConstantFP::get(get_const_fp_value(value1) / get_const_fp_value(value2), module_);

    case Instruction::cmp:
        switch (dynamic_cast<CmpInst *>(instr)->get_cmp_op()) {
        case CmpInst::EQ: return ConstantInt::get(get_const_int_value(value1) == get_const_int_value(value2), module_);
        case CmpInst::NE: return ConstantInt::get(get_const_int_value(value1) != get_const_int_value(value2), module_);
        case CmpInst::GT: return ConstantInt::get(get_const_int_value(value1) > get_const_int_value(value2), module_);
        case CmpInst::GE: return ConstantInt::get(get_const_int_value(value1) >= get_const_int_value(value2), module_);
        case CmpInst::LT: return ConstantInt::get(get_const_int_value(value1) < get_const_int_value(value2), module_);
        case CmpInst::LE: return ConstantInt::get(get_const_int_value(value1) <= get_const_int_value(value2), module_);
        }
    case Instruction::fcmp:
        switch (dynamic_cast<FCmpInst *>(instr)->get_cmp_op()) {
        case FCmpInst::EQ: return ConstantInt::get(get_const_fp_value(value1) == get_const_fp_value(value2), module_);
        case FCmpInst::NE: return ConstantInt::get(get_const_fp_value(value1) != get_const_fp_value(value2), module_);
        case FCmpInst::GT: return ConstantInt::get(get_const_fp_value(value1) > get_const_fp_value(value2), module_);
        case FCmpInst::GE: return ConstantInt::get(get_const_fp_value(value1) >= get_const_fp_value(value2), module_);
        case FCmpInst::LT: return ConstantInt::get(get_const_fp_value(value1) < get_const_fp_value(value2), module_);
        case FCmpInst::LE: return ConstantInt::get(get_const_fp_value(value1) <= get_const_fp_value(value2), module_);
        }
    default: return nullptr;
    }
}

Constant *ConstFolder::compute(Instruction *instr, Constant *value1) {
    auto op = instr->get_instr_type();
    switch (op) {
    case Instruction::sitofp: return ConstantFP::get((float)get_const_int_value(value1), module_);
    case Instruction::fptosi: return ConstantInt::get((int)get_const_fp_value(value1), module_);
    case Instruction::zext: return ConstantInt::get((int)get_const_int_value(value1), module_);
    default: return nullptr;
    }
}

namespace utils {
static std::string print_congruence_class(const CongruenceClass &cc) {
    std::stringstream ss;
    if (cc.index_ == 0) {
        ss << "top class\n";
        return ss.str();
    }
    ss << "\nindex: " << cc.index_ << "\nleader: " << cc.leader_->print()
       << "\nvalue phi: " << (cc.value_phi_ ? cc.value_phi_->print() : "nullptr"s)
       << "\nvalue expr: " << (cc.value_expr_ ? cc.value_expr_->print() : "nullptr"s) << "\nmembers: {";
    for (auto &member : cc.members_)
        ss << member->get_name() << "; ";
    ss << "}\n";
    return ss.str();
}

static std::string dump_cc_json(const CongruenceClass &cc) {
    std::string json;
    json += "[";
    for (auto member : cc.members_) {
        auto c = dynamic_cast<Constant *>(member);
        if (c)
            json += member->print() + ", ";
        else
            json += "\"%" + member->get_name() + "\", ";
    }
    json += "]";
    return json;
}

static std::string dump_partition_json(const GVN::partitions &p) {
    std::string json;
    json += "[";
    for (auto cc : p)
        json += dump_cc_json(*cc) + ", ";
    json += "]";
    return json;
}

static std::string dump_bb2partition(const std::map<BasicBlock *, GVN::partitions> &map) {
    std::string json;
    json += "{";
    for (auto [bb, p] : map)
        json += "\"" + bb->get_name() + "\": " + dump_partition_json(p) + ",";
    json += "}";
    return json;
}

// logging utility for you
static void print_partitions(const GVN::partitions &p) {
    if (p.empty()) {
        LOG_DEBUG << "empty partitions\n";
        return;
    }
    std::string log;
    for (auto &cc : p)
        log += print_congruence_class(*cc);
    LOG_DEBUG << log; // please don't use std::cout
}
} // namespace utils

GVN::partitions GVN::join(const partitions &P1, const partitions &P2) {
    // TODO: do intersection pair-wise
    partitions p = {};
    if (!P1.size()) return p;
    else if (!P2.size()) return p;

    for (auto &cc : P1) {
        if (cc->index_ == 0) return clone(P2);
    }
    for (auto &cc : P2) {
        if (cc->index_ == 0) return clone(P1);
    } 

    if (P1 == P2) return clone(P1);

    for (auto &cc1 : P1) {
        for (auto &cc2 : P2) {            
            auto newCC = GVN::intersect(cc1, cc2);            
            if (newCC) p.insert(newCC);
        }
    }   

    return p;
}

std::shared_ptr<CongruenceClass> GVN::intersect(std::shared_ptr<CongruenceClass> Ci,
                                                std::shared_ptr<CongruenceClass> Cj) {
    // TODO
    auto newC = createCongruenceClass(next_value_number_++);
    std::set_intersection(Ci->members_.cbegin(), Ci->members_.cend(), 
                          Cj->members_.cbegin(), Cj->members_.cend(), 
                          inserter(newC->members_, newC->members_.begin()));

    if (!newC->members_.size()) return {};
    
    if (Ci->leader_ == Cj->leader_) {
        newC->index_ = Ci->index_;
        newC->value_expr_ = Ci->value_expr_;
        newC->leader_ = Ci->leader_;
        newC->value_phi_ = Ci->value_phi_;
        return newC;
    }

    for (auto mem : newC->members_) {
        auto instr = dynamic_cast<Instruction *>(mem);
        if (instr && instr->is_phi()) {
            shared_ptr<Expression> lval, rval, lexp, rexp;
            
            auto conLval = dynamic_cast<Constant *>(instr->get_operand(0));
            if (conLval) {
                lval = ConstantExpression::create(conLval, 0);
                lexp = lval;
            } else {                            
                auto InsLval = instr->get_operand(0);
                for (auto mem : Ci->members_) {
                    if (mem == InsLval) {                   
                        if (Ci->value_expr_->get_expr_type() == Expression::e_constant ||
                            Ci->value_expr_->get_expr_type() == Expression::e_arg ||
                            Ci->value_expr_->get_expr_type() == Expression::e_global) {
                            lval = Ci->value_expr_;
                        } else {
                            lval = VarExpression::create(mem, Ci->value_expr_);
                        }
                        lexp = Ci->value_expr_;
                        break;
                    } 
                }  
            }

            auto conRval = dynamic_cast<Constant *>(instr->get_operand(2));
            if (conRval) {
                rval = ConstantExpression::create(conRval, 0);
                rexp = rval;
            } else {
                auto InsRval = instr->get_operand(2);
                for (auto mem : Cj->members_) {
                    if (mem == InsRval) {                        
                        if (Cj->value_expr_->get_expr_type() == Expression::e_constant ||
                            Cj->value_expr_->get_expr_type() == Expression::e_arg ||
                            Cj->value_expr_->get_expr_type() == Expression::e_global) {
                            rval = Cj->value_expr_;
                        } else {
                            rval = VarExpression::create(mem, Cj->value_expr_);
                        }
                        rexp = Cj->value_expr_;
                        break;
                    }
                }  
            }
            
            if (lval && rval) {
                shared_ptr<Expression> exp;                    
                if (*lval == *rval) exp = lexp;
                else exp = PhiExpression::create(lval, rval);

                newC->value_expr_ = exp;
                newC->leader_ = mem;
                return newC;
            } else {
                newC->value_expr_ = Ci->value_expr_;
                newC->leader_ = Ci->leader_;
                newC->value_phi_ = Ci->value_phi_;
                return newC;
            }
        }  else {
            newC->value_expr_ = Ci->value_expr_;
            newC->leader_ = Ci->leader_;
            newC->value_phi_ = Ci->value_phi_;
            return newC;
        }
    }           

    return {};
}

void GVN::copyStatment(Instruction *instr) {
    auto instrval = dynamic_cast<Value *>(instr);

    auto lval = instr->get_operand(0);
    auto rval = instr->get_operand(2);
    auto bb_prel = dynamic_cast<BasicBlock *>(instr->get_operand(1));
    auto bb_prer = dynamic_cast<BasicBlock *>(instr->get_operand(3));

    for (auto &cc : pout_[bb_prel]) {
        if (cc->index_ == 0) break;
        std::set<Value *>::iterator it;
        if((it = cc->members_.find(instrval)) != cc->members_.end()) {
            cc->members_.erase(it);
            if(!cc->members_.size()) {
                pout_[bb_prel].erase(cc);
                break;
            }
        }
    }

    int lflag = 0;
    auto conLval = dynamic_cast<Constant *>(lval);
    for (auto &cc : pout_[bb_prel]) {
        if (cc->index_ == 0) break;
        if (cc->value_expr_->get_expr_type() == Expression::e_valf) {
            auto lcon = std::dynamic_pointer_cast<ConstantfExpression>(cc->value_expr_);
            auto conval = lcon->get_lval();
            if (conLval == conval && lcon->get_bb() == bb_prel) {
                cc->members_.insert(instrval);
                lflag = 1;
                break;
            }
        }
        for (auto mem : cc->members_) {
            if (mem == lval) {
                cc->members_.insert(instrval);
                lflag = 1;
                break;
            }
        } 
        if (lflag) break;
    }
    
    if (conLval && !lflag) {
        auto lexp = ConstantfExpression::create(conLval, 0, bb_prel);
        auto newC = createCongruenceClass(next_value_number_++);
        newC->value_expr_ = lexp;
        newC->leader_ = instrval;
        newC->members_.insert(instrval);
        pout_[bb_prel].insert(newC);
        for (auto &cc : pout_[bb_prel]) { 
            if (cc->index_ == 0) pout_[bb_prel].erase(cc);
        }
    }

    for (auto &cc : pout_[bb_prer]) {
        if (cc->index_ == 0) break;
        std::set<Value *>::iterator it;
        if((it = cc->members_.find(instrval)) != cc->members_.end()) {
            cc->members_.erase(it);
            if(!cc->members_.size()) {
                pout_[bb_prer].erase(cc);
                break;
            }
        }
    }

    int rflag = 0;   
    auto conRval = dynamic_cast<Constant *>(rval);                
    for (auto &cc : pout_[bb_prer]) {
        if (cc->index_ == 0) break;
        if (cc->value_expr_->get_expr_type() == Expression::e_valf) {
            auto rcon = std::dynamic_pointer_cast<ConstantfExpression>(cc->value_expr_);
            auto conval = rcon->get_lval();
            if (conRval == conval && rcon->get_bb() == bb_prer) {
                cc->members_.insert(instrval);
                rflag = 1;
                break;
            }
        }
        for (auto mem : cc->members_) {
            if (mem == rval) {
                cc->members_.insert(instrval);
                rflag = 1;
                break;
            }
        } 
        if (rflag) break;
    }
    
    if (conRval && !rflag) {
        auto rexp = ConstantfExpression::create(conRval, 0, bb_prer);
        auto newC = createCongruenceClass(next_value_number_++);
        newC->value_expr_ = rexp;
        newC->leader_ = instrval;
        newC->members_.insert(instrval);
        pout_[bb_prer].insert(newC);
        for (auto &cc : pout_[bb_prer]) { 
            if (cc->index_ == 0) pout_[bb_prer].erase(cc);
        }
    }
}

void GVN::detectEquivalences() {
    bool changed = false;
    // initialize pout with top
    partitions pout = {};
    partitions Top;
    Top.insert(GVN::createCongruenceClass());

    //deal with function
    if (func_->get_num_of_args()) {
        for(auto &arg : func_->get_args()) {
            auto argval = dynamic_cast<Value *>(arg);
            auto lval = ArgExpression::create(arg);        
            auto newC = createCongruenceClass(next_value_number_++);
            newC->value_expr_ = lval;
            newC->leader_ = argval;
            newC->members_.insert(newC->leader_);
            pout.insert(newC);
        }
    }

    //deal with global_variable
    for (auto &gv1 : m_->get_global_variable()) {
        auto gv = &gv1;
        auto gvVal = dynamic_cast<Value *>(gv);        
        auto lval = GlobalExpression::create(gv);    
        auto newC = createCongruenceClass(next_value_number_++);
        newC->value_expr_ = lval;
        newC->leader_ = gvVal;
        newC->members_.insert(newC->leader_);
        pout.insert(newC);
    }

    //deal with entry_bb's partition_out
    int flag = 0;
    for (auto &bb1 : func_->get_basic_blocks()) {
        auto bb = &bb1;

        if (flag == 0) {
            pin_.insert({bb, {}});
            for (auto &instr1 : bb->get_instructions()) {
                auto instr = &instr1;
                pout = transferFunction(instr, pout);
            }
            pout_.insert({bb, pout});
            flag = 1;
        } else {
            pout_.insert({bb, Top});
        }
    }

    // // iterate until converge
    int num = 0;
    do {
        // see the pseudo code in documentation
        int flag = 0; 
        changed = false;
        partitions ori_pout;
        for (auto &bb1 : func_->get_basic_blocks()) { // you might need to visit the blocks in depth-first order
            auto bb = &bb1;
            // get PIN of bb by predecessor(s)
            // iterate through all instructions in the block
            // and the phi instruction in all the successors
            if (flag == 0) {
                flag++;
                continue;
            }

            partitions pin = Top;
            for (auto &instr1 : bb->get_instructions()) {
                auto instr = &instr1;
                if (instr->is_phi()) copyStatment(instr);
            }
            ori_pout = pout_[bb];

            for (auto &bb_pre : bb->get_pre_basic_blocks()) {   
                pin = join(pin, pout_[bb_pre]);
            }
            if (pin_[bb] == pin && num != 0) continue;
            
            pin_[bb] = clone(pin);
            pout_[bb] = clone(pin_[bb]);

            for (auto &instr1 : bb->get_instructions()) {
                auto instr = &instr1;
                pout_[bb] = transferFunction(instr, pout_[bb]);
            // check changes in pout
            }
            if (!(pout_[bb] == ori_pout)) changed = true;
        }
        num++;   
    } while (changed);

    for (auto &bb1 : func_->get_basic_blocks()) {
        auto bb = &bb1;
        for (auto &cc : pout_[bb]) {
            if (cc->value_expr_ && cc->value_expr_->get_expr_type() == Expression::e_constant) {
                auto lcon = std::dynamic_pointer_cast<ConstantExpression>(cc->value_expr_);
                cc->leader_ = dynamic_cast<Constant *>(lcon->get_lval());
            }
        }
    }
}

shared_ptr<Expression> GVN::valueExpr(Instruction *instr, partitions pin) {
    // TODO
    if (instr->isBinary()) {
        shared_ptr<Expression> lval, rval;

        auto conLval = dynamic_cast<Constant *>(instr->get_operand(0));
        if (conLval) {
            lval = ConstantExpression::create(conLval, 0);
        } else {
            auto InsLval = dynamic_cast<Instruction *>(instr->get_operand(0));
            for (auto &cc : pin) {
                int flag = 0;
                for (auto mem : cc->members_) {
                    if (mem == instr->get_operand(0)) {
                        if (cc->value_expr_->get_expr_type() == Expression::e_constant && !InsLval->is_phi()) {
                            auto lcon = std::dynamic_pointer_cast<ConstantExpression>(cc->value_expr_);
                            lval = cc->value_expr_;
                            conLval = lcon->get_lval();
                        }
                        else if (cc->value_expr_->get_expr_type() == Expression::e_arg ||
                                 cc->value_expr_->get_expr_type() == Expression::e_global) {
                            lval = cc->value_expr_;
                        } else {
                            lval = VarExpression::create(mem, cc->value_expr_);
                        }
                        
                        flag = 1;
                        break;
                    }
                } 
                if (flag) break;
            }
        }

        auto conRval = dynamic_cast<Constant *>(instr->get_operand(1));
        if (conRval) {
            rval = ConstantExpression::create(conRval, 0);
        } else {
            auto InsRval = dynamic_cast<Instruction *>(instr->get_operand(1));
            for (auto &cc : pin) {
                int flag = 0;
                for (auto mem : cc->members_) {
                    if (mem == instr->get_operand(1)) {                 
                        if (cc->value_expr_->get_expr_type() == Expression::e_constant && !InsRval->is_phi()) {
                            auto rcon = std::dynamic_pointer_cast<ConstantExpression>(cc->value_expr_);
                            rval = cc->value_expr_;
                            conRval = rcon->get_lval();
                        }
                        else if (cc->value_expr_->get_expr_type() == Expression::e_arg ||
                                 cc->value_expr_->get_expr_type() == Expression::e_global) {
                            rval = cc->value_expr_;
                        } else {
                            rval = VarExpression::create(mem, cc->value_expr_);
                        }
                        
                        flag = 1;
                        break;
                    }
                }  
                if (flag) break;
            }
        }     

        if (conLval && conRval) {
            auto conval = folder_->compute(instr, conLval, conRval);
            auto conexp = ConstantExpression::create(conval, 0);
            return conexp;
        }

        if (!lval || !rval) return {};

        auto exp = BinaryExpression::create(instr->get_instr_type(), lval, rval);

        return exp;
    } 
    else if (instr->is_cmp() || instr->is_fcmp()) {
        shared_ptr<Expression> lval, rval;

        auto conLval = dynamic_cast<Constant *>(instr->get_operand(0));
        if (conLval) {
            lval = ConstantExpression::create(conLval, 0);
        } else {
            auto InsLval = dynamic_cast<Instruction *>(instr->get_operand(0));
            for (auto &cc : pin) {
                int flag = 0;
                for (auto mem : cc->members_) {
                    if (mem == instr->get_operand(0)) {
                        if (cc->value_expr_->get_expr_type() == Expression::e_constant && !InsLval->is_phi()) {
                            auto lcon = std::dynamic_pointer_cast<ConstantExpression>(cc->value_expr_);
                            lval = cc->value_expr_;
                            conLval = lcon->get_lval();
                        }
                        else if (cc->value_expr_->get_expr_type() == Expression::e_arg ||
                                 cc->value_expr_->get_expr_type() == Expression::e_global) {
                            lval = cc->value_expr_;
                        } else {
                            lval = VarExpression::create(mem, cc->value_expr_);
                        }
                        
                        flag = 1;
                        break;
                    }
                } 
                if (flag) break;
            }
        }

        auto conRval = dynamic_cast<Constant *>(instr->get_operand(1));
        if (conRval) {
            rval = ConstantExpression::create(conRval, 0);
        } else {
            auto InsRval = dynamic_cast<Instruction *>(instr->get_operand(1));
            for (auto &cc : pin) {
                int flag = 0;
                for (auto mem : cc->members_) {
                    if (mem == instr->get_operand(1)) {                 
                        if (cc->value_expr_->get_expr_type() == Expression::e_constant && !InsRval->is_phi()) {
                            auto rcon = std::dynamic_pointer_cast<ConstantExpression>(cc->value_expr_);
                            rval = cc->value_expr_;
                            conRval = rcon->get_lval();
                        }
                        else if (cc->value_expr_->get_expr_type() == Expression::e_arg ||
                                 cc->value_expr_->get_expr_type() == Expression::e_global) {
                            rval = cc->value_expr_;
                        } else {
                            rval = VarExpression::create(mem, cc->value_expr_);
                        }
                        
                        flag = 1;
                        break;
                    }
                }  
                if (flag) break;
            }
        }       

        if (conLval && conRval) {
            auto conval = folder_->compute(instr, conLval, conRval);
            auto conexp = ConstantExpression::create(conval, 1);
            return conexp;
        }

        if (!lval || !rval) return {};

        if (instr->is_cmp()) {
            auto exp = CmpExpression::create(dynamic_cast<CmpInst *>(instr)->get_cmp_op(), instr->get_instr_type(), lval, rval);
            return exp;
        } else {
            auto exp = FCmpExpression::create(dynamic_cast<FCmpInst *>(instr)->get_cmp_op(), instr->get_instr_type(), lval, rval);
            return exp;
        }
    } 
    else if (instr->is_call()) {
        auto func = dynamic_cast<Function *>(instr->get_operand(0));
        std::vector<std::shared_ptr<Expression>> args;
        shared_ptr<Expression> lval;
        bool is_pure = func_info_->is_pure_function(func);

        if (func->get_num_of_args()) {
            for (unsigned int i = 1; i < instr->get_num_operand(); i++) {
                auto conLval = dynamic_cast<Constant *>(instr->get_operand(i));
                if (conLval) {
                    lval = ConstantExpression::create(conLval, 0);
                } else {
                    auto InsLval = instr->get_operand(i);
                    for (auto &cc : pin) {
                        int flag = 0;
                        for (auto mem : cc->members_) {
                            if (mem == InsLval) {
                                if (cc->value_expr_->get_expr_type() == Expression::e_constant) {
                                    auto lcon = std::dynamic_pointer_cast<ConstantExpression>(cc->value_expr_);
                                    lval = cc->value_expr_;
                                    conLval = lcon->get_lval();
                                } 
                                else if (cc->value_expr_->get_expr_type() == Expression::e_arg ||
                                         cc->value_expr_->get_expr_type() == Expression::e_global) {
                                    lval = cc->value_expr_;
                                } else {
                                    lval = VarExpression::create(mem, cc->value_expr_);
                                }
                                
                                flag = 1;
                                break;
                            }
                        } 
                        if (flag) break;
                    }
                }

                if (!lval) return {};
                args.push_back(lval);
            }
        } 
        
        auto exp = FuncExpression::create(instr, func, args, is_pure);

        return exp;
    } 
    else if (instr->is_gep()) {
        shared_ptr<Expression> lval, rval, rrval;

        auto conLval = dynamic_cast<Constant *>(instr->get_operand(0));
        if (conLval) {
            lval = ConstantExpression::create(conLval, 0);
        } else {
            auto InsLval = instr->get_operand(0);
            for (auto &cc : pin) {
                int flag = 0;
                for (auto mem : cc->members_) {
                    if (mem == InsLval) {
                        if (cc->value_expr_->get_expr_type() == Expression::e_constant) {
                            lval = cc->value_expr_;
                        } 
                        else if (cc->value_expr_->get_expr_type() == Expression::e_arg ||
                                 cc->value_expr_->get_expr_type() == Expression::e_global) {
                            lval = cc->value_expr_;
                        } else {
                            lval = VarExpression::create(mem, cc->value_expr_);
                        }
                        
                        flag = 1;
                        break;
                    }
                } 
                if (flag) break;
            }
        }

        auto conRval = dynamic_cast<Constant *>(instr->get_operand(1));
        if (conRval) {
            rval = ConstantExpression::create(conRval, 0);
        } else {
            auto InsRval = instr->get_operand(1);
            for (auto &cc : pin) {
                int flag = 0;
                for (auto mem : cc->members_) {
                    if (mem == InsRval) {                       
                        if (cc->value_expr_->get_expr_type() == Expression::e_constant) {
                            rval = cc->value_expr_;
                        } 
                        else if (cc->value_expr_->get_expr_type() == Expression::e_arg ||
                                 cc->value_expr_->get_expr_type() == Expression::e_global) {
                            rval = cc->value_expr_;
                        } else {
                            rval = VarExpression::create(mem, cc->value_expr_);
                        }
                        
                        flag = 1;
                        break;
                    }
                }  
                if (flag) break;
            }
        }
        
        if (instr->get_num_operand() > 2) {
            auto conRRval = dynamic_cast<Constant *>(instr->get_operand(2));
            if (conRRval) {
                rrval = ConstantExpression::create(conRRval, 0);
            } else {
                auto InsRRval = instr->get_operand(2);
                for (auto &cc : pin) {
                    int flag = 0;
                    for (auto mem : cc->members_) {
                        if (mem == InsRRval) {                   
                            if (cc->value_expr_->get_expr_type() == Expression::e_constant) {
                                rrval = cc->value_expr_;
                            } 
                            else if (cc->value_expr_->get_expr_type() == Expression::e_arg ||
                                     cc->value_expr_->get_expr_type() == Expression::e_global) {
                                rrval = cc->value_expr_;
                            } else {
                                rrval = VarExpression::create(mem, cc->value_expr_);
                            }
                            
                            flag = 1;
                            break;
                        }
                    }  
                    if (flag) break;
                }
            }
        } 
        else rrval = {};

        if (!lval || !rval) return {};

        auto exp = GepExpression::create(lval, rval, rrval);

        return exp;
    }
    else if (instr->is_load()) {
        shared_ptr<Expression> lval;

        auto conLval = dynamic_cast<Constant *>(instr->get_operand(0));
        if (conLval) {
            lval = ConstantExpression::create(conLval, 0);
        } else {
            auto InsLval = instr->get_operand(0);
            for (auto &cc : pin) {
                int flag = 0;
                for (auto mem : cc->members_) {
                    if (mem == InsLval) {
                        if (cc->value_expr_->get_expr_type() == Expression::e_constant) {
                            lval = cc->value_expr_;
                        } 
                        else if (cc->value_expr_->get_expr_type() == Expression::e_arg ||
                                 cc->value_expr_->get_expr_type() == Expression::e_global) {
                            lval = cc->value_expr_;
                        } else {
                            lval = VarExpression::create(mem, cc->value_expr_);
                        }
                        
                        flag = 1;
                        break;
                    }
                } 
                if (flag) break;
            }
        }

        if (!lval) return {};

        auto exp = LoadExpression::create(instr, lval);

        return exp;
    }
    else if (instr->is_alloca()) {
        auto exp = AlloExpression::create(instr);

        return exp;
    }
    else if (instr->is_fp2si() || instr->is_si2fp() || instr->is_zext()) {
        shared_ptr<Expression> lval;

        auto conLval = dynamic_cast<Constant *>(instr->get_operand(0));
        if (conLval) {
            lval = ConstantExpression::create(conLval, 0);
        } else {
            auto InsLval = dynamic_cast<Instruction *>(instr->get_operand(0));
            for (auto &cc : pin) {
                int flag = 0;
                for (auto mem : cc->members_) {
                    if (mem == instr->get_operand(0)) {
                        if (cc->value_expr_->get_expr_type() == Expression::e_constant && !InsLval->is_phi()) {
                            auto lcon = std::dynamic_pointer_cast<ConstantExpression>(cc->value_expr_);
                            lval = cc->value_expr_;
                            conLval = lcon->get_lval();
                        } 
                        else if (cc->value_expr_->get_expr_type() == Expression::e_arg ||
                                 cc->value_expr_->get_expr_type() == Expression::e_global) {
                            lval = cc->value_expr_;
                        } else {
                            lval = VarExpression::create(mem, cc->value_expr_);
                        }
                        
                        flag = 1;
                        break;
                    }
                } 
                if (flag) break;
            }
        }

        if (conLval) {
            auto changeLval = folder_->compute(instr, conLval);
            auto conexp = ConstantExpression::create(changeLval, 0);
            return conexp;
        }

        if (!lval) return {};

        auto exp = SingleExpression::create(lval);

        return exp;
    }
    return {};
}

// instruction of the form `x = e`, mostly x is just e (SSA), but for copy stmt x is a phi instruction in the
// successor. Phi values (not copy stmt) should be handled in detectEquiv
/// \param bb basic block in which the transfer function is called
GVN::partitions GVN::transferFunction(Instruction *x, partitions pin) {
    partitions pout = clone(pin);

    if (x->is_void() || x->is_phi()) return pout;
    auto xval = dynamic_cast<Value *>(x);
    // TODO: get different ValueExpr by Instruction::OpID, modify pout
    std::set<Value *>::iterator it;
    for (auto &cc : pout) {
        if (cc->index_ == 0 || !cc->members_.size()) {
            pout.erase(cc);
            if (!pout.size()) break;
        }
        if ((it = cc->members_.find(xval)) != cc->members_.end()) {
            cc->members_.erase(it);
        }
    }

    auto ve = valueExpr(x, pout);
    if (!ve) return pout;
    shared_ptr<Expression> vpf;
    vpf = valuePhiFunc(ve, pout);

    int flag = 0;
    for (auto &cc : pout) {
        if(vpf && (*(cc->value_expr_) == *(vpf))){
            cc->members_.insert(xval);
            cc->value_expr_ = vpf;
            flag = 1;
            break;
        }   
        else if (*(cc->value_expr_) == *(ve)) {
            cc->members_.insert(xval);
            flag = 1;
            break;
        }
    }
    if (flag) return pout;
    
    auto newC = createCongruenceClass(next_value_number_++);
    if (vpf) newC->value_expr_ = vpf; 
    else newC->value_expr_ = ve;
    
    newC->leader_ = xval;
    newC->members_.insert(newC->leader_);
    pout.insert(newC);

    return pout;
}

shared_ptr<PhiExpression> GVN::valuePhiFunc(shared_ptr<Expression> ve, const partitions &pin) {
    // TODO
    if (ve->get_expr_type() == Expression::e_bin) {
        auto bve = std::dynamic_pointer_cast<BinaryExpression>(ve);
        if (bve->get_lexpr_type() == Expression::e_val && 
            bve->get_rexpr_type() == Expression::e_val) {
            auto lva = std::dynamic_pointer_cast<VarExpression>(bve->get_lval());
            auto rva = std::dynamic_pointer_cast<VarExpression>(bve->get_rval());
            
            auto lins = dynamic_cast<Instruction *>(lva->get_lval());
            auto rins = dynamic_cast<Instruction *>(rva->get_lval());
            
            if (!lins || !rins) return {};
            
            if (lins->is_phi() && rins->is_phi()) {
                shared_ptr<PhiExpression> lphi, rphi;

                lphi = std::dynamic_pointer_cast<PhiExpression>(lva->get_expr());
                rphi = std::dynamic_pointer_cast<PhiExpression>(rva->get_expr());

                if (!lphi || !rphi) return {};

                shared_ptr<Expression> lphi_lval, lphi_rval, rphi_lval, rphi_rval;
                
                lphi_lval = lphi->get_lval();  //vi1
                lphi_rval = lphi->get_rval();  //vj1
                rphi_lval = rphi->get_lval();  //vi2
                rphi_rval = rphi->get_rval();  //vj2
                
                shared_ptr<Expression> lexp, rexp;                
                partitions lpout, rpout;

                for (auto &cc : pin) {
                    if (cc->value_expr_ == lphi) {
                        auto instr = dynamic_cast<Instruction *>(cc->leader_);

                        if (!instr) return {};

                        auto ins_instr = dynamic_cast<Instruction *>(instr->get_operand(0));
                        
                        if (ins_instr) {
                            Constant *lphi_conval;
                            if (lphi_lval->get_expr_type() == Expression::e_constant && 
                                rphi_lval->get_expr_type() == Expression::e_constant) {
                                auto lcon = std::dynamic_pointer_cast<ConstantExpression>(lphi_lval);
                                auto rcon = std::dynamic_pointer_cast<ConstantExpression>(rphi_lval);

                                auto bconLval = lcon->get_lval();
                                auto bconRval = rcon->get_lval();
                                lphi_conval = folder_->compute(ins_instr, bconLval, bconRval);
                                lexp = ConstantExpression::create(lphi_conval, 0);
                            } else {
                                lexp = BinaryExpression::create(bve->get_instr_type(), lphi_lval, rphi_lval);
                            }
                        } 
                        else return {};
                        
                        auto bb = dynamic_cast<BasicBlock *>(instr->get_operand(1));
                        lpout = pout_[bb];

                        break;
                    }
                }

                for (auto &cc : pin) {
                    if (cc->value_expr_ == rphi) {
                        auto instr = dynamic_cast<Instruction *>(cc->leader_);

                        if (!instr) return {};

                        auto ins_instr = dynamic_cast<Instruction *>(instr->get_operand(2));

                        if (ins_instr) {
                            Constant *rphi_conval;
                            if (lphi_rval->get_expr_type() == Expression::e_constant && 
                                rphi_rval->get_expr_type() == Expression::e_constant) {
                                auto lcon = std::dynamic_pointer_cast<ConstantExpression>(lphi_rval);
                                auto rcon = std::dynamic_pointer_cast<ConstantExpression>(rphi_rval);

                                auto bconLval = lcon->get_lval();
                                auto bconRval = rcon->get_lval();
                                rphi_conval = folder_->compute(ins_instr, bconLval, bconRval);
                                rexp = ConstantExpression::create(rphi_conval, 0);
                            } else {
                                rexp = BinaryExpression::create(bve->get_instr_type(), lphi_rval, rphi_rval);
                            }
                        } 
                        else return {};

                        auto bb = dynamic_cast<BasicBlock *>(instr->get_operand(3));
                        rpout = pout_[bb];

                        break;
                    }
                }

                auto vi = getVN(lpout, lexp);
                if (!vi) vi = valuePhiFunc(lexp, lpout);
                
                auto vj = getVN(rpout, rexp);
                if (!vj) vj = valuePhiFunc(rexp, rpout);

                if (vi && vj) return PhiExpression::create(vi, vj);
            }
        }
    } 
    return {};
}

shared_ptr<Expression> GVN::getVN(const partitions &pout, shared_ptr<Expression> ve) {
    // TODO: return what?
    for (auto it = pout.begin(); it != pout.end(); it++) {
        if ((*it)->value_expr_ and *(*it)->value_expr_ == *ve) {
            if ((*it)->value_expr_->get_expr_type() == Expression::e_constant) {
                return (*it)->value_expr_;
            }
            
            auto var = VarExpression::create((*it)->leader_, (*it)->value_expr_);
            return var;
        }
    }
    return {};
}

void GVN::initPerFunction() {
    next_value_number_ = 1;
    pin_.clear();
    pout_.clear();
}

void GVN::replace_cc_members() {
    for (auto &[_bb, part] : pout_) {
        auto bb = _bb; // workaround: structured bindings can't be captured in C++17
        for (auto &cc : part) {
            if (cc->index_ == 0)
                continue;
            // if you are planning to do constant propagation, leaders should be set to constant at some point
            for (auto &member : cc->members_) {
                bool member_is_phi = dynamic_cast<PhiInst *>(member);
                bool value_phi = cc->value_phi_ != nullptr;
                if (member != cc->leader_ and (value_phi or !member_is_phi)) {
                    // only replace the members if users are in the same block as bb
                    member->replace_use_with_when(cc->leader_, [bb](User *user) {
                        if (auto instr = dynamic_cast<Instruction *>(user)) {
                            auto parent = instr->get_parent();
                            auto &bb_pre = parent->get_pre_basic_blocks();
                            if (instr->is_phi()) // as copy stmt, the phi belongs to this block
                                return std::find(bb_pre.begin(), bb_pre.end(), bb) != bb_pre.end();
                            else
                                return parent == bb;
                        }
                        return false;
                    });
                }
            }
        }
    }
    return;
}

// top-level function, done for you
void GVN::run() {
    m_->set_print_name();
    
    std::ofstream gvn_json;
    if (dump_json_) {
        gvn_json.open("gvn.json", std::ios::out);
        gvn_json << "[";
    }

    folder_ = std::make_unique<ConstFolder>(m_);
    func_info_ = std::make_unique<FuncInfo>(m_);
    func_info_->run();
    dce_ = std::make_unique<DeadCode>(m_);
    dce_->run(); // let dce take care of some dead phis with undef

    for (auto &f : m_->get_functions()) {
        if (f.get_basic_blocks().empty())
            continue;
        func_ = &f;
        initPerFunction();
        LOG_INFO << "Processing " << f.get_name();
        detectEquivalences();
        LOG_INFO << "\n===============pin=========================\n";
        for (auto &[bb, part] : pin_) {
            LOG_INFO << "\n===============bb: " << bb->get_name() << "=========================\npartitionIn: ";
            for (auto &cc : part)
                LOG_INFO << utils::print_congruence_class(*cc);
        }
        LOG_INFO << "\n===============pout=========================\n";
        for (auto &[bb, part] : pout_) {
            LOG_INFO << "\n=====bb: " << bb->get_name() << "=====\npartitionOut: ";
            for (auto &cc : part)
                LOG_INFO << utils::print_congruence_class(*cc);
        }
        if (dump_json_) {
            gvn_json << "{\n\"function\": ";
            gvn_json << "\"" << f.get_name() << "\", ";
            gvn_json << "\n\"pout\": " << utils::dump_bb2partition(pout_);
            gvn_json << "},";
        }
        replace_cc_members(); // don't delete instructions, just replace them
    }
    dce_->run(); // let dce do that for us
    if (dump_json_)
        gvn_json << "]";
}

template <typename T>
static bool equiv_as(const Expression &lhs, const Expression &rhs) {
    // we use static_cast because we are very sure that both operands are actually T, not other types.
    return static_cast<const T *>(&lhs)->equiv(static_cast<const T *>(&rhs));
}

bool GVNExpression::operator==(const Expression &lhs, const Expression &rhs) {
    if (lhs.get_expr_type() != rhs.get_expr_type())
        return false;
    switch (lhs.get_expr_type()) {
    case Expression::e_constant: return equiv_as<ConstantExpression>(lhs, rhs);
    case Expression::e_bin: return equiv_as<BinaryExpression>(lhs, rhs);
    case Expression::e_phi: return equiv_as<PhiExpression>(lhs, rhs);
    case Expression::e_func: return equiv_as<FuncExpression>(lhs, rhs);
    case Expression::e_val: return equiv_as<VarExpression>(lhs, rhs);
    case Expression::e_load: return equiv_as<LoadExpression>(lhs, rhs);
    case Expression::e_global: return equiv_as<GlobalExpression>(lhs, rhs);
    case Expression::e_gep: return equiv_as<GepExpression>(lhs, rhs);
    case Expression::e_arg: return equiv_as<ArgExpression>(lhs, rhs);
    case Expression::e_alloca: return equiv_as<AlloExpression>(lhs, rhs);
    case Expression::e_single: return equiv_as<SingleExpression>(lhs, rhs);
    case Expression::e_cmp: return equiv_as<CmpExpression>(lhs, rhs);
    case Expression::e_fcmp: return equiv_as<FCmpExpression>(lhs, rhs);
    case Expression::e_valf: return equiv_as<ConstantfExpression>(lhs, rhs);
    }
    return false;
}

bool GVNExpression::operator==(const shared_ptr<Expression> &lhs, const shared_ptr<Expression> &rhs) {
    if (lhs == nullptr and rhs == nullptr) // is the nullptr check necessary here?
        return true;
    return lhs and rhs and *lhs == *rhs;
}

GVN::partitions GVN::clone(const partitions &p) {
    partitions data;
    for (auto &cc : p) {
        data.insert(std::make_shared<CongruenceClass>(*cc));
    }
    return data;
}

bool operator==(const GVN::partitions &p1, const GVN::partitions &p2) {
    // TODO: how to compare partitions?
    if (!p1.size() && !p2.size()) return true;

    if (!p1.size() || !p2.size()) return false;

    if (p1.size() == p2.size()) {
        auto it1 = p1.cbegin();
        auto it2 = p2.cbegin();
        while (it1 != p1.cend() && it2 != p2.cend()) {           
            if (!(**it1 == **it2)) return false;
            it1++;
            it2++;
        }        
        return true;
    }
    return false;
}

bool CongruenceClass::operator==(const CongruenceClass &other) const {
    // TODO: which fields need to be compared?
    if (this->value_expr_ == other.value_expr_ && this->members_.size() == other.members_.size()) {
        auto it1 = this->members_.cbegin();
        auto it2 = other.members_.cbegin();
        while (it1 != this->members_.cend() && it2 != other.members_.cend()) {
            if (!(*it1 == *it2)) return false;
            it1++;
            it2++;
        }        
        return true;
    } 

    return false;
}