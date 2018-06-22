//
// Created by oruret on 2018/6/3.
//
#pragma once
#include "common.h"
#include <ext/hash_map>
using namespace __gnu_cxx;
/*******************************************************************************
                                   扫描器（工具类）
*******************************************************************************/
class Scanner{

    char*fileName;//文件名
    FILE*file;//文件指针
    //内部状态
    static const int BUFLEN = 80;//缓冲区大小
    int lineLen;//当前行长度
    int readPos;//读取的位置
    char line[BUFLEN];
    int lineNum;//行数
    int colNum;//列数
//    char ch;
    char lastch;

public:
    //构造与析构函数
    Scanner(char* name);
    ~Scanner();

    //扫描
    scan();

    //外部接口
    char* getFile();
    char* getLine();
    char* getCol();

};

/*******************************************************************************
                                   关键字表（工具类）
*******************************************************************************/
class Keywords{
    struct string_hash{
        size_t operator()(const string& str) const{
            return __stl_hash_string(str.c_str());
        }
    };
    hash_map<string, Tag,string_hash>keywords;
public:
    Keywords();
    Tag getTag(string name);
};

/*******************************************************************************
                                   词法分析器
*******************************************************************************/

class Lexer{
    static Keywords keywords;

    Scanner &scanner;
    char ch;
    bool scan(char need=0);
    Token *token;

public:
    Lexer (Scanner&sc);
    ~Lexer();
    Token* tokenize();//有限自动机匹配，词法记号解析
};