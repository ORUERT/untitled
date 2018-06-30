//
// Created by oruret on 2018/6/4.
//
#pragma once
#include "common.h"
#include <ext/hash_map>

using namespace __gnu_cxx;
/*
 * 符号表
 */

class SymTab{
    //声明顺序记录
    vector<string>varList;//记录变量的添加顺序
    vector<string>funList;//记录函数的添加顺序

    //hash函数
    struct string_hash{
        size_t operator()(const string& str)const{
            return __stl_hash_string(str.c_str());
        }
    };
    //内部数据结构

    //变量表,每个元素是同名变量的链表
    hash_map<string,vector< Var* >* , string_hash>varTab;
    //字符串常量表
    hash_map<string,Var*,string_hash>strTab;
    //函数表，去除函数重载特性
    hash_map<string,Fun*,string_hash>funTab;

    //辅助分析数据记录
    Fun*curFun;//分析当前的函数
    int scopeId;//作用域唯一编号
    vector<int>scopePath;//动态记录作用域的路径，全局为0,0 1 2-第一个函数的第一个局部块

public:
    SymTab();
    ~SymTab();

    void enter();//进入局部作用域
    void leave();//离开局部作用域

    void addVar(Var* v);//添加一个变量
    void addStr(Var* v);//添加s一个字符串常量
    Var* getVar(string name);

    //函数管理
    void decFun(Fun*fun);
    void defFun(Fun*fun);
    void endDefFun();
    Fun* getFun(string name,vector<Var*>&args);
    void addInst(InterInst*inst);

    //外部调用函数
    void setIr(GenIR*ir);
    vector<int>& getScopePath();//获取scopePath
    Fun*getCurFun();//获取当前分析的函数

};