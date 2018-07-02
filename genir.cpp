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
    //++--
    if(opt == INC)return genIncR(val);
    if(opt == DEC)return genDecR(val);
    return val;
}

Var* GenIR::genOneOpLeft(Var*val,Tag opt){
    if(!val)return NULL;
    if(val->isVoid()){
        SEMERROR(EXPR_IS_VOID);
        return NULL;
    }
    //&x,*p
    if(opt == LEA)return genLea(VAL);
    if(opt == MUL)return genPtr(val);
    //++--
    if(opt == INC)return genIncL(val);
    if(opt == DEC)return genDecL(val);
    //not minus
    if(val->isRef())val = genAssign(val);
    if(opt == NOT)return genNot(val);
    if(opt == SUB)return genMinus(val);
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
    if(!val || !rval)return NULL;
    if(lval->isVoid()||rval->isVoid()){
        SEMERROR(EXPR_IS_VOID);
        return NULL;
    }
    if(opt == ASSIGN){
//        fprintf(file,"mov eax,[%s]\n",rval->getName().c_str());
//        fprintf(file,"mov [%s],eax\n",lval->getName().c_str());
//        return lval;
        return genAssign(lval,rval);
    }//赋值
    if(lval->isRef())lval = genAssign(lval);//处理左值
    if(rval->isRef())rval = genAssign(rval);//处理右值
    if(opt == OR)return genOr(lval,rval);//or
    if(opt == AND)return genAnd(lval,rval);//and
    if(opt == EQU)return genEqu(lval,rval);//equ
    if(opt == NEQU)return genNequ(lval,rval);//nequ
    if(opt == ADD)return genAdd(lval,rval);//add
    if(opt == SUB)return genSub(lval,rval);//sub
    if(!lval->isBase()||!rval->isBase()){
        SEMERROR(EXPR_NOT_BASE);
        return lval;
    }

    if(opt == GT)return genGt(lval,rval);
    if(opt == GE)return genGe(lval,rval);
    if(opt == LT)return genLt(lval,rval);
    if(opt == LE)return genLe(lval,rval);
    if(opt == MUL)return genMul(lval,rval);
    if(opt == DIV)return genDiv(lval,rval);
    if(opt == MOD)return genMod(lval,rval);
    return lval;
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


Var* GenIR::genAdd(Var *lval, Var *rval) {
    Var* tmp = NULL;
    if(!lval->isBase()&&rval->isBase()){
        temp = new Var(symtab.getScopePath(),lval);
        rval = genMul(rval,Var::getStep(lval));
    }else if(lval->isBase()&&!rval->isBase()){
        temp = new Var(symtab.getScopePath(),rval);
        lval = genMul(lval,Var::getStep(rval));
    }
}


Var* GenIR::genSub(Var *lval, Var *rval) {
    Var*tmp = NULL;
    if(!rval->isBase()){
        SEMERROR(EXPR_NOT_BASE);
        return lval;
    }
    if(!lval->isBase()){
        tmp = new Var(symtab.getScopePath(),lval);
        rval = genMul(rval,Var::getStep(lval));
    }else {
        tmp = new Var(symtab.getScopePath(),KW_INT,false);
    }
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_SUB,tmp,lval,rval));
    return tmp;
}

Var* GenIR::genMul(Var *lval, Var *rval) {
    Var* tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_MUL,tmp.lval,rval));
    return tmp;
}

Var* GenIR::genIncL(Var *val) {
    if(!val->getLeft()){
        SEMERROR(EXPR_NOT_LEFT_VAL);
        return val;
    }
    if(val->isRef()){ //++*p
        Var* t1 = genAssign(val); //t1 = *p
        Var* t2 = genAdd(t1,Var::getStep(val)); //t2 = t1 +1
        return genAssign(val,t2); // *p = t2
    }
    symtab.addInst(new InterInst(OP_ADD,val,val,Var::getStep(val)));
    return val;
}

