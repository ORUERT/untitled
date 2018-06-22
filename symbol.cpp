//
// Created by oruret on 2018/6/4.
//

#include "symbol.h"
#include "error.h"
#include "token.h"
#include "symtab.h"
//#include "

//打印语义错误
#define SEMERROR(code,name) Error::semError(code,name);


//void变量
Var::Var() {
    clear();
    setName("<void>");//特殊变量名字
//    setLeft(false);
//    intVal = 0;//记录数字数值
//    literal = false;//hack类型

}

//临时变量
Var::Var(vector<int>&sp,Tag t ,bool ptr) {
    clear();
    scopePath = sp;
    setType(t);
    setPtr(ptr);
    setName("");
    setLeft(false);
}

//拷贝出一个临时变量
Var::Var(vector<int>&sp,Var *v) {
    clear();
    scopePath = sp;
    setType(v->type);
    setPtr(v->isPtr||v->isArray);
    setName("";)
    setLeft(false);
}

//变量，指针
Var::Var(vector<int> &sp, bool ext, Tag t, bool ptr, string name, Var *init) {
    clear();
    scopePath=sp;
    setExtern(ext);
    setType(t);
    setPtr(ptr);
    setName(name);
    initData = init;
}

//数组
Var::Var(vector<int>&sp,bool ext,Tag t,string name ,int len){
    clear();
    scopePath = sp;
    setExtern(ext);
    setType(t);
    setName(name);
    setArray(len);
}

//整数变量
Var::Var(int val) {
    clear();
    setName("<int>");
    literal = true;
    setLeft(false);
    setType(KW_INT);
    intVal = val;
}

//常量，不涉及作用域的变化，字符串储存在字符串表，其他常量作为初始值（使用完删除）
Var::Var(Token *lt) {
    clear();
    literal = true;
    setLeft(false);
    switch(lt->tag){
        case NUM:
            setType(KW_INT);
            name="<int>";//类型作为名字
            intVal = ((Num*)lt)->val;//记录数字数值
            break;
        case CH:
            setType(KW_CHAR);
            name="<char>";
            intVal=0;
            charVal=((Char*)lt)->ch;
            break;
        case STR:
            setType(KW_CHAR);
//            nam
            strVal=((Str*)lt)->str;//记录字符串值
            setArray(strVal.size()+1);//字符串作为字符数组储存
            break;
    }
}


void Var::clear() {
    scopePath.push_back(-1);//默认全局作用域
    externed = false;
    isPtr = false;
    isArray = false;
    isLeft  = true;//默认变量作为左值
//    inited = false;
//    literal = false;
//    size = 0;
//    offset = 0;
//    ptr = NULL;//没有指向当前变量的指针变量
//    index = -1;//无效索引
//    initData = NULL;
//    live = false;
//    regId = -1;//默认放在内存
//    inMen = false;
}

string Var::getName() {
    return name;
}

void Fun::enterScope() {
    scopeEsp.push_back(0);
}
void Fun::leaveScope() {
    maxDepth = (curEsp>maxDepth)?curEsp:maxDepth;
    curEsp -= scopeEsp.back();
    scopeEsp.pop_back();
}