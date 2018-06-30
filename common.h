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
/*
	语义错误码
*/
enum SemError
{
    VAR_RE_DEF,					//变量重定义
    FUN_RE_DEF,					//函数重定义
    VAR_UN_DEC,					//变量未声明
    FUN_UN_DEC,					//函数未声明
    FUN_DEC_ERR,				//函数声明与定义不匹配
    FUN_CALL_ERR,				//行参实参不匹配
    DEC_INIT_DENY,			//声明不允许初始化
    EXTERN_FUN_DEF,			//函数声明不能使用extern
    ARRAY_LEN_INVALID,	//数组长度无效
    VAR_INIT_ERR,				//变量初始化类型错误
    GLB_INIT_ERR,				//全局变量初始化值不是常量
    VOID_VAR,						//void变量
    EXPR_NOT_LEFT_VAL,	//无效的左值表达式
    ASSIGN_TYPE_ERR,		//赋值类型不匹配
    EXPR_IS_BASE,				//表达式不能是基本类型
    EXPR_NOT_BASE,			//表达式不是基本类型
    ARR_TYPE_ERR,				//数组运算类型错误
    EXPR_IS_VOID,				//表达式不能是VOID类型
    BREAK_ERR,					//break不在循环或switch-case中
    CONTINUE_ERR,				//continue不在循环中
    RETURN_ERR					//return语句和函数返回值类型不匹配
};

/*
	语义警告码
*/
enum SemWarn
{
    FUN_DEC_CONFLICT,		//函数参数列表类型冲突
    FUN_RET_CONFLICT		//函数返回值类型冲突
};

enum Operator{
    OP_NOP, //空指令
    OP_DEC, //声明
    OP_ENTRY,OP_EXIT, //函数出入口
    OP_AS, //赋值
    OP_ADD,OP_SUB,OP_MUL,OP_DIV,OP_MOD, //算术运算
    OP_NEG, //取负
    OP_GT,OP_GE,OP_LT,OP_LE,OP_EQU,OP_NE, //关系元算
    OP_NOT, //非
    OP_AND,OP_OR, //与、或
    OP_LEA, //取址
    OP_SET,OP_GET, //指针预算
    OP_JMP, //无条件跳转
    OP_JT,OP_JF,OP_JNE, //条件跳转
    OP_ARG, //参数传递
    OP_PROC, //调用过程
    OP_CALL, //调用函数
    OP_RET, //直接返回
    OP_RETV //带数据返回
};

#endif //UNTITLED_COMMON_H
