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
    void addVar(Var* v);
};