//
// Created by oruret on 2018/6/4.
//
#include "symbol.h"
#include "symtab.h"
#include "error.h"

//打印语义错误
#define SEMERROR(code,name) Error::semError(code,name)

Var* SymTab::voidVar=NULL;

vector<int>& SymTab::getScopePath(){
    return scopePath;
}

SymTab::SymTab() {
    voidVar= new Var();

    scopeId = 0;
    curFun = NULL;
    scopePath.push_back(0);
}

SymTab::~SymTab() {
    //清除函数，保证变量信息在指令清除时可用
    hash_map<string,Fun*,string_hash>::iterator funIt,funEnd = funTab.end();
    for(funIt = funTab.begin();funIt!=funEnd;++funIt){
        delete funIt->second;
    }
}

void SymTab::addVar(Var* var){
    if(varTab.find(var->getName())==varTab.end()){//没有该名字的变量
        varTab[var->getName()] = new vector<Var*>;//创建链表
        varTab[var->getNmae()]->push_back(var);//添加变量
        varList.push_back(var->getName());
    }else {
        //判断同名变量是否都不在一个作用域
        vector<Var*>&list = *varTab[var->getName()];
        int i;
        for(i = 0 ; i < list.size() ; i ++){
            if(list[i]->getPath().back() == var->getPath().back())//在一个作用域
                break;
        }
        if(i == list.size()||var->getName()[0]=='<')
            list.push_back(var);
        else {
            //同一作用域存在同名变量定义，extern时声明外部文件的变量，相当于定义了该全局变量
            SEMERROR(VAR_RE_DEF,var->getName());
            delete var;
            return;//无效变量，删除
        }
    }

    if(ir){
        int flag = ir->genVarInit(var);
        if(curFun&&flag)curFun->locate(var);
    }
}

void SymTab::addStr(Var *v) {
    hash_map<string,Var*,string_hash>::iterator strIt,strEnd = strTab.end();
    for(strIt = strTab.begin();strIt!=strEnd;++strIt){
        Var*str = strIt->second;
        if(v->getStrVal() == str->getStrVal()){
            delete v;
            v = str;
            return ;
        }
    }
    strTab[v->getName()] = v;
}



void SymTab::enter(){
    scopeId++;
    scopePath.push_back(scopeId);
    if(curFun)curFun->enterScope();
}

void SymTab::leave(){
    scopePath.pop_back();
    if(curFun)curFun->leaveScope();
}

Var* SymTab::getVar(string name) {
    Var* select = NULL;
    if(varTab.find(name)!=varTab.end()){
        vector<Var*>&list = *varTab[name];
        int pathLen = scopePath.size();
        int maxLen = 0;
        for(int i = 0 ; i < list.size(); i ++){
            int len = list[i]->getPath().size();
            if(len<=pathLen&&list[i]->getPath()[len-1] == scopePath[len-1]){
                if(len>maxLen){
                    maxLen = len;
                    select = list[i];
                }
            }
        }
    }
    if(!select)
        SEMERROR(VAR_UN_DEC,name);
    return select;
}


void SymTab::decFun(Fun *fun) {
    fun->setExtern(true);
    if(funTba.find(fun->getName())==funTab.end()){
        funTab[fun->getName()] = fun;
    }else {
        Fun* last = funtab[fun->getName()];
        if(!last->match(fun)){
            SEMERROR(FUN_DEC_ERR,fun->getName);
        }
        delete fun;
    }
}

void SymTab::defFun(Fun *fun) {
    if(fun->getExtern()){
        SEMERROR(EXTERN_FUN_DEF,fun->getName());
        fun->setExtern(false);
    }
    if(funTab.find(fun->getName()) == funTab.end()){
        funTab[fun->getName()] = fun;
        funList.push_back(fun->getName());
    }else {
        Fun*list = funTab[fun->getName()];
        if(last->getExtern()){
            if(!last->match(fun)){
                SEMERROR(FUN_DEC_ERR,fun->getName());
            }
            last->define(fun);
        }else {
            SEMERROR(FUN_RE_DEF,fun->getName());
        }
        delete fun;
        fun = last;
    }
    curFun = fun;
    ir->genFunHead(curFun);
}

void SymTab::endDefFun() {
    ir->genFunTail(curFun);
    curFun = NULL;
}

Fun* SymTab::getFun(string name,vector<Var*>& args){
    if(funTab.find(name)!=funTab.end()){
        Fun*last = funTab[name];
        if(!last->match(args)){
            SEMERROR(FUN_CALL_ERR,name);
            return NULL;
        }
        return last;
    }
    SEMERROR(FUN_UN_DEC,name);
    return NULL;
}
/*
 * 添加一条中间代码
 */

void SymTab::addInst(InterInst *inst) {
    if(curFun)curFun->addInst(inst);
    else delete inst;
}

Fun* SymTab::getCurFun() {
    return curFun;
}