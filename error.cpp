//
// Created by oruret on 2018/6/3.
//

#include "error.h"
#include "token.h"
#include "lexer.h"

int Error::errorNum = 0;
int Error::warnNum = 0;
Scanner* Error::scanner = NULL;

/*
 * 初始化错误个数
 */

Error::Error(Scanner*sc){
    scanner = sc;
}

/*
 * 打印词法错误
 */
void Error::lexError(int code){
    static const char *lexErrorTable[] = {
            "字符串丢失右引号",
            "二进制数没有实体数据",
            "十六进制数没有实体数据",
            "字符丢失右单引号",
            "不支持空字符",
            "错误的"或"运算符",
            "多行注释没有正常结束",
            "词法记号不存在"
    };
    errorNum++;
    printf("%s<%d行,%d列> 词法错误: %s.\n",
           scanner->getFile(),
           scanner->getLine(),
           scanner->getCol(),
           lexErrorTable[code]
    );
}


/*
 * 打印语法错误
 */

void Error::synError(int code, Token *t) {
    static const char *synErrorTable[] = {
            "类型",
            "标识符",
            "数组长度",
            "常量",
            "逗号",
            "分号",
            "=",
            "冒号",
            "while",
            "(",
            ")",
            "[",
            "]",
            "{",
            "}"
    };
    errorNum++;
    if(code%2 == 0)
        printf("%s<第%d行> 语法错误 : 在 %s 之前丢失 %s.\n"
               ,scanner->getFile(),scanner->getLine()
        ,t->toString().c_str(),synErrorTable[code/2]);
    else
        printf("%s<第%d行> 语法错误 : 在 %s 处没有正确匹配 %s.\n"
               ,scanner->getFile(),scanner->getLine()
        ,t->toString().c_str(),synErrorTable[code/2]);
}

/*
 * 打印语义错误
 */
void Error::semError(int code, string name) {
    static const char *semErrorTable[]={
            "变量重定义",
            "函数重定义",
            "变量未声明",
            "函数未声明",
            "函数声明与定义不匹配",
            "函数形参与实参不匹配",
            "变量声明时不允许初始化",
            "函数定义不能使用声明保留字extern",
            "数组长度应该是正整数",
            "变量初始化类型错误",
            "全局变量初始化值不是常量",
            "变量不能声明为void类型",
            "无效的左值表达式",
            "赋值表达式不兼容",
            "表达式操作数不能是基本类型",
            "表达式操作数不是基本类型",
            "数组索引运算类型错误",
            "void的函数返回值不能参与表达式运算",
            "break语句不能出现在循环或者switch语句之外",
            "continue不能出现在循环之外",
            "return语句和函数返回值类型不匹配"
    };
    errorNum++;
    printf("%s<第%d行> 语义错误 : %s %s.\n",scanner->getFile(),scanner->getLine(),name.c_str(),semWarnTable[code]);
}

void Error::semWarn(int code, string name) {
    //语义警告信息串
    static const char *semWarnTable[]={
            "函数参数列表类型冲突",
            "函数返回值类型不精确匹配"
    };
    warnNum++;
    printf("%s<第%d行> 语义警告 : %s %s.\n",scanner->getFile(),scanner->getLine(),name.c_str(),semWarnTable[code]);
}

#define SEMERROR(code,name) Error::semError(code,name)
#define SEMWARN(code,name) Error::semWarn(code,name)