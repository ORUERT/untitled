#pragma once
#include "common.h"

class InterInst{
private:
    string label; //标签
    Operator op; //操作符
    Var *result;//运算结果
    InterInst*target;//跳转符号
    Var*arg1;//参数1
    Fun*fun;//函数
    Var *arg2;//参数2

    bool first;//是否是首指令
    void init();//初始化

public:
    //构造
    InterInst (Operator op,Var *rs,Var *arg1,Var *arg2 = NULL);//一般运算指令
    InterInst (Operator op,Fun* fun,Var *rs=NULL);//函数调用指令，ENTRY,EXIT
    InterInst (Operator op,Var *arg1=NULL);//参数进展指令，NOP
    InterInst ();//产生唯一编号
    InterInst (Operator op,InterInst *tar,Var *arg1 = NULL,Var *arg2=NULL);//条件跳转指令，return

};

/*
 * 中间代码
 */

class InterCode{
    vector<InterInst*>code;

public:
    //内存管理
    ~InterCode();

    //管理操作
    void addInst(InterInst*inst);//添加一条中间代码

    //关键操作
    void markFirst();//标志“首指令”

    //外部调用接口
    void toString();
    vector<InterInst*>&getCode();//获取中间代码序列
};