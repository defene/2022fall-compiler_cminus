#include "codegen.hpp"

// $r0          $zero       constant 0
// $r1          $ra         return address
// $r2          $tp         thread pointer
// $r3          $sp         stack pointer
// $r4 - $r5    $a0 - $a1   argument, return value
// $r6 - $r11   $a2 - $a7   argument
// $r12 - $r20  $t0 - $t8   temporary
// $r21                     saved
// $r22         $fp         frame pointer
// $r23 - $r31  $s0 - $s8   static

class Reg {
  public:
    Reg(int index) : id(index) {}

    int id;

    string print() {
        if (id == 0)
            return "$zero";
        if (id == 1)
            return "$ra";
        if (id == 2)
            return "$tp";
        if (id == 3)
            return "$sp";
        if (4 <= id and id <= 11)
            return "$a" + std::to_string(id - 4);
        if (12 <= id and id <= 20)
            return "$t" + std::to_string(id - 12);
        if (id == 22)
            return "$fp";
        if (23 <= id and id <= 31)
            return "$ft" + std::to_string(id - 23);
        if (32 <= id and id <= 40)
            return "$fa" + std::to_string(id - 32);
        assert(false);
    }
};

void CodeGen::dfs(BasicBlock *bsb) {
    for (auto bb : bsb->get_succ_basic_blocks()) {
        auto it = std::find(begin(dfs_basic_blocks_), end(dfs_basic_blocks_), bb);
        if (it == dfs_basic_blocks_.end()) {
            dfs_basic_blocks_.push_back(bb);
            dfs(bb);
        }
    }
}

void CodeGen::calcUse(Instruction *instr, BasicBlock *bb) {
    if (instr->isBinary() || instr->is_cmp() || instr->is_fcmp() || instr->is_store()) {
        auto Lval = dynamic_cast<Value *>(instr->get_operand(0));
        auto Rval = dynamic_cast<Value *>(instr->get_operand(1));

        if (dynamic_cast<Instruction *>(instr->get_operand(0))) {
            postion *use = new(postion);
            use->bb = bb; use->insnum = insnum_;
            for (auto var : duChain_list_) {
                if (var->leader_ == Lval) {
                    var->use_.insert(use);
                    break;
                }
            }
        }
        if (dynamic_cast<Instruction *>(instr->get_operand(1))) {
            postion *use = new(postion);
            use->bb = bb; use->insnum = insnum_;
            for (auto var : duChain_list_) {
                if (var->leader_ == Rval) {
                    var->use_.insert(use);
                    break;
                }
            }
        }
    } 
    else if (instr->is_call()) {
        auto func = dynamic_cast<Function *>(instr->get_operand(0));
        if (func->get_num_of_args()) {
            for (unsigned int i = 1; i < instr->get_num_operand(); i++) {
                auto argval = dynamic_cast<Value *>(instr->get_operand(i));

                if (dynamic_cast<Instruction *>(instr->get_operand(i))) {
                    postion *use = new(postion);
                    use->bb = bb; use->insnum = insnum_;
                    for (auto var : duChain_list_) {
                        if (var->leader_ == argval) {
                            var->use_.insert(use);
                            break;
                        }
                    }
                }
            }
        } 
    } 
    else if (instr->is_gep()) {
        auto Lval = dynamic_cast<Value *>(instr->get_operand(0));
        auto Rval = dynamic_cast<Value *>(instr->get_operand(1));

        if (dynamic_cast<Instruction *>(instr->get_operand(0))) {
            postion *use = new(postion);
            use->bb = bb; use->insnum = insnum_;
            for (auto var : duChain_list_) {
                if (var->leader_ == Lval) {
                    var->use_.insert(use);
                }
            }
        }
        if (dynamic_cast<Instruction *>(instr->get_operand(1))) {
            postion *use = new(postion);
            use->bb = bb; use->insnum = insnum_;
            for (auto var : duChain_list_) {
                if (var->leader_ == Rval) {
                    var->use_.insert(use);
                    break;
                }
            }
        }
        if (instr->get_num_operand() > 2 && dynamic_cast<Instruction *>(instr->get_operand(2))) {
            auto RRval = dynamic_cast<Value *>(instr->get_operand(2));
            postion *use = new(postion);
            use->bb = bb; use->insnum = insnum_;
            for (auto var : duChain_list_) {
                if (var->leader_ == RRval) {
                    var->use_.insert(use);
                    break;
                }
            }
        } 
    }
    else if (instr->is_fp2si() || instr->is_si2fp() || instr->is_zext() ||
             instr->is_load() || instr->isTerminator()) {
        auto argval = dynamic_cast<Value *>(instr->get_operand(0));
        if (dynamic_cast<Instruction *>(instr->get_operand(0))) {
            postion *use = new(postion);
            use->bb = bb; use->insnum = insnum_;
            for (auto var : duChain_list_) {
                if (var->leader_ == argval) {
                    var->use_.insert(use);
                    break;
                }
            }
        }
    }
}

void CodeGen::detectDUchain() {
    for (auto &bb1 : func_->get_basic_blocks()) {
        auto bb = &bb1;
        for (auto &instr1 : bb->get_instructions()) {
            auto instr = &instr1;
            
            if (!instr->is_void()) {
                auto insval = dynamic_cast<Value *>(instr);
                int flag = 0;
                postion *def = new(postion);
                def->bb = bb; def->insnum = insnum_;
                
                for (auto var : duChain_list_) {
                    if (var->leader_ == insval) {
                        var->def_ = def;
                        flag = 1;
                        break;
                    }
                }

                if (!flag) {
                    duChain *newv = new(duChain);
                    
                    newv->leader_ = insval; newv->def_ = def; newv->is_arg  = false;
                    newv->regnum = -20; newv->realoffset = -1; newv->is_global = false;
                    if (instr->is_alloca()) {
                        auto type = dynamic_cast<AllocaInst *>(instr)->get_alloca_type();
                        offset_ += type->get_size();
                        newv->offset = offset_;
                    }
                    else {
                        newv->offset = 0;
                    }
                    duChain_list_.push_back(newv);
                }
            }
            calcUse(instr, bb);
            insnum_++;
        }

        for (auto sucbb : bb->get_succ_basic_blocks()) {
            for (auto &instr1 : sucbb->get_instructions()) {
                auto instr = &instr1;
                if (!instr->is_phi()) break;
                auto Lval = instr->get_operand(0);
                auto Rval = instr->get_operand(2);
                auto Lbb = dynamic_cast<BasicBlock *>(instr->get_operand(1));
                auto Rbb = dynamic_cast<BasicBlock *>(instr->get_operand(3));

                if (dynamic_cast<Instruction *>(instr->get_operand(0)) && Lbb == bb) {
                    postion *use = new(postion);
                    use->bb = bb; use->insnum = insnum_ - 1;
                    for (auto var : duChain_list_) {
                        if (var->leader_ == Lval) {
                            var->use_.insert(use);
                            break;
                        }
                    }
                }
                if (dynamic_cast<Instruction *>(instr->get_operand(2)) && Rbb == bb) {
                    postion *use = new(postion);
                    use->bb = bb; use->insnum = insnum_ - 1;
                    for (auto var : duChain_list_) {
                        if (var->leader_ == Rval) {
                            var->use_.insert(use);
                            break;
                        }
                    }
                }
            }
        }
    }

    for (auto du : duChain_list_) {
        if (!du->use_.size()) {
            postion *use = new(postion);
            use->bb = du->def_->bb; use->insnum = du->def_->insnum;
            du->use_.insert(use);
        }
    }
}

