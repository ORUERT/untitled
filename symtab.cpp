//
// Created by oruret on 2018/6/4.
//
#include "symbol.h"
#include "symtab.h"
#include "error.h"

//打印语义错误
#define SEMERROR(code,name) Error::semError(code,name)


vector<int>& SymTab::getScopePath(){
    return scopePath;
}

SymTab::SymTab() {

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
}

void SymTab::enter(){
    scopeId++;
    scopePath.push_back(scopeId);
    if(curFun)curFun->enterScope();
}