Var* GenIR::genDecL(Var *val) {
    if(!val->getLeft()){
        SEMERROR(EXPR_NOT_LEFT_VAL);
        return val;
    }
    if(val->isRef()){
        Var* t1 = genAssign(val);
        Var* t2 = genSub(t1,Var::getStep(val));
        return genAssign(val,t2);
    }
    symtab.addInst(new InterInst(OP_SUB,val,val,Var::getStep(val)));
    return val;
}

Var* GenIR::genIncR(Var *val){
    Var*tmp = genAssign(val);
    symtab.addInst(new InterInst(OP_ADD,val,val,Var::getStep(val)));
    return tmp;
}
Var* GenIR::genDecR(Var*val)
{
    Var*tmp=genAssign(val);//拷贝
    symtab.addInst(new InterInst(OP_SUB,val,val,Var::getStep(val)));//val--
    return tmp;
}

Var* GenIR::genMinus(Var *val) {
    if(!val->isBase()){
        SEMERROR(EXPR_NOT_BASE);
        return val;
    }
    Var *tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_NEG,tmp,val));
    return tmp;
}

Var* GenIR::genOr(Var *lval, Var *rval) {
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_OR,tmp,lval,rval));
    return tmp;
}
Var* GenIR::genAdd(Var *lval, Var *rval) {
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_AND,tmp,lval,rval));
    return tmp;
}
Var* GenIR::genGt(Var*lval,Var*rval){
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_GT,tmp,lval,rval));
    return tmp;
}
Var* GenIR::genGe(Var*lval,Var*rval){
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_GE,tmp,lval,rval));
    return tmp;
}
Var* GenIR::genLt(Var *lval, Var *rval) {
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_LT,tmp,lval,rval));
    return tmp;
}
Var* GenIR::genLe(Var *lval, Var *rval) {
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_LE,tmp,lval,rval));
    return tmp;
}
Var* GenIR::genEqu(Var *lval, Var *rval) {
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_EQU,tmp,lval,rval));//中间代码tmp=lval==rval
    return tmp;
}
Var* GenIR::genNequ(Var *lval, Var *rval) {
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_NE,tmp,lval,rval));
    return tmp;
}
Var* GenIR::genDiv(Var *lval, Var *rval) {
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_DIV,tmp,lval,rval));
    return tmp;
}

Var* GenIR::genMod(Var *lval, Var *rval) {
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_MOD,tmp,lval,rval));
    return tmp;
}
//单目
Var* GenIR::genNot(Var *val) {
    Var*tmp = new Var(symtab.getScopePath(),KW_INT,false);
    symtab.addVar(tmp);
    symtab.addInst(new InterInst(OP_NOT,tmp,val));
    return tmp;
}


//实际参数传递
void GenIR::genPara(Var *arg) {
    if(arg->isRef())arg= genAssign(arg);
    //不可以赋值参数或者传递引用，需要直接无条件复制参数
    InterInst*argInst = new InterInst(OP_ARG,arg);//push arg
    symtab.addInst(argInst);
}

Var* GenIR::genCall(Fun *function, vector<Var *> &args) {
    if(!function)return NULL;
    for(int i =  args.size()-1; i >=0 ; i --){
        genPara(args[i]);
    }
    if(function->getType() == KW_VOID){
        symtab.addInst(new InterInst(OP_PROC,function));
        return Var::getVoid();
    }else {
        Var*ret = new Var(symtab.getScopePath(),function->getType(),false);
        symtab.addInst(new InterInst(OP_CALl,funtion,ret));
        symtab.addVar(ret);
        return ret;
    }
}


void GenIR::genWhileHead(InterInst *&_while, InterInst *&_exit) {
    _while = new InterInst();
    symtab.addInst(_while);
    _exit = new InterInst();
    push(_while,_exit);
}

void GenIR::genWhileCond(Var *cond, InterInst *_exit) {
    if(cond){
        if(cond->isVoid())cond = Var::getTrue();
        else if(cond->isRef())cond = genAssign(cond);
        symtab.addInst(new InterInst(OP_JF,_exit,cond));
    }
}