bool compare(duChain *a, duChain *b) {
    auto it1 = a->use_.end(); it1--;
    auto it2 = b->use_.end(); it2--;

    auto a1 = (*it1)->insnum;
    auto b1 = (*it2)->insnum;

    return a1 < b1;
}

void CodeGen::ExpireOldIntervals(duChain *du, bool type) {
    std::vector<duChain *>::iterator it;
    if (type) {
        for (it = factive_list_.begin(); it != factive_list_.end(); ) {
            auto it1 = (*it)->use_.end(); it1--; 
            auto a1 = (*it1)->insnum;
            if (a1 > du->def_->insnum) {
                return;
            }
            fregisters[(*it)->regnum - 23] = 1;
            it = factive_list_.erase(it);
        }
    } 
    else {
        for (it = active_list_.begin(); it != active_list_.end(); ) {
            auto it1 = (*it)->use_.end(); it1--; 
            auto a1 = (*it1)->insnum;
            if (a1 > du->def_->insnum) {
                return;
            }
            registers[(*it)->regnum - 12] = 1;
            it = active_list_.erase(it);
        }
    }
}

void CodeGen::SpillAtInterval(duChain *du, bool type) {
    if (type) {
        auto it1 = factive_list_.end(); it1--;
        auto it2 = (*it1)->use_.end(); it2--;
        auto spill = (*it2)->insnum; 
        auto it3 = du->use_.end(); it3--;
        auto inter = (*it3)->insnum; 
        if (spill > inter) {
            du->regnum = (*it1)->regnum;
            (*it1)->regnum = -20;
            offset_ += 4;
            (*it1)->offset = offset_;
            it1 = factive_list_.erase(it1);
            factive_list_.insert(it1, du);
        }
        else {
            du->regnum = -20;
            offset_ += 4;
            du->offset = offset_;
        }
    }
    else {
        auto it1 = active_list_.end(); it1--;
        auto it2 = (*it1)->use_.end(); it2--;
        auto spill = (*it2)->insnum; 
        auto it3 = du->use_.end(); it3--;
        auto inter = (*it3)->insnum; 
        if (spill > inter) {
            du->regnum = (*it1)->regnum;
            (*it1)->regnum = -20;
            offset_ += 8;
            (*it1)->offset = offset_;
            it1 = active_list_.erase(it1);
            active_list_.insert(it1, du);
        }
        else {
            du->regnum = -20;
            offset_ += 8;
            du->offset = offset_;
        }
    }
}

void CodeGen::LinearScan() {
    for (auto du : duChain_list_) {
        auto it1 = du->use_.end(); it1--; 
        if (du->leader_->get_type()->is_float_type()) {
            ExpireOldIntervals(du, 1);
            if (factive_list_.size() == regsize_) {
                SpillAtInterval(du, 1);
            }
            else {
                for (int i = 0; i < 8; i++) {
                    if (fregisters[i] == true) {
                        du->regnum = i + 23; 
                        if (i >= maxfreg) maxfreg = i + 1;
                        fregisters[i] = false;
                        factive_list_.push_back(du);
                        break;
                    }
                }
                std::sort(factive_list_.begin(), factive_list_.end(), compare);
            }
        }
        else {
            ExpireOldIntervals(du, 0);
            if (active_list_.size() == regsize_) {
                SpillAtInterval(du, 0);
            }
            else {
                for (int i = 0; i < 8; i++) {
                    if (registers[i] == 1) {
                        du->regnum = i + 12; 
                        if (i >= maxreg) maxreg = i + 1;
                        registers[i] = 0;
                        active_list_.push_back(du);
                        break;
                    }
                }
                std::sort(active_list_.begin(), active_list_.end(), compare);
            }
        }
    }
}

