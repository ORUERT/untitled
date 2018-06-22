//
// Created by oruret on 2018/6/4.
//
#pragma once
#include "common.h"
#include "intercode.h"
#include "set.h"

/*******************************************************************************
                                   变量结构体
*******************************************************************************/

class Var{
    //特殊标记
    bool literal;//是否字面量
    vector<int>scopePath;

    //基本声明形式
    bool externed;//extern声明或定义
    Tag type;//变量类型
    string name;//变量名称
    bool isPtr;//是否是指针
    bool isArray;//是否是数组
    int arraySize;//数组长度

    //初始值部分
    bool isLeft;//是否可以作为左值
    Var* initData;//缓存初始数据，延迟处置处理
    bool inited;//是否初始化数据，字面量一定是初始化了的特殊变量

    union{
        int intVal;
        char charVal;
    };
    string ptrVal;//初始化字符指针常量字符串名称？？？
    string strVal;//字符串敞亮的值
    Var*ptr;//指向当前变量指针变量

    //附加信息
    int size;//变量大小
    int offset;//局部变量，参数变量的栈帧偏移，默认值0为无效之--表示全局变量

    //内部使用函数
    void setExtern(bool ext);//设置extern
    void setType(Tag t);
    void setPtr(bool ptr);
    void setName(string n);
    void setArray(int len);
    void clear();

public:
    //特殊变量1

    //构造函数
    Var(vector<int>&sp,bool ext,Tag t,bool ptr,string name,Var*init = NULL);//变量
    Var(vector<int>&sp,bool ext,Tag t,string name,int len);//数组
    Var(Token*lt);//设定字面量
    Var(int val);//整数变量
    Var(vector<int>&sp,Tag t,bool ptr);//临时变量
    Var(vector<int>&sp,Var*v);//拷贝变量
    Var();

    //外界调用接口
    string getName();
    vector<int>& getPath();
};

/*******************************************************************************
                                   函数结构
*******************************************************************************/

class Fun{
    //基本信息
    bool externed;//声明或者定义
    Tag type;//变量类型
    string name;//变量名称
    vector<Var*>paraVar;//形参变量列表

    //临时变量地址分配

public:
    //构造函数与析构函数
    Fun(bool ext,Tag t ,string n,vector<Var*>&paraList);
    ~Fun();

    //作用域管理，局部变量地址计算
    void enterScope();//进入一个新的作用域
    void leaveScope();//离开当前作用域

};