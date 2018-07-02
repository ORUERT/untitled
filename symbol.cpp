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


void Var::setType(Tag t) {
    type = t;
    if(type == KW_VOID){
        SEMERROR(VOID_VAR,"");
        type = KW_INT;
    }
    if(!externed&&type == KW_INT)size = 4;
    else if(!externed&&type == KW_CHAR)size =1;
}

void Var::setArray(int len) {
    if(len<=0){
        SEMERROR(ARRAY_LEN_INVALID,name);
        return ;
    }else {
        isArray = true;
        isLeft =false;
        arraySize = len;
        if(!externed)size*=len;
    }
}

bool Var::isBase() {
    return !isArray && !isPtr;
}

bool Var::isVoid() {
    return type==KW_VOID;
}

bool Var::setInit() {
    Var*init = initData;
    if(!init)return false;
    inited = false;
    if(externed){//声明不允许初始化
        SEMERROR(DEC_INIT_DENY,name);
    }
    else if(!GenIR::typeCheck(this,init)){//类型检查不兼容
        SEMERROR(VAR_INIT_ERR,name);
    }
    else if(init->literal){//初始值为常量
        inited = true;
        if(init->isArray)//字符串常量
            ptrVal = init->name;
        else
            intVal = init->intVal;
    }else {
        if(scopePath.size() == 1){//全局变量
            SEMERROR(GLB_INIT_ERR,name);
        }
        else
            return true;//表达式initData后续处理
    }
    return false;
}

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
    setName("");
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

bool Var::getArray() {
    return isArray;
}

bool Var::getPtr() {
    return isPtr;
}

string Var::getStrVal() {
    return strVal;
}

bool Var::isRef() {
    return !!ptr;
}
Var* Var::getVoid() {
    return SymTab::voidVar;
}

bool Var::isLiteral() {
    return this->literal&&isBase();
}
bool Var::getLeft() {
    return isLeft;
}

Var* Var::getPointer() {
    return ptr;
}
void Var::setPointer(Var* p){
    ptr = p;
}

Var* Var::getStep(Var *v) {
    if(v->isBase())return Symtab::one;
    else if(v->type == KW_CHAR)return SymTab::one;
    else if(v->type == KW_INT)return SymTab::four;
    else return NULL;
}

void Fun::enterScope() {
    scopeEsp.push_back(0);
}
void Fun::leaveScope() {
    maxDepth = (curEsp>maxDepth)?curEsp:maxDepth;
    curEsp -= scopeEsp.back();
    scopeEsp.pop_back();
}

void Fun::locate(Var *var) {
    int size = var->getSize();
    size+=(4-size%4)%4;
    scopeEsp.back()+=size;
    curEsp+=size;
    var->setOffset(-curEsp);
}

bool Fun::match(Fun*f){
    if(name != f->name){
        return false;
    }
    if(paraVar.size() != f->paraVar.size()){
        return false;
    }
    int len = paraVar.size();
    for(int i  = 0 ; i < len ; i++){
        if(GenIR::typeCheck(paraVar[i],f->paraVar[i])){
            if(paraVar[i]->getType()!=f->paraVar[i]->getType()){
                SEMWARN(FUN_DEC_CONFLICT,name);
            }
        }else {
            return false;
        }
    }
    if(type!=f->type){
        SEMWARN(FUN_RET_CONFLICT,name);
    }
    return true;
}

bool Fun::match(vector<Var*> &args){
    if(paraVar.size() != args.size()){
        return false;
    }
    int len = paraVar.size();
    for(int i = 0 ; i < len ; i ++){
        if(!GenIR::typeCheck(paraVar[i],args[i]))return false;
    }
    return true;
}

void Fun::define(Fun* def){
    externed = false;
    paraVar = def->paraVar;
}

void Fun::setReturnPoint(InterInst *inst) {
    returnPoint = inst;
}

InterInst* Fun::getReturnPoint() {
    return returnPoint;
}

void Fun::addInst(InterInst *inst) {
    interCode.addInst(inst);
}

Tag Fun::getType() {
    return type;
}
