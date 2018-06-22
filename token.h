//
// Created by oruret on 2018/4/9.
//
#pragma once
#include "common.h"
/*
 * 词法记号类
 */

#include <string>
#include "cifa.cpp"
using namespace std;

/*
 * 词法记号类
 */

class Token{

public:
    Tag tag;
    Token (Tag t);
    virtual string toString();
    virtual ~Token();
};

/*
 * 标识符记号类
 */
class Id:public Token{
public:
    string name;
    Id (string n);
    virtual string toString();
};

/*
 * 数字记号类
 */

class Num:public Token{
public:
    int val;
    Num (int v);
    virtual string toString();
};

/*
 * 字符记号类
 */

class Char:public Token{
public:
    char ch;
    Char (char c);
    virtual string toString();
};

/*
 * 字符串记号类
 */

class Str:public Token{
public:
    string str;
    Str (string s);
    virtual string toString();
};
//
///*
// * 关键字符类
// */
//
//class KeyWords{
//    //hash 函数
//    /*
//     * struct string_hash{
//     * size_t operator()(const string& str)const{
//     * return __stl_hash_string(str.c_str());
//     * }
//     * };
//     */
//    hash_map<string , Tag, string_hash> keywords;
//public:
//    KeyWords();
//    Tag getTag(string name);
//};
//
//KeyWords::KeyWords(){
//    keywords["int"] = KW_INT;
//    keywords["char"] = KW_CHAR;
//    keywords["void"] = KW_VOID;
//    keywords["extern"] = KW_EXTERN;
//    keywords["if"] = KW_IF;
//    keywords["else"] = KW_ELSE;
//    keywords["switch"] = KW_SWITCH;
//    keywords["case"] = KW_CASE;
//    keywords["default"] = KW_DEAFULT;
//    keywords["while"] = KW_WHILE;
//    keywords["do"] = KW_DO;
//    keywords["for"] = KW_FOR;
//    keywords["break"] = KW_BREAK;
//    keywords["continue"] = KW_CONTINUE;
//    keywords["return"] = KW_RETURN;
//
//};
//
//Tag KeyWords::getTag(string name) {
//    return keywords.find(name)!=keywords.end()?keywords[name]:ID;
//}

/*
 * 关键字      字符串值        词法标签
 * int          "int"           KW_INT
 * char         "char"          KW_CHAR
 * void         "void"          KW_VOID
 * extern       "extern"        KW_EXTERN
 * if           "if"            KW_IF
 * else         "else"          KW_ELSE
 * switch       "switch"        KW_SWITCH
 * case         "case"          KW_CASE
 * default      "default"       KW_DEFAULT
 * while        "while"         KW_WHILE
 * do           "do"            KW_DO
 * for          "for"           KW_FOR
 * break        "break"         KW_BREAK
 * continue     "continue"      KW_CONTINUE
 * return       "return"        KW_RETURN
 */