//
// Created by oruret on 2018/6/28.
//

#include "genir.h"
#include "symbol.h"
#include <sstream>
#include "symtab.h"
#include "error.h"

//打印语义错误
#define SEMERROR(code) Error::semError(code)

//int GenIR::lbNum = 0;

GenIR::GenIR(SymTab &tab):symtab(tab) {
    symtab.setIr(this);
    lbNum =0 ;
    push(NULL,NULL);//初始化作用域
}

bool GenIR::genVarInit(Var *var) {
    if(var->getName()[0] == '<')return false;//处理整型常量和字符串常量
    symtab.addInst(new InterInst(OP_DEC,var));
    if(var->setInit())//处理常量以及语法错误
        genTwoOp(var,ASSIGN,var->getInitData());//合并操作
    return true;
}

/*
 * 右单目运算语句
 */

Var* GenIR::genOneOpRight(Var*val,Tag opt){
    if(!val)return NULL;
    if(val->isVoid()){
        SEMERROR(EXPR_IS_VOID);
        return NULL;
    }
    if(!val->getLeft()){
        SEMERROR(EXPR_NOT_LEFT_VAL);
        return val;
    }
    if(opt == INC)return genIncR(val);
    if(opt == DEC)return genDecR(val);
    return val;
}

Var* GenIR::genArray(Var*array,Var*index){
    if(!array||!index)return NULL;
    if(array->isVoid||index->isVoid){
        SEMERROR(EXPR_IS_VOID);
        return NULL;
    }
    if(array->isBase()||!index->isBase()){
        SEMERROR(ARR_TYPE_ERR);
        return index;
    }
    return genPtr(genAdd(array,index));
}

Var* GenIR::genAdd(Var*lval, var*rval) {
    Var*tmp =NULL;
    //指针和数组只能和基本类型相加
    if((lval->getArray()||lval->getPtr())&&rval->isBase()){
        tmp = new Var(symtab.getScopePath(),lval);
        rval = genMul(rval,Var::getStep(lval));
    }else if(rval->isBase()&&(rval->getArray()||rval->getPtr())){
        tmp = new Var(symtab.getScopePath(),rval);
        lval = genMul(lval,Var::getStep(rval));
    }else if(lval->isBase()&&rval->isBase()){
        tmp = new Var(symtab.getScopePath(),KW_INT,false);
    }else {
        SEMERROR(EXPR_NOT_BASE);
        return lval;
    }

    //加法命令
    symtab.addVar(tmp);
    symtab.addInst(new InterInst((OP_ADD),tmp,lval,rval));//中间代码tmp=lval+rval
    return tmp;
}

Var* GenIR::genTwoOp(Var *lval, Tag opt, Var *rval) {
    if(opt == ASSIGN){
        fprintf(file,"mov eax,[%s]\n",rval->getName().c_str());
        fprintf(file,"mov [%s],eax\n",lval->getName().c_str());
        return lval;
    }
}

void GenIR::genFunHead(Fun *function) {
    function->enterScope();
    symtab.addInst(new InterInst(OP_ENTRY,function));
    function->setReturnPoint(new InterInst);
}

void GenIR::genFunTail(Fun *function) {
    symtab.addInst(function->getReturnPoint());
    symtab.addInst(new InterInst(OP_EXIT,function));
    function->leaveScope();
}

void GenIR::genReturn(Var *ret) {
    if(!ret)return;
    Fun*fun = symtab.getCurFun();
    if(ret->isVoid()&&fun->getType()!=KW_VOID||ret->isBase()&&fun->getType()==KW_VOID){
        SEMERROR(RETURN_ERR);
        return ;
    }
    InterInst* returnPoint=fun->getReturnPoint();
    if(ret->isVoid()){
        symtab.addInst(new InterInst(OP_RET,returnPoint));
    }else {
        if(ret->isRef())ret=genAssign(ret);
        symtab.addInst(new InterInst(OP_RETV,returnPoint,ret));
    }
}

bool GenIR::typeCheck(Var*lval,Var*rval){
    bool flag= false;
    if(!rval)return false;
    if(lval->isBase() && rval->isBase())
        flag = true;
    else if(!lval->isBase()&&!rval->isBase())
        flag = rval->getType() == lval->getType();
    return flag;
}

/*
 * 赋值语句
 */

Var* GenIR::genAssign(Var *lval, Var *rval) {
    //被赋值对象必须是左值
    if(!lval->getLeft()){
        SEMERROR(EXPR_NOT_LEFT_VAL);
        return rval;
    }
    //类型检查
    if(!typeCheck(lval,rval)){
        SEMERROR(ASSIGN_TYPE_ERR);
        return rval;
    }
    //考虑右值
    if(rval->isRef()){
        if(!lval->isRef()){
            //中间代码lval=*(rval->ptr)
            symtab.addInst(new InterInst(OP_GET,lval,rval->getPointer()));//?
            return lval;
        }else {
            //中间代码*（lval->ptr)=*(rval->ptr)，先处理右值
            rval=genAssign(rval);
        }
    }
    //赋值运算
    if(lval->isRef()){
        //中间代码*(lval->ptr)=rval
        symtab.addInst(new InterInst(OP_SET,rval,lval->getPointer()));
    }else {
        //中间代码lval=rval
        symtab.addInst(new InterInst(OP_AS,lval,rval));
    }
    return lval;
}

/*
 * 拷贝赋值语句,处理取*p的情况
 */

Var* GenIR::genAssign(Var*val){
    Var*tmp = new Var(symtab.getScopePath(),val);//拷贝变量信息
    symtab.addVar(tmp);
    if(val->isRef()){
        //中间代码tmp = *(val->ptr)
        symtab.addInst(new InterInst(OP_GET,tmp,val->getPointer()));
    }else
        symtab.addInst(new InterInst(OP_AS,tmp,val));//中间代码tmp=val
    return tmp;
}

Var* GenIR::genPtr(Var *val) {
    if(val->isBase()){
        SEMERROR(EXPR_IS_BASE);
        return val;
    }
    Var*tmp = new Var(symtab.getScopePath(),val->getType(),false);
    tmp->setLeft(true);
    tmp->setPointer(val);
    symtab.addVar(tmp);
    return tmp;
}

Var* GenIR::genLea(Var *val) {
    if(!val->getLeft()){
        SEMERROR(EXPR_NOT_LEFT_VAL);
        return val;
    }
    if(val->isRef()){
        return val->getPointer();
    }else {
        Var* tmp = new Var(symtab.getScopePath(),val->getType(),true);
        symtab.addVar(tmp);
        symtab.addInst(new InterInst(OP_LEA,tmp.val));
        return tmp;
    }
}