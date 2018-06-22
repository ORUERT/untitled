//
// Created by oruret on 2018/6/3.
//

#pragma once
#include "common.h"


class Error {
    static Scanner *Scanner;//当前扫描器

public:
    Error(Scanner*sc);

    static int errorNum;//错误个数
    static int warnNum;//警告个数
    //外界接口
    static int getErrorNum();
    static int getWarnNum();

    //错误接口
    static void lexError(int code);//打印词法错误
    static void synError(int code,Token*t);//打印语法错误
    static void semError(int code ,string name="");//打印语义错误
    static void semWarn(int code ,string name="");//打印语义警告
};

#define FATAL "<fatal>:"
#define ERROR "<error>:"
#define WARN "<warn>:"

//调试输出
#ifdef DEBUG
#define PDEBUG(fmt, args...) printf(fmt,##args)
#else
#define PDEBUG(fmt, args...)
#endif //DEBUG????

void SEMWARN(int code);//打印语义警告

