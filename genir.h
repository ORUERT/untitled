#pragma once
#include "common.h"
/*
 * 中间代码生成器
 */

class GenIR {
    static int lbNum;//标签好嘛，用于生成唯一的标签

    SymTab &symtab;//符号表

    //break continue辅助标签列表

    //函数调用

    //双目运算

    //单目运算

public:
    GenIR(SymTab & tab);

    Var* genAssign(Var*lval,Var*rval);
    Var* genAssign(Var*val);

    bool genVarInit(Var* var);
    //产生符号和语句
    Var* genOneOpRight(Var* val,Tag opt);
    Var* genPtr(Var*val);
    Var* genAdd(Var*lval,var*rval);
    Var* genArray(Var*array,Var*index);
    Var* genTwoOp(Var*lval,Tag opt,Var*rval);

    void genFunHead(Fun*function);
    void genFunTail(Fun*function);
    void genReturn(Var*ret);
    Var* genPtr(Var*val);
    Var* genLea(Var*val);

};

