//
// Created by oruret on 2018/6/3.
//

#ifndef UNTITLED_COMMON_H
#define UNTITLED_COMMON_H

/*
 * 词法记号标签
 */

enum Tag{
    ERR,
    END,
    ID,
    KW_INT,KW_CHAR,KW_VOID,
    KW_EXTERN,
    NUM,CH,STR,
    NOT,LEA,
    ADD,SUB,MUL,DIV,MOD,
    INC,DEC,
    GT,GE,LT,LE,EQU,NEQU,
    AND,OR,
    LPAREN,RPAREN,
    LBRACK,RBRACK,
    LBRACE,RBRACE,
    COMMA,COLON,SEMICON,
    ASSIGN,
    KW_IF,KW_ELSE,
    KW_SWITCH,KW_CASE,KW_DEAFULT,
    KW_WHILE,KW_DO,KW_FOR,
    KW_BREAK,KW_CONTINUE,KW_RETURN
};

/*
 * 词法分析错误类型
 */

enum LexError{
    STR_NO_R_QUTION,//字符串没有右引号
    NUM_BIN_TYPE,//二进制数没有实体数据
    NUM_HEX_TYPE,//十六进制数没有实体数据
    CHAR_NO_R_QUTION,//字符没有右引号
    CHAR_NO_DATA,//字符没有数据
    OR_NO_PAIR,//||只有一个|
    COMMENT_NO_END,//多行注释没有正常结束
    TOKEN_NO_EXIST//不存在的词法记号
};

/*
 * 语法分析错误类型
 */

enum SynError{
    TYPE_LOST,
    TYPE_WRONG,
    ID_LOST,
    ID_WRONG,
    NUM_LOST,
    NUM_WRONG,
    LITERAL_LOST,
    LITERAL_WRONG,
    COMMA_LOST,
    COMMA_WRONG,
    SEMICON_LOST,
    SEMICON_WRONG,
    ASSIGN_LOST,
    ASSIGN_WRONG,
    COLON_LOST,
    COLON_WRONG,
    WHILE_LOST,
    WHILE_WRONG,
    LPAREN_LOST,
    LPAREN_WRONG,
    RPAREN_LOST,
    RPAREN_WRONG,
    LBRACK_LOST,
    LBRACK_WRONG,
    RBRACE_LOST,
    RBRACE_WRONG,
    LBRACE_LOST,
    LBRACE_WRONG,
    RBRACE_LOST,
    RBRACE_WRONG
};

#endif //UNTITLED_COMMON_H
