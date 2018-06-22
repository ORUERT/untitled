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