void CodeGen::binarygen(Instruction *instr) {
    auto op = instr->get_instr_type();
    std::string str, rs1, rs2;
    switch (op) {
    case Instruction::add: 
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        if (du->regnum == -20) {
                            output.push_back("addi.w $r21, " + rs1 + ", " + con->print());
                            output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("addi.w " + reg.print() + ", " + rs1 + ", " + con->print());
                        }
                    }
                    else {
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $r24, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $r24, $r24, " + std::to_string(num % 4096));
                        if (du->regnum == -20) {
                            output.push_back("add.w $r21, " + rs1 + ", $r24");
                            output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("add.w " + reg.print() + ", " + rs1 + ", $r24");
                        }
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        if (du->regnum == -20) {
                            output.push_back("addi.w $r21, " + rs1 + ", " + con->print());
                            output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("addi.w " + reg.print() + ", " + rs1 + ", " + con->print());
                        }
                    }
                    else {
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $r24, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $r24, $r24, " + std::to_string(num % 4096));
                        if (du->regnum == -20) {
                            output.push_back("add.w $r21, " + rs1 + ", $r24");
                            output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("add.w " + reg.print() + ", " + rs1 + ", $r24");
                        }
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else {
            str = "add.w ";
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (du->regnum == -20) {
                        str = str + "$r21, " + rs1 + ", " + rs2;
                        output.push_back(str);
                        output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", " + rs2;
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs2 = reg.print();
                    }
                }
            }
        }
        break;
    case Instruction::sub:
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        if (du->regnum == -20) {
                            output.push_back("addi.w $r21, " + rs1 + ", " + con->print());
                            output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("addi.w " + reg.print() + ", " + rs1 + ", " + con->print());
                        }
                    }
                    else {
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $r24, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $r24, $r24, " + std::to_string(num % 4096));
                        if (du->regnum == -20) {
                            output.push_back("sub.w $r21, $r24, " + rs1);
                            output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("sub.w " + reg.print() + ", $r24, " + rs1);
                        }
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        if (du->regnum == -20) {
                            output.push_back("addi.w $r21, " + rs1 + ", -" + con->print());
                            output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("addi.w " + reg.print() + ", " + rs1 + ", -" + con->print());
                        }
                    }
                    else {
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $r24, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $r24, $r24, " + std::to_string(num % 4096));
                        if (du->regnum == -20) {
                            output.push_back("sub.w $r21, " + rs1 + ", $r24");
                            output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("sub.w " + reg.print() + ", " + rs1 + ", $r24");
                        }
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else {
            str = "sub.w ";
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (du->regnum == -20) {
                        str = str + "$r21, " + rs1 + ", " + rs2;
                        output.push_back(str);
                        output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", " + rs2;
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs2 = reg.print();
                    }
                }
            }
        }
        break;
    case Instruction::mul:   
        str = "mul.w ";      
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {                 
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        if (du->regnum == -20) {
                            output.push_back("ori $r21, $zero, " + con->print());
                            output.push_back(str + "$r21, " + rs1 + ", $r21");
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("ori " + reg.print() + ", $zero, " + con->print());
                            output.push_back(str + reg.print() + ", " + rs1 + ", " + reg.print());
                        }
                    }
                    else {
                        Reg reg(du->regnum);
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $r21, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $r21, $r21, " + std::to_string(num % 4096));
                        output.push_back(str + reg.print() + ", " + rs1 + ", $r21");
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {                 
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        if (du->regnum == -20) {
                            output.push_back("ori $r21, $zero, " + con->print());
                            output.push_back(str + "$r21, " + rs1 + ", $r21");
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("ori " + reg.print() + ", $zero, " + con->print());
                            output.push_back(str + reg.print() + ", " + rs1 + ", " + reg.print());
                        }
                    }
                    else {
                        Reg reg(du->regnum);
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $r21, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $r21, $r21, " + std::to_string(num % 4096));
                        output.push_back(str + reg.print() + ", " + rs1 + ", $r21");
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (du->regnum == -1) {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", " + rs2;
                        output.push_back(str);
                        output.push_back("st.w " + reg.print() + ", $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", " + rs2;
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs2 = reg.print();
                    }
                }
            }
        }
        break;
    case Instruction::sdiv: 
        str = "div.w ";         
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {                 
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        if (du->regnum == -20) {
                            output.push_back("ori $r21, $zero, " + con->print());
                            output.push_back(str + "$r21, $r21, " + rs1);
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("ori " + reg.print() + ", $zero, " + con->print());
                            output.push_back(str + reg.print() + ", " + reg.print() + ", " + rs1);
                        }
                    }
                    else {
                        Reg reg(du->regnum);
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $r21, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $r21, $r21, " + std::to_string(num % 4096));
                        output.push_back(str + reg.print() + ", $r21, " + rs1);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {                 
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        if (du->regnum == -20) {
                            output.push_back("ori $r21, $zero, " + con->print());
                            output.push_back(str + "$r21, " + rs1 + ", $r21");
                        }
                        else {
                            Reg reg(du->regnum);
                            output.push_back("ori " + reg.print() + ", $zero, " + con->print());
                            output.push_back(str + reg.print() + ", " + rs1 + ", " + reg.print());
                        }
                    }
                    else {
                        Reg reg(du->regnum);
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $r21, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $r21, $r21, " + std::to_string(num % 4096));
                        output.push_back(str + reg.print() + ", " + rs1 + ", $r21");
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (du->regnum == -1) {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", " + rs2;
                        output.push_back(str);
                        output.push_back("st.w " + reg.print() + ", $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", " + rs2;
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs2 = reg.print();
                    }
                }
            }
        }
        break;
    case Instruction::fadd:
        str = "fadd.s ";
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                    fcon_list_.push_back(dynamic_cast<ConstantFP *>(con)); flabel_++;
                    output.push_back("fld.s $ft8, $r21, 0");
                    if (du->regnum == -20) {
                        str = str + "$ft8, " + rs1 + ", $ft8";
                        output.push_back(str);
                        output.push_back("fst.s $ft8, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", $ft8";
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $ft9, $fp, " + std::to_string(-du->offset));
                        rs1 = "$ft9";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                    fcon_list_.push_back(dynamic_cast<ConstantFP *>(con)); flabel_++;
                    output.push_back("fld.s $ft8, $r21, 0");
                    if (du->regnum == -20) {
                        str = str + "$ft8, " + rs1 + ", $ft8";
                        output.push_back(str);
                        output.push_back("fst.s $ft8, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", $ft8";
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $ft9, $fp, " + std::to_string(-du->offset));
                        rs1 = "$ft9";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (du->regnum == -20) {
                        str = str + "$r21, " + rs1 + ", " + rs2;
                        output.push_back(str);
                        output.push_back("fst.s $r21, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", " + rs2;
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs2 = reg.print();
                    }
                }
            }
        }
        break;
    case Instruction::fsub:
        str = "fsub.s ";
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                    fcon_list_.push_back(dynamic_cast<ConstantFP *>(con)); flabel_++;
                    output.push_back("fld.s $ft8, $r21, 0");
                    if (du->regnum == -20) {
                        str = str + "$ft8, " + rs1 + ", $ft8";
                        output.push_back(str);
                        output.push_back("fst.s $ft8, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", $ft8";
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $ft9, $fp, " + std::to_string(-du->offset));
                        rs1 = "$ft9";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                    fcon_list_.push_back(dynamic_cast<ConstantFP *>(con)); flabel_++;
                    output.push_back("fld.s $ft8, $r21, 0");
                    if (du->regnum == -20) {
                        str = str + "$ft8, " + rs1 + ", $ft8";
                        output.push_back(str);
                        output.push_back("fst.s $ft8, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", $ft8";
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $ft9, $fp, " + std::to_string(-du->offset));
                        rs1 = "$ft9";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (du->regnum == -20) {
                        str = str + "$r21, " + rs1 + ", " + rs2;
                        output.push_back(str);
                        output.push_back("fst.s $r21, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", " + rs2;
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs2 = reg.print();
                    }
                }
            }
        }
        break;
    case Instruction::fmul:
        str = "fmul.s ";
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                    fcon_list_.push_back(dynamic_cast<ConstantFP *>(con)); flabel_++;
                    output.push_back("fld.s $ft8, $r21, 0");
                    if (du->regnum == -20) {
                        str = str + "$ft8, " + rs1 + ", $ft8";
                        output.push_back(str);
                        output.push_back("fst.s $ft8, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", $ft8";
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $ft9, $fp, " + std::to_string(-du->offset));
                        rs1 = "$ft9";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                    fcon_list_.push_back(dynamic_cast<ConstantFP *>(con)); flabel_++;
                    output.push_back("fld.s $ft8, $r21, 0");
                    if (du->regnum == -20) {
                        str = str + "$ft8, " + rs1 + ", $ft8";
                        output.push_back(str);
                        output.push_back("fst.s $ft8, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", $ft8";
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $ft9, $fp, " + std::to_string(-du->offset));
                        rs1 = "$ft9";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (du->regnum == -20) {
                        str = str + "$r21, " + rs1 + ", " + rs2;
                        output.push_back(str);
                        output.push_back("fst.s $r21, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", " + rs2;
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs2 = reg.print();
                    }
                }
            }
        }
        break;
    case Instruction::fdiv:
        str = "fdiv.s ";
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                    fcon_list_.push_back(dynamic_cast<ConstantFP *>(con)); flabel_++;
                    output.push_back("fld.s $ft8, $r21, 0");
                    if (du->regnum == -20) {
                        str = str + "$ft8, " + rs1 + ", $ft8";
                        output.push_back(str);
                        output.push_back("fst.s $ft8, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", $ft8";
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $ft9, $fp, " + std::to_string(-du->offset));
                        rs1 = "$ft9";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                    fcon_list_.push_back(dynamic_cast<ConstantFP *>(con)); flabel_++;
                    output.push_back("fld.s $ft8, $r21, 0");
                    if (du->regnum == -20) {
                        str = str + "$ft8, " + rs1 + ", $ft8";
                        output.push_back(str);
                        output.push_back("fst.s $ft8, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", $ft8";
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $ft9, $fp, " + std::to_string(-du->offset));
                        rs1 = "$ft9";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    if (du->regnum == -20) {
                        str = str + "$r21, " + rs1 + ", " + rs2;
                        output.push_back(str);
                        output.push_back("fst.s $r21, $fp, " + std::to_string(-du->offset));
                    }
                    else {
                        Reg reg(du->regnum);
                        str = str + reg.print() + ", " + rs1 + ", " + rs2;
                        output.push_back(str);
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("fld.s $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs2 = reg.print();
                    }
                }
            }
        }
        break;
    default: break;
    }
}

void CodeGen::callgen(Instruction *instr) {
    auto func = dynamic_cast<Function *>(instr->get_operand(0));
    if (func->get_num_of_args()) {
        for (unsigned int i = 1; i < instr->get_num_operand(); i++) {
            if (i <= 8) {
                auto conLval = dynamic_cast<Constant *>(instr->get_operand(i));
                if (dynamic_cast<ConstantInt *>(conLval)) {  
                    Reg reg(i + 3);                  
                    if (dynamic_cast<ConstantInt *>(conLval)->get_value() <= 2047) {
                        output.push_back("ori " + reg.print() + ", $zero, " + conLval->print());
                    }
                    else {
                        auto num = dynamic_cast<ConstantInt *>(conLval)->get_value();
                        output.push_back("lu12i.w " + reg.print() + ", " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori "+ reg.print() +", " + reg.print()  + ", " + std::to_string(num % 4096));
                    }
                } 
                else if (dynamic_cast<ConstantFP *>(conLval)) {
                    Reg reg(i + 31);
                    output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                    fcon_list_.push_back(dynamic_cast<ConstantFP *>(conLval)); flabel_++;
                    output.push_back("fld.s $ft8, $r21, 0");
                    output.push_back("fmov.s "+ reg.print()  + ", $ft8");
                }
                else {
                    auto InsLval = dynamic_cast<Value *>(instr->get_operand(i));
                    for (auto du : duChain_list_) {
                        if (du->leader_ == InsLval) {
                            Reg rs(du->regnum);
                            if (du->leader_->get_type()->is_float_type()) {
                                Reg reg(i + 32);
                                output.push_back("fmov.s " + reg.print() + ", " + rs.print());
                            }
                            else {
                                Reg reg(i + 3);
                                output.push_back("or " + reg.print() + ", $zero, " + rs.print());
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    output.push_back("bl " + func->get_name());
    if (!instr->is_void()) {
        for (auto du : duChain_list_) {
            if (du->leader_ == dynamic_cast<Value *>(instr)) {
                if (du->regnum == -20) {
                    output.push_back("st.w $r4, $r12, " + std::to_string(-du->offset));
                }
                else {
                    Reg reg(du->regnum);
                    if (du->leader_->get_type()->is_float_type()) {
                        output.push_back("fmov.s " + reg.print() + ", $fa0");
                    }
                    else {
                        output.push_back("or " + reg.print() + ", $r4, $zero ");
                    }
                }
            }
        }
    }
}

void CodeGen::cmpgen(Instruction *instr, Instruction *binstr) {
    std::string str, str2, rs1, rs2;
    auto trueBB = dynamic_cast<BasicBlock *>(binstr->get_operand(1));
    auto falseBB = dynamic_cast<BasicBlock *>(binstr->get_operand(2));
    switch (dynamic_cast<CmpInst *>(instr)->get_cmp_op()) {
    case CmpInst::EQ: 
        str = "beq ";
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + "$r21, " + rs1 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + "$r21, " + rs1 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        rs1 = reg.print();
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", " + rs2 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r24, $zero, " + reg.print());
                        rs2 = "$r24";
                    }
                }
            }
        }
        break;
    case CmpInst::NE:
        str = "bne ";
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + "$r21, " + rs1 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", $r21, ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", " + rs2 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r24, $zero, " + reg.print());
                        rs2 = "$r24";
                    }
                }
            }
        }
        break;
    case CmpInst::GT: 
        str = "bge ";
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + "$r21, " + rs1 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", $r21, ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", " + rs2 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r24, $zero, " + reg.print());
                        rs2 = "$r24";
                    }
                }
            }
        }
        break;
    case CmpInst::GE: 
        str = "bge "; str2 = "beq ";
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + "$r21, " + rs1 + ", ." + func_->get_name() + trueBB->get_name();
                    str2 = str2 + "$r21, " + rs1 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back(str2);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", $r21, ." + func_->get_name() + trueBB->get_name();
                    str2 = str2 + rs1 + ", $r21, ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back(str2);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", " + rs2 + ", ." + func_->get_name() + trueBB->get_name();
                    str2 = str2 + rs1 + ", " + rs2 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back(str2);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r24, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r24, $zero, " + reg.print());
                        rs2 = "$r24";
                    }
                }
            }
        }
        break;
    case CmpInst::LT: 
        str = "blt ";
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + "$r21, " + rs1 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", $r21, ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", " + rs2 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r24, $zero, " + reg.print());
                        rs2 = "$r24";
                    }
                }
            }
        }
        break;
    case CmpInst::LE: 
        str = "blt "; str2 = "beq ";
        if (dynamic_cast<Constant *>(instr->get_operand(0))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(0));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + "$r21, " + rs1 + ", ." + func_->get_name() + trueBB->get_name();
                    str2 = str2 + "$r21, " + rs1 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back(str2);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
            }
        }
        else if (dynamic_cast<Constant *>(instr->get_operand(1))) {
            auto con = dynamic_cast<Constant *>(instr->get_operand(1));
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", $r21, ." + func_->get_name() + trueBB->get_name();
                    str2 = str2 + rs1 + ", $r21, ." + func_->get_name() + trueBB->get_name();
                    output.push_back("addi.w $r21, $zero, " + con->print());
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back(str2);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
            }
        }
        else {
            for (auto du : duChain_list_) {
                if (du->leader_ == dynamic_cast<Value *>(instr)) {
                    Reg reg(du->regnum);
                    str = str + rs1 + ", " + rs2 + ", ." + func_->get_name() + trueBB->get_name();
                    str2 = str2 + rs1 + ", " + rs2 + ", ." + func_->get_name() + trueBB->get_name();
                    output.push_back("ori " + reg.print() + ", $zero, 1");
                    output.push_back(str);
                    output.push_back(str2);
                    output.push_back("ori " + reg.print() + ", $zero, 0");
                    output.push_back("b ." + func_->get_name() + falseBB->get_name());
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                        rs1 = "$r23";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r23, $zero, " + reg.print());
                        rs1 = "$r23";
                    }
                }
                else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                    if (du->regnum == -20) {
                        output.push_back("ld.w $r24, $fp, " + std::to_string(-du->offset));
                        rs2 = "$r24";
                    }
                    else {
                        Reg reg(du->regnum);
                        output.push_back("or $r24, $zero, " + reg.print());
                        rs2 = "$r24";
                    }
                }
            }
        }
        break;
    default: break;
    }
}

