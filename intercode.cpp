//
// Created by oruret on 2018/6/29.
//

#include "intercode.h"

void InterInst::init() {
    op = OP_NOP;
    this->result = NULL;
    this->target = NULL;
    this->arg1 = NULL;
    this->fun = NULL;
    this->arg2 = NULL;
    first = false;
    isDead = false;
}

/*
 * 一般运算指令
 */
InterInst::InterInst(Operator op,Var*rs,Var*arg1,Var*arg2){
    init();
    this->op = op;
    this->result = rs;
    this->arg1 = arg1;
    this->arg2 = arg2;
}
/*
 * 函数调用指令
 */
InterInst::InterInst(Operator op, Fun *fun, Var *rs) {
    init();
    this->op = op;
    this->result = rs;
    this->fun = fun;
    this->arg2 = NULL;
}

/*
 * 参数进栈指令
 */
InterInst::InterInst(Operator op, Var *arg1) {
    init();
    this->op = op;
    this->result = NULL;
    this->arg1 = arg1;
    this->arg2 = NULL;
}

/*
 * 产生唯一标号
 */
InterInst::InterInst() {
    init();
    label = GenIR::genLb();
}

/*
 * 条件跳转指令
 */

InterInst::InterInst(Operator op, InterInst *tar, Var *arg1, Var *arg2) {
    init();
    this->op = op;
    this->target = tar;
    this->arg2 = arg2;
    this->arg1 = arg1;
}