#pragma once
#include "common.h"
/*
 * 中间代码生成器
 */

class GenIR {
    static int lbNum;//标签好嘛，用于生成唯一的标签

    SymTab &symtab;//符号表

    //break continue辅助标签列表
    vector<InterInst*>heads;
    vector<InterInst*>tails;
    void push(InterInst*head,InterInst*tail);
    void pop();

    //函数调用
    void genPara(Var*arg);//参数传递
    //双目运算
    Var* genMul(Var*lval,Var*rval);
    Var* genAdd(Var* lval,Var* rval);
    Var* genSub(Var*lval,Var*rval);
    Var* genOr(Var*lval,Var*rval);
    Var* genAnd(Var*lval,Var*rval);
    Var* genEqu(Var*lval,Var*rval);
    Var* genNequ(Var*lval,Var*rval);
    Var* genGt(Var*lval,Var*rval);
    Var* genGe(Var*lval,Var*rval);
    Var* genLt(Var*lval,Var*rval);
    Var* genLe(Var*lval,Var*rval);
    Var* genDiv(Var*lval,Var*rval);
    Var* genMod(Var*lval,Var*rval);
    //单目运算
    Var* genIncL(Var*val);
    Var* genDecL(Var*val);
    Var* genNot(Var*val);
    Var* genMinus(Var*val);
    Var* genLea(Var*val);
    Var* genPtr(Var*val);
public:
    GenIR(SymTab & tab);

    Var* genAssign(Var*lval,Var*rval);
    Var* genAssign(Var*val);

    bool genVarInit(Var* var);
    //产生符号和语句
    Var* genArray(Var*array,Var*index);
    Var* genTwoOp(Var*lval,Tag opt,Var*rval);
    Var* genOneOpRight(Var* val,Tag opt);
    Var* genOneOpLeft(Var*val,Tag opt);
    Var* genCall(Fun*function,vector<Var*> & args);

    //产生复合语句
    void genWhileHead(InterInst*& _while,InterInst*& _exit);//创建_while和_exit，为break和continue语句代码提供跳转标签
    void genWhileCond(Var*cond,InterInst* _exit);//处理循环条件，空条件,生成OP_JF指令
    void genWhileTail(InterInst*& _while,InterInst*& _exit);//产生OP_JMP指令并添加_exit


    void genFunHead(Fun*function);
    void genFunTail(Fun*function);
    void genReturn(Var*ret);



};