void CodeGen::emitcode() {
    insnum_ = 1;
    for (int i = 0; i < 8; i++) {
        registers[i] = 0;
    }
    for (int i = 0; i < 8; i++) {
        fregisters[i] = false;
    }

    output.push_back(".globl " + func_->get_name());
    output.push_back(".type " + func_->get_name() + ", @function");
    output.push_back(func_->get_name() + ":");

    for (auto &gv1 : m->get_global_variable()) {
        auto gv = &gv1;
        auto gvVal = dynamic_cast<Value *>(gv);        
        postion *def = new(postion);
        def->bb = *(dfs_basic_blocks_.begin()); def->insnum = insnum_;

        duChain *newv = new(duChain);
        newv->leader_ = gvVal; newv->def_ = def; newv->is_arg = false;
        newv->offset = -1; newv->realoffset = -1; newv->is_global = true;
        duChain_list_.insert(duChain_list_.begin(), newv);
    }

    if (func_->get_num_of_args() <= 8) {
        int i = 0;
        for(auto &arg : func_->get_args()) {
            auto argval = dynamic_cast<Value *>(arg);
            postion *def = new(postion);
            def->bb = *(dfs_basic_blocks_.begin()); def->insnum = insnum_;

            duChain *newv = new(duChain);
            if (argval->get_type()->is_pointer_type()) {
                offset_ += 8;
            }
            else {
                offset_ += 4;
            }
            if (argval->get_type()->is_float_type()) {
                newv->regnum = i + 32;
            }
            else {
                newv->regnum = i + 4;
            }
            newv->leader_ = argval; newv->def_ = def; newv->is_arg = true;
            newv->offset = offset_; i++; newv->realoffset = -1; newv->is_global = false;
            duChain_list_.insert(duChain_list_.begin(), newv);
        }
    }
    int allocaoffset = maxfreg * 4 + maxreg * 4;
    offset_ += allocaoffset;

    while (offset_ % 16) offset_ += 4;
    
    output.push_back("addi.d $sp, $sp, " + std::to_string(-offset_));
    output.push_back("st.d $ra, $sp, " + std::to_string(offset_ - 8));
    output.push_back("st.d $fp, $sp, " + std::to_string(offset_ - 16));
    output.push_back("addi.d $fp, $sp, " + std::to_string(offset_));
    
    for (auto &bb1 : func_->get_basic_blocks()) {
        auto bb = &bb1;
        output.push_back("." + func_->get_name() + bb->get_name() + ":");
        for (auto it = bb->get_instructions().begin(); it != bb->get_instructions().end(); it++) {
            auto &instr1 = *it;
            auto instr = &instr1;
            // printf("%d %d\n", registers[4], insnum_);
            
            for (auto du : duChain_list_) {
                if (du->end == insnum_ && du->regnum >= 12) {
                    if (du->leader_->get_type()->is_float_type()) {
                        fregisters[du->regnum - 23] = false;
                    }
                    else {
                        registers[du->regnum - 12] = 0;
                    }
                }
            }

            if (instr->is_ret()) {
                auto con = dynamic_cast<Constant *>(instr->get_operand(0));
                if (con) {
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        output.push_back("ori $a0, $zero, " + con->print());
                    }
                    else if (dynamic_cast<ConstantFP *>(con)) {
                        output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                        fcon_list_.push_back(dynamic_cast<ConstantFP *>(con)); flabel_++;
                        output.push_back("fld.s $ft8, $r21, 0");
                        output.push_back("fmov.s $fa0, $ft8");
                    }
                    else {
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $a0, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $a0, $a0, " + std::to_string(num % 4096));
                    }
                }
                else {
                    auto val = dynamic_cast<Value *>(instr->get_operand(0));
                    if (val->get_type()->is_float_type()) {
                        for (auto du : duChain_list_) {
                            if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                                if (du->regnum == -20) {
                                    output.push_back("fld.s $ft9, $fp, " + std::to_string(-du->offset));
                                    output.push_back("fmov.s $fa0, $ft9");
                                }
                                else {
                                    Reg reg(du->regnum);
                                    output.push_back("fmov.s $fa0, " + reg.print());
                                }
                            }
                        }
                    }
                    else {
                        std::string str = "or $a0, $zero, ";
                        for (auto du : duChain_list_) {
                            if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                                if (du->regnum == -20) {
                                    output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                                    output.push_back(str + "$r23");
                                }
                                else {
                                    Reg reg(du->regnum);
                                    output.push_back(str + reg.print());
                                }
                            }
                        }
                    }
                }
                output.push_back("ld.d $ra, $sp, " + std::to_string(offset_ - 8));
                output.push_back("ld.d $fp, $sp, " + std::to_string(offset_ - 16));
                output.push_back("addi.d $sp, $sp, " + std::to_string(offset_));
                output.push_back("jr $ra");
            } 
            else if (instr->isBinary()) {
                binarygen(instr);
            }
            else if (instr->is_call()) {
                int offset = allocaoffset;
                for (int i = 0; i < 8; i++) {
                    Reg reg(i + 12);
                    if (registers[i] == -1) {
                        offset -= 8;
                        output.push_back("st.d " + reg.print() + ", $fp, " + std::to_string(offset - offset_));            
                    }
                    else if (registers[i]){
                        offset -= 4;
                        output.push_back("st.w " + reg.print() + ", $fp, " + std::to_string(offset - offset_));
                    }
                    if (fregisters[i]) {
                        Reg reg(i + 23);
                        offset -= 4;
                        output.push_back("fst.s " + reg.print() + ", $fp, " + std::to_string(offset - offset_));
                    }
                }
                if (func_->get_num_of_args()) {
                    for(auto &arg : func_->get_args()) {
                        auto argval = dynamic_cast<Value *>(arg);
                        for (auto du : duChain_list_) {
                            if (du->leader_ == argval) {
                                Reg reg(du->regnum);
                                if (argval->get_type()->is_pointer_type()) {
                                    output.push_back("st.d " + reg.print() + ", $fp, " + std::to_string(-du->offset));
                                }
                                else {
                                    output.push_back("st.w " + reg.print() + ", $fp, " + std::to_string(-du->offset));
                                }                  
                                break;
                            }
                        }
                    }
                }
                callgen(instr);
                offset = allocaoffset;
                if (func_->get_num_of_args()) {
                    for(auto &arg : func_->get_args()) {
                        auto argval = dynamic_cast<Value *>(arg);
                        for (auto du : duChain_list_) {
                            if (du->leader_ == argval) {
                                Reg reg(du->regnum);
                                if (argval->get_type()->is_pointer_type()) {
                                    output.push_back("ld.d " + reg.print() + ", $fp, " + std::to_string(-du->offset));
                                }
                                else {
                                    output.push_back("ld.w " + reg.print() + ", $fp, " + std::to_string(-du->offset));
                                }   
                                break;
                            }
                        }
                    }
                }
                for (int i = 0; i < 8; i++) {
                    Reg reg(i + 12);
                    if (registers[i] == -1) {
                        offset -= 8;
                        output.push_back("ld.d " + reg.print() + ", $fp, " + std::to_string(offset - offset_));
                    }
                    else if (registers[i]){
                        offset -= 4;
                        output.push_back("ld.w " + reg.print() + ", $fp, " + std::to_string(offset - offset_));
                    }
                    if (fregisters[i]) {
                        Reg reg(i + 23);
                        offset -= 4;
                        output.push_back("fld.s " + reg.print() + ", $fp, " + std::to_string(offset - offset_));
                    }
                }
            }
            else if (instr->is_cmp()) {
                while (1) {
                    auto &binstr1 = *it; auto binstr = &binstr1;
                    if (binstr->is_br()) {
                        cmpgen(instr, binstr);
                        break;
                    }
                    for (auto du : duChain_list_) {
                        if (du->leader_ == dynamic_cast<Value *>(instr)) {
                            if (du->def_->insnum == insnum_ && du->regnum >= 12 && du->regnum < 32) {
                                if (du->leader_->get_type()->is_float_type()) {
                                    fregisters[du->regnum - 23] = true;
                                }
                                else {
                                    registers[du->regnum - 12] = 1;
                                }
                            }
                            if (du->end == insnum_ && du->regnum >= 12) {
                                if (du->leader_->get_type()->is_float_type()) {
                                    fregisters[du->regnum - 23] = false;
                                }
                                else {
                                    registers[du->regnum - 12] = 0;
                                }
                            }
                        }
                    }
                    it++;
                    insnum_++;
                }
            }
            else if (instr->is_br()) {
                it++;
                if (it == bb->get_instructions().end()) {
                    for (auto sucbb : bb->get_succ_basic_blocks()) {
                        for (auto it = sucbb->get_instructions().rbegin(); it != sucbb->get_instructions().rend(); it++) {
                            auto &instr1 = *it;
                            auto instr = &instr1;
                            if (instr->is_phi()) {
                                std::string rsl, rsr;
                                auto Lval = dynamic_cast<Constant *>(instr->get_operand(0));
                                auto Rval = dynamic_cast<Constant *>(instr->get_operand(2));
                                auto Lbb = dynamic_cast<BasicBlock *>(instr->get_operand(1));
                                auto Rbb = dynamic_cast<BasicBlock *>(instr->get_operand(3));

                                for (auto du : duChain_list_) {
                                    if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                                        if (du->regnum == -20) {
                                            output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                                            rsl = "$r23";
                                        }
                                        else {
                                            Reg reg(du->regnum);
                                            rsl = reg.print();
                                        }
                                    }
                                    else if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(2))) {
                                        if (du->regnum == -20) {
                                            output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                                            rsr = "$r23";
                                        }
                                        else {
                                            Reg reg(du->regnum);
                                            rsr = reg.print();
                                        }
                                    }
                                }
                                for (auto du : duChain_list_) {
                                    if (du->leader_ == dynamic_cast<Value *>(instr)) {
                                        Reg reg(du->regnum);
                                        if (Lbb == bb) {
                                            if (Lval) {
                                                output.push_back("ori " + reg.print() + ", $zero, " + Lval->print());
                                            }
                                            else {
                                                output.push_back("or " + reg.print() + ", $zero, " + rsl);
                                            }
                                        }
                                        else if (Rbb == bb) {
                                            if (Rval) {
                                                output.push_back("ori " + reg.print() + ", $zero, " + Rval->print());
                                            }
                                            else {
                                                output.push_back("or " + reg.print() + ", $zero, " + rsr);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                it--;
                if (dynamic_cast<BasicBlock *>(instr->get_operand(0))) {
                    auto nextBB = dynamic_cast<BasicBlock *>(instr->get_operand(0));
                    output.push_back("b ." + func_->get_name() +  nextBB->get_name());
                }
                else {
                    auto trueBB = dynamic_cast<BasicBlock *>(instr->get_operand(1));
                    auto falseBB = dynamic_cast<BasicBlock *>(instr->get_operand(2));
                    if (dynamic_cast<Constant *>(instr->get_operand(0))) {
                        auto conval = dynamic_cast<ConstantInt *>(instr->get_operand(0));
                        if (conval->get_value()) {
                            output.push_back("b ." + func_->get_name() +  trueBB->get_name());
                        }
                        else {
                            output.push_back("b ." + func_->get_name() +  falseBB->get_name());
                        }
                    }
                    else {
                        for (auto du : duChain_list_) {
                            if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                                Reg reg(du->regnum);
                                output.push_back("bnez " + reg.print() + ", ." + func_->get_name() +  trueBB->get_name());
                                output.push_back("b ." + func_->get_name() +  falseBB->get_name());
                            }
                        }
                    }
                }             
            }
            else if (instr->is_gep()) {
                int base = 0, offset = 0;
                std::string rsb, rso;
                if (instr->get_num_operand() > 2) {
                    auto third = dynamic_cast<Constant *>(instr->get_operand(2));
                    if (third) {
                        auto conval = dynamic_cast<ConstantInt *>(instr->get_operand(2));
                        offset = conval->get_value() * 4; 
                    } 
                }
                else {
                    auto second = dynamic_cast<Constant *>(instr->get_operand(1));
                    if (second) {
                        auto conval = dynamic_cast<ConstantInt *>(instr->get_operand(1));
                        offset = conval->get_value() * 4; 
                    } 
                }

                for (auto du : duChain_list_) {
                    if (dynamic_cast<Value *>(instr->get_operand(0)) == du->leader_) {
                        if (du->is_global) {
                            output.push_back("la.local $r23, " + du->leader_->get_name());
                            rsb = "$r23";
                        }
                        else if (!du->is_arg) {
                            base = -du->offset;
                        }
                        else {
                            if (du->regnum == -20) {
                                output.push_back("ld.d $r23, $fp, " + std::to_string(-du->offset));
                                rsb = "$r23";
                            }
                            else {
                                Reg reg(du->regnum);
                                rsb = reg.print();  
                            }
                        }
                    }
                    if (instr->get_num_operand() > 2 && dynamic_cast<Value *>(instr->get_operand(2)) == du->leader_) {
                        if (du->regnum == -20) {
                            output.push_back("ld.d $r21, $fp, " + std::to_string(-du->offset));
                            rso = "$r21";
                        }
                        else {
                            Reg reg(du->regnum);
                            rso = reg.print();  
                        }
                    }
                    if (instr->get_num_operand() == 2 && dynamic_cast<Value *>(instr->get_operand(1)) == du->leader_) {
                        if (du->regnum == -20) {
                            output.push_back("ld.d $r21, $fp, " + std::to_string(-du->offset));
                            rso = "$r21";
                        }
                        else {
                            Reg reg(du->regnum);
                            rso = reg.print();  
                        }
                    }
                    if (dynamic_cast<Value *>(instr) == du->leader_) {
                        if (rso.size() || rsb.size()) {
                            if (du->regnum == -20) {
                                if (rso.size() && rsb.size()) {
                                    output.push_back("slli.d $r24, " + rso + ", 2");
                                    output.push_back("add.d $r24, " + rsb + ", $r24");
                                }
                                else if (rso.size()) {
                                    output.push_back("slli.d $r24, " + rso + ", 2");
                                    output.push_back("addi.d $r24, $r24, " + std::to_string(base));
                                    output.push_back("add.d $r24, $fp, $r24");
                                }
                                else if (rsb.size()) {
                                    output.push_back("addi.d $r24, " + rsb + ", "  + std::to_string(offset));
                                }
                                output.push_back("st.d $r24, $fp, " + std::to_string(offset));
                                du->realoffset = -1; 
                            }
                            else {
                                Reg reg(du->regnum);
                                registers[du->regnum - 12] = -1;
                                if (rso.size() && rsb.size()) {
                                    output.push_back("slli.d $r24, " + rso + ", 2");
                                    output.push_back("add.d " + reg.print() + ", " + rsb + ", $r24");
                                }
                                else if (rso.size()) {
                                    output.push_back("slli.d $r24, " + rso + ", 2");
                                    output.push_back("addi.d $r24, $r24, " + std::to_string(base));
                                    output.push_back("add.d " + reg.print() + ", $fp, $r24");
                                }
                                else if (rsb.size()) {
                                    output.push_back("addi.d " + reg.print() +", " + rsb + ", "  + std::to_string(offset));
                                }
                                du->realoffset = -1; 
                            }
                        }
                        else {
                            du->realoffset = base + offset;
                            if (du->regnum == -20) {
                                output.push_back("addi.d $r24, $fp, "  + std::to_string(du->realoffset));
                                output.push_back("st.d $r24, $fp, " + std::to_string(-du->offset));
                            }
                            else {
                                Reg reg(du->regnum);
                                registers[du->regnum - 12] = -1;
                                output.push_back("addi.d " + reg.print() +", $fp, "  + std::to_string(du->realoffset));
                            }
                        }
                        break;
                    }
                }         
            }
            else if (instr->is_load()) {
                int offset;
                std::string rs;
                for (auto du : duChain_list_) {
                    if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                        if (du->is_global) {
                            output.push_back("la.local $r21, " + du->leader_->get_name());
                            rs = "$r21";
                        }
                        else {
                            if (du->realoffset == -1) {
                                if (du->regnum == -20) {
                                    output.push_back("ld.w $r21, $fp, " + std::to_string(-du->offset));
                                    rs = "$r21";
                                }
                                else {
                                    Reg reg(du->regnum);
                                    rs = reg.print();
                                }
                            }
                            else {
                                offset = du->realoffset;
                            }
                        }
                        break;
                    }
                }
                for (auto du : duChain_list_) {
                    if (du->leader_ == dynamic_cast<Value *>(instr)) {
                        if (du->regnum == -20) {
                            if (rs.size()) {
                                output.push_back("ld.w $r21, " + rs + ", 0");
                                output.push_back("st.w $r21, " + rs + ", " + std::to_string(-du->offset));
                            }
                            else {
                                output.push_back("ld.w $r21, $fp, " + std::to_string(offset));
                                output.push_back("st.w $r21, " + rs + ", " + std::to_string(-du->offset));
                            }                            
                        }
                        else {
                            Reg reg(du->regnum);
                            if (rs.size()) {
                                output.push_back("ld.w " + reg.print() + ", " + rs + ", 0");
                            }
                            else {
                                output.push_back("ld.w " + reg.print() + ", $fp, " + std::to_string(offset));
                            }
                        }
                        break;
                    }
                }
            }
            else if (instr->is_store()) {
                int offset;
                std::string rs, rb;
                for (auto du : duChain_list_) {
                    if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(1))) {
                        if (du->is_global) {
                            output.push_back("la.local $r21, " + du->leader_->get_name());
                            rb = "$r21";
                        }
                        else {
                            if (du->realoffset == -1) {
                                if (du->regnum == -20) {
                                    output.push_back("ld.w $r21, $fp, " + std::to_string(-du->offset));
                                    rb = "$r21";
                                }
                                else {
                                    Reg reg(du->regnum);
                                    rb = reg.print();
                                }
                            }
                            else {
                                offset = du->realoffset;
                            }
                        }
                        break;
                    }
                }

                auto con = dynamic_cast<Constant *>(instr->get_operand(0));
                if (con) {
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        output.push_back("addi.w $r23, $zero, " + con->print());
                    }
                    else {
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $r23, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $r23, $r23, " + std::to_string(num % 4096));
                    }
                    rs = "$r23";
                }
                else {
                    for (auto du : duChain_list_) {
                        if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                            if (du->regnum == -20) {
                                output.push_back("ld.w $r23, $fp, " + std::to_string(-du->offset));
                                rs = "$r23";
                            }
                            else {
                                Reg reg(du->regnum);
                                rs = reg.print();
                            }
                            break;
                        }
                    }
                }

                if (rb.size()) {
                    output.push_back("st.w " + rs + ", " + rb + ", 0");
                }
                else {
                    output.push_back("st.w " + rs + ", $fp, " + std::to_string(offset));
                }
            }
            else if (instr->is_si2fp()) {
                std::string rs;
                if (dynamic_cast<Constant *>(instr->get_operand(0))) {
                    auto con = dynamic_cast<Constant *>(instr->get_operand(0));               
                    if (dynamic_cast<ConstantInt *>(con)->get_value() <= 2047) {
                        output.push_back("ori $r21, $zero, " + con->print());
                    }
                    else {
                        auto num = dynamic_cast<ConstantInt *>(con)->get_value();
                        output.push_back("lu12i.w $r21, " + std::to_string(num - num % 4096) + " >> 12");
                        output.push_back("ori $r21, $r21, " + std::to_string(num % 4096));
                    }
                    for (auto du : duChain_list_) {
                        if (du->leader_ == dynamic_cast<Value *>(instr)) {
                            if (du->regnum == -20) {
                                output.push_back("movgr2fr.w $ft8, $r21");
                                output.push_back("ffint.s.w $ft8, $ft8");
                                output.push_back("fst.s $ft8, $fp, " + std::to_string(-du->offset));
                            }
                            else {
                                Reg reg(du->regnum);
                                output.push_back("movgr2fr.w $ft8, $r21");
                                output.push_back("ffint.s.w " + reg.print() + ", $ft8");
                            }
                        }
                    }
                }
                else {
                    for (auto du : duChain_list_) {
                        if (du->leader_ == dynamic_cast<Value *>(instr)) {
                            if (du->regnum == -20) {
                                output.push_back("movgr2fr.w $ft8, " + rs);
                                output.push_back("ffint.s.w $ft8, $ft8");
                                output.push_back("fst.s $ft8, $fp, " + std::to_string(-du->offset));
                            }
                            else {
                                Reg reg(du->regnum);
                                output.push_back("movgr2fr.w $ft8, " + rs);
                                output.push_back("ffint.s.w " + reg.print() + ", $ft8");
                            }
                        }
                        if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                            if (du->regnum == -20) {
                                output.push_back("ld.w $r21, $fp, " + std::to_string(-du->offset));
                                rs = "$r21";
                            }
                            else {
                                Reg reg(du->regnum);
                                rs = reg.print();
                            }
                        }
                    }
                }
            }
            else if (instr->is_fp2si()) {
                std::string rs;
                if (dynamic_cast<Constant *>(instr->get_operand(0))) {
                    auto con = dynamic_cast<Constant *>(instr->get_operand(0));
                    output.push_back("la.local $r21, .LC" + std::to_string(flabel_));
                    fcon_list_.push_back(dynamic_cast<ConstantFP *>(con)); flabel_++;
                    output.push_back("fld.s $ft8, $r21, 0");
                    for (auto du : duChain_list_) {
                        if (du->leader_ == dynamic_cast<Value *>(instr)) {
                            if (du->regnum == -20) {
                                output.push_back("ftint.w.s $ft8, $ft8");
                                output.push_back("movfr2gr.s $r21, $ft8");
                                output.push_back("st.w $r21, $fp, " + std::to_string(-du->offset));
                            }
                            else {
                                Reg reg(du->regnum);
                                output.push_back("movfr2gr.s $r21, $ft8");
                                output.push_back("ftint.w.s " + reg.print() + ", $r21");
                            }
                        }
                    }
                }
                else {
                    for (auto du : duChain_list_) {
                        if (du->leader_ == dynamic_cast<Value *>(instr)) {
                            if (du->regnum == -20) {
                                output.push_back("ftint.w.s $ft8, $ft8");
                                output.push_back("movfr2gr.s $r21, $ft8");
                                output.push_back("fst.s $r21, $fp, " + std::to_string(-du->offset));
                            }
                            else {
                                Reg reg(du->regnum);
                                output.push_back("ftint.w.s $ft8, " + rs);
                                output.push_back("movfr2gr.s " + reg.print() + ", $ft8");
                            }
                        }
                        if (du->leader_ == dynamic_cast<Value *>(instr->get_operand(0))) {
                            if (du->regnum == -20) {
                                output.push_back("fld.s $ft8, $fp, " + std::to_string(-du->offset));
                                rs = "$ft8";
                            }
                            else {
                                Reg reg(du->regnum);
                                rs = reg.print();
                            }
                        }
                    }
                }
            }

            for (auto du : duChain_list_) {
                if (du->def_->insnum == insnum_ && du->regnum >= 12 && du->regnum < 32) {
                    if (du->leader_->get_type()->is_float_type()) {
                        fregisters[du->regnum - 23] = true;
                    }
                    else if (du->leader_->get_type()->is_pointer_type()) {
                        registers[du->regnum - 12] = -1;
                    }
                    else {
                        registers[du->regnum - 12] = 1;
                    }
                }
            }
            insnum_++;
        }
    }

    int i = 0;
    for (auto num : fcon_list_) {
        output.push_back(".LC" + std::to_string(i) + ":");
        i++;
        std::stringstream fp_ir_ss;
        std::string fp_ir;
        float val = (float)num->get_value();
        fp_ir_ss << "0x" << std::hex << *(uint32_t *)&val << std::endl;
        fp_ir_ss >> fp_ir;
        output.push_back(".word " + fp_ir);
    }
}

