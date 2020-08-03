#include "CodeGenVisitor.h"

std::string CodeGenVisitor::genCode(
        const std::shared_ptr<ASTNode> &op,
        const VarAllocVisitor::Map<VarAllocVisitor::Map<int>> &varMap) {
    varMap_ = &varMap;
    (*this)(op);
    return os.str();
}

void CodeGenVisitor::visit(const ProgramNode *op) {
    os << ".global main\n";
    Visitor::visit(op);
}

void CodeGenVisitor::visit(const FunctionNode *op) {
    curFunc_ = op->name_;
    os << op->name_ << ":\n";
    os << "sd fp, -8(sp)\n"
          "mv fp, sp\n";
    for (size_t i = 0, n = op->args_.size(); i < n; i++) {
        // copy args as new vars
        auto offset = varMap_->at(curFunc_).at(op->args_[i]->name_);
        os << "ld t0, " << (8 * i) << "(fp)\n"
              "sd t0, " << (-16 - 8 * offset) << "(fp)  # Store to " << op->args_[i]->name_ << "\n";
    }
    (*this)(op->body_);
    os << "mv sp, fp\n"
          "ld fp, -8(sp)\n"
          "ret\n";
}

void CodeGenVisitor::visit(const VarNode *op) {
    Visitor::visit(op);
    auto offset = varMap_->at(curFunc_).at(op->name_);
    os << "ld a0, " << (-16 - 8 * offset) << "(fp)  # Load from " << op->name_ << "\n" << push;
}

void CodeGenVisitor::visit(const AssignNode *op) {
    os << "addi sp, fp, " << (-8 - 8 * (int)varMap_->at(curFunc_).size()) << "\n";
    (*this)(op->expr_);
    auto offset = varMap_->at(curFunc_).at(op->var_->name_);
    os << "sd a0, " << (-16 - 8 * offset) << "(fp)  # Store to " << op->var_->name_ << "\n";
}

void CodeGenVisitor::visit(const InvokeNode *op) {
    os << "addi sp, fp, " << (-8 - 8 * (int)varMap_->at(curFunc_).size()) << "\n";
    Visitor::visit(op);
}

void CodeGenVisitor::visit(const IfThenElseNode *op) {
    (*this)(op->cond_);
    if (op->elseCase_ == nullptr) {
        auto elseTarget = jumpCnt_++;
        os << "beqz a0, " << elseTarget << "f\n";
        (*this)(op->thenCase_);
        os << elseTarget << ":\n";
    } else {
        auto elseTarget = jumpCnt_++;
        auto endTarget = jumpCnt_++;
        os << "beqz a0, " << elseTarget << "f\n";
        (*this)(op->thenCase_);
        os << "j " << endTarget << "f\n";
        os << elseTarget << ":\n";
        (*this)(op->elseCase_);
        os << endTarget << ":\n";
    }
}

void CodeGenVisitor::visit(const WhileNode *op) {
    auto beginTarget = jumpCnt_++;
    auto endTarget = jumpCnt_++;
    os << beginTarget << ":\n";
    (*this)(op->cond_);
    os << "beqz a0, " << endTarget << "f\n";
    (*this)(op->body_);
    os << "j " << beginTarget << "b\n";
    os << endTarget << ":\n";
}

void CodeGenVisitor::visit(const IntegerNode *op) {
    Visitor::visit(op);
    os << "ori a0, x0, " << op->literal_ << "\n" << push;
}

void CodeGenVisitor::visit(const CallNode *op) {
    os << "sd ra, -8(sp)\n"
          "sd t0, -16(sp)\n"
          "sd t1, -24(sp)\n"
          "addi sp, sp, -24\n";
    for (size_t i = op->args_.size() - 1; ~i; i--) {
        (*this)(op->args_[i]);  // results are saved to stack
    }
    os << "call "  << op->callee_ << "\n";
    os << "addi sp, sp, " << (24 + 8 * op->args_.size()) << "\n"
          "ld ra, -8(sp)\n"
          "ld t0, -16(sp)\n"
          "ld t1, -24(sp)\n";
    os << push;
}

void CodeGenVisitor::visit(const AddNode *op) {
    Visitor::visit(op);
    os << pop2 << "add a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const SubNode *op) {
    Visitor::visit(op);
    os << pop2 << "sub a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const MulNode *op) {
    Visitor::visit(op);
    os << pop2 << "mul a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const DivNode *op) {
    Visitor::visit(op);
    os << pop2 << "div a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const LTNode *op) {
    Visitor::visit(op);
    os << pop2 << "slt a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const LENode *op) {
    Visitor::visit(op);
    os << pop2 << "sgt a0, t0, t1\n"
                  "xori a0, a0, 1\n" << push;
}

void CodeGenVisitor::visit(const GTNode *op) {
    Visitor::visit(op);
    os << pop2 << "sgt a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const GENode *op) {
    Visitor::visit(op);
    os << pop2 << "slt a0, t0, t1\n"
                  "xori a0, a0, 1\n" << push;
}

void CodeGenVisitor::visit(const EQNode *op) {
    Visitor::visit(op);
    os << pop2 << "sub t0, t0, t1\n"
                  "seqz a0, t0\n" << push;
}

void CodeGenVisitor::visit(const NENode *op) {
    Visitor::visit(op);
    os << pop2 << "sub t0, t0, t1\n"
                  "snez a0, t0\n" << push;
}