void CodeGen::initPerFunction() {
    dfs_basic_blocks_.clear();
    dfs_basic_blocks_.push_back(func_->get_entry_block()); //prepare for dfs

    duChain_list_.clear();
    active_list_.clear();
    factive_list_.clear();
    insnum_ = 1;
    offset_ = 16;
    maxreg = 0;
    maxfreg = 0;
    flabel_ = 0;
    
    for (int i = 0; i < 8; i++) {
        registers[i] = 1;
    }
    for (int i = 0; i < 8; i++) {
        fregisters[i] = true;
    }
}

void CodeGen::run() {
    // TODO: implement
    // 以下内容生成 int main() { return 0; } 的汇编代码

    output.push_back(".text");
    for (auto &gv1 : m->get_global_variable()) {
        auto gv = &gv1;
        auto gvVal = dynamic_cast<Value *>(gv);        
        output.push_back(".comm " + gvVal->get_name() + ", " + std::to_string(gvVal->get_type()->get_size()));
    }

    for (auto &func : m->get_functions()) {
        if (not func.is_declaration()) {
            func_ = &func;
            initPerFunction();            
            // dfs(func_->get_entry_block());
            // topu();
            // for (auto bb : topu_list_) {
            //     std::cout << bb->get_name() << std::endl;
            // }
            printf("dfs ready\n");
            printf("live variables ready\n");
            detectDUchain();
            // for (auto du : duChain_list_) {
            //     std::cout << du->leader_->get_name() << std::endl;
            //     std::cout << "def: " + du->def_->bb->get_name() + ", " + std::to_string(du->def_->insnum) << std::endl;
            //     for (auto use : du->use_) {
            //         std::cout << "use: " + use->bb->get_name() + ", " + std::to_string(use->insnum) << std::endl;
            //     }
            //     std::cout << "reg: " + std::to_string(du->regnum) << std::endl; 
            // }
            printf("duchain ready\n");
            LinearScan(); 
            for (auto du : duChain_list_) {
                std::cout << du->leader_->get_name() << std::endl;
                std::cout << "def: " + du->def_->bb->get_name() + ", " + std::to_string(du->def_->insnum) << std::endl;
                for (auto use : du->use_) {
                    std::cout << "use: " + use->bb->get_name() + ", " + std::to_string(use->insnum) << std::endl;
                }
                std::cout << "reg: " + std::to_string(du->regnum) << std::endl; 
                std::cout << "end: " + std::to_string(du->end) << std::endl; 
            }
            // for (auto du : duChain_list_) {
            //     std::cout << du->leader_->get_name() << std::endl;
            //     std::cout << "reg: " + std::to_string(du->regnum) << std::endl; 
            //     std::cout << "end: " + std::to_string(du->end) << std::endl; 
            //     std::cout << "arg: " + std::to_string(du->is_arg) << std::endl;
            // }
            emitcode();
            // for (auto du : duChain_list_) {
            //     std::cout << du->leader_->get_name() << std::endl;
            //     std::cout << "reg: " + std::to_string(du->regnum) << std::endl; 
            //     std::cout << "end: " + std::to_string(du->end) << std::endl; 
                // std::cout << "arg: " + std::to_string(du->is_arg) << std::endl;
                // std::cout << "offset: " + std::to_string(du->offset) << std::endl;
                // std::cout << "realoffset: " + std::to_string(du->realoffset) << std::endl;
            // }
            // for (auto du : duChain_list_) {
            //     std::cout << du->leader_->get_name() << std::endl;
            //     std::cout << "def: " + du->def_->bb->get_name() + ", " + std::to_string(du->def_->insnum) << std::endl;
            //     for (auto use : du->use_) {
            //         std::cout << "use: " + use->bb->get_name() + ", " + std::to_string(use->insnum) << std::endl;
            //     }
            // }
        }
    }
}

std::vector<duChain *> CodeGen::clone(const std::vector<duChain *> &p) {
    std::vector<duChain *> data;
    for (auto cc : p) {
        data.push_back(cc);
    }
    return data;
}