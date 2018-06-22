//
// Created by oruret on 2018/6/3.
//
/*
 * 类别      词法记号         含义
 * 特殊      err              错误记号
 *           end             文件结束
 * 标识符    id               标识符
 * 常量      num              数字
 *           ch               字符
 *           str              字符串
 * 关键字    kw_int            int
 *           kw_char            char
 *           kw_void            void
 *           kw_extern          extern
 *           kw_if              if
 *           kw_else            else
 *           kw_switch          switch
 *           kw_case            case
 *           kw_default         default
 * 界符      kw_while            while
 *           kw_for             for
 *           kw_do              do
 *           kw_break           break
 *           kw_continue        continue
 *           kw_return          return
 *           add                +
 *           sub                -
 *           mul                *
 *           div                /
 *           mod                %
 *           inc                ++
 *           dec                --
 *           not                !
 *           lea                &
 *           and                &&
 *           or                 ||
 *           assign             =
 *           gt                 >
 *           ge                 >=
 *           lt                 <
 *           le                 <=
 *           equ                ==
 *           nequ               !=
 *           comma              ,
 *           colon              :
 *           semicon            ;
 *           lparen             (
 *           rparen             )
 *           lbrack             [
 *           rbrack             ]
 *           lbrac              {
 *           rbrac              }
 */
#include "lexer.h"
#include "token.h"
#include "error.h"
#include "compiler.h"

/*******************************************************************************
                                   扫描器
*******************************************************************************/

Scanner::Scanner(char *name) {
    file = fopen(name,"r");//打开文件
    if(!file){
        printf(FATAL"文件%s打开失败！请检查文件名路径。\n",name);
    }
    fileName = name;
    //初始化
    lineLen = 0;
    readPos = -1;
    lastch = 0;
    lineNum = 1;
    colNum = 0;
}

Scanner::~Scanner() {
    if(file){
        PDEBUG(WARN"文件未全部扫描！\n");
        Error::warnNum++;//警告数增加
        fclose(file);
    }
}

/*
 * 显示字符
 */
void Scanner::showChar(char ch){
    if(ch == -1)printf("EOF");
    else if(ch == '\n')printf("\\n");//打印/n
    else if(ch == '\t')printf("\\t");
    else if(ch == ' ')printf("<blank>");
    else printf("%c",ch);
    printf("\t\t<%d>\n",ch);
}
int Scanner::scan() {
    if(!file){
        return -1;
    }
    if(readPos == lineLen - 1){
        lineLen = fread(line,1,BUFLEN,file);
        /*
         * size_t fread(void *buffer, size_t size, size_t count, FILE *stream);
           -- buffer:指向数据块的指针
           -- size:每个数据的大小，单位为Byte(例如：sizeof(int)就是4)
           -- count:数据个数
           -- stream:文件指针
         */
        if(lineLen == 0){
            lineLen = 1;
            line[0] = -1;
        }
        readPos = -1;
    }
    readPos ++;
    char ch = line[readPos];

    if(lastch == '\n'){
        lineNum ++;
        colNum = 0;
    }

    if(ch == -1) {
        fclose(file);
        file = NULL;
    }else if(ch != '\n'){
        colNum ++;
    }
    lastch = ch;
    if(Args::showChar)showChar(ch);
    return ch;
}

char* Scanner::getFile() {
    return fileName;
}

char* Scanner::getLine() {
    return lineNum;
}

char* Scanner::getCol() {
    return colNum;
}
/*******************************************************************************
                                   关键字表
*******************************************************************************/
KeyWords::KeyWords(){
    keywords["int"] = KW_INT;
    keywords["char"] = KW_CHAR;
    keywords["void"] = KW_VOID;
    keywords["extern"] = KW_EXTERN;
    keywords["if"] = KW_IF;
    keywords["else"] = KW_ELSE;
    keywords["switch"] = KW_SWITCH;
    keywords["case"] = KW_CASE;
    keywords["default"] = KW_DEAFULT;
    keywords["while"] = KW_WHILE;
    keywords["do"] = KW_DO;
    keywords["for"] = KW_FOR;
    keywords["break"] = KW_BREAK;
    keywords["continue"] = KW_CONTINUE;
    keywords["return"] = KW_RETURN;

};

Tag KeyWords::getTag(string name) {
    return keywords.find(name)!=keywords.end()?keywords[name]:ID;
}
/*******************************************************************************
                                   词法分析器
*******************************************************************************/

Keywords Lexer::keywords;

Lexer::Lexer(Scanner &sc):scanner(sc) {
    token = NULL;
    ch = ' ';
}

Lexer::~Lexer() {
    if(!token){
        delete token;
    }
}

bool Lexer::scan(char need){
    ch = scanner.scan();
    if(need){
        if(ch!=need){
            return false;
        }
        ch = scanner.scan();
        return true;
    }
    return true;
}

#define LEXERROR(code) Error::lexError(code) //封装

Token* Lexer::tokenize(){

    for(;ch!=-1;){
        Token *t =NULL;
        while(ch==' '||ch=='\n'||ch=='\t')scan();
        //处理其他词法记号
        //处理标识符关键字
        if(ch>='a'&&ch<='z'||ch>='A'&&ch<='Z'||ch=='_'){
            string name = "";
            do{
                name.push_back(ch);
                scan();
            }while(ch>='a'&&ch<='z'||ch>='A'&&ch<='Z'||ch=='_'||ch>='0'&&ch<='9');
            Tag tag = keywords.getTag(name);
            if(tag == ID){
                t = new Id(name);
            }else {
                t = new Token(tag);
            }
        }else if(ch>='0'&&ch<='9'){//处理数字
            int val = 0;
            if(ch != '0'){  //十进制
                do{
                    val = val * 10 + ch - '0';
                    scan();
                }while(ch>='0'&&ch<='9');
            }else{
                scan();
                if(ch == 'x'){//十六进制
                    scan();
                    if(ch>='0'&&ch<='9'||ch>='A'&&ch<='F'||ch>='a'&&ch<='f'){
                        do{
                            val = val * 16 +ch;
                            if(ch>='0'&&ch<='9')val-='0';
                            else if(ch>='A'&&ch<='F')val+=10-'A';
                            else if(ch>='a'&&ch<='f')val+=10-'a';
                            scan();
                        }while(ch>='0'&&ch<='9'||ch>='A'&&ch<='F'||ch>='a'&&ch<='f');
                    }else {
                        LEXERROR(NUM_HEX_TYPE);
                        t = new Token(ERR);
                    }
                }else if(ch == 'b'){
                    scan();
                    if(ch>='0'&&ch<='1'){
                        do{
                            val = val * 2 + ch - '0';
                            scan();
                        }while(ch>='0'&&ch<='1');
                    }else {
                        LEXERROR(NUM_BIN_TYPE);
                        t=new Token(ERR);
                    }
                }else if(ch>='0'&&ch<='7'){
                    do{
                        val = val * 8 + ch - '0';
                        scan();
                    }while(ch>='0'&&ch<='7');
                }
            }
            if(!t)t=new Num(val);
        }else if(ch == '\''){   //字符
            char c;
            scan();
            if(ch == '\\'){         //转义字符
                scan();
                if(ch == 'n')c='\n';
                else if(ch == '\\')c='\\';
                else if(ch == 't')c='\t';
                else if(ch == '0')c = '\0';
                else if(ch == '\'')c = '\'';
                else if(ch == -1||ch == "\n"){
                    LEXERROR(CHAR_NO_R_QUTION);
                    t = new Token(ERR);
                }
                else c = ch;
            }else if(ch == '\n'||ch == -1){ // 字符结束
                LEXERROR(CHAR_NO_R_QUTION);
                t = new Token(ERR);
            }else if(ch == '\''){
                LEXERROR(CHAR_NO_DATA);
                t = new Token(ERR);
                scan();
            }else c = ch;
            if(!t){ //正常字符
                if(scan('\'')){  //处理右单引号
                    t = new Char(c);
                }else {
                    LEXERROR(CHAR_NO_R_QUTION);
                    t = new Token(ERR);
                }
            }
        }else if(ch == '"'){ //字符串常量
            string  str = "";
            while(!scan('"')){
                if(ch=='\\'){
                    scan();
                    if(ch=='n')str.push_back('\n');
                    else if(ch=='\\')str.push_back('\\');
                    else if(ch=='t')str.push_back('\t');
                    else if(ch=='"')str.push_back('"');
                    else if(ch=='0')str.push_back('\0');
                    else if(ch=='\n'); // '\\n' 字符串换行?????
                    else if(ch==-1){
                        LEXERROR(STR_NO_R_QUTION);
                        t=new Token(ERR);
                        break;
                    }else str.push_back(ch);
                }else if(ch == '\n' ||ch == -1){
                    LEXERROR(STR_NO_R_QUTION);
                    t = new Token(ERR);
                    break;
                }else str.push_back(ch);
            }
            if(!t) t = new Str(str);
        }else {
            switch(ch){ //界符
                case '+':
                    t = new Token(scan('+')?INC:ADD);break;
                case '-':
                    t = new Token(scan('-')?DEC:SUB);break;
                case "*":
                    t = new Token(MUL);scan();break;
                case '/':
                    scan();
                    if(ch == '/'){
                        while(!(ch=='\n' || ch == -1)){
                            scan();
                        }
                        t = new Token(ERR);
                    }else if(ch == '*') {
                        while (!scan(-1)) {
                            if (ch == '*') {
                                if (ch == '/') {//没法处理 /**/*/的情况
                                    break;
                                }
                            }
                        }
                        if (!t && ch == -1) {
                            LEXERROR(COMMENT_NO_END);
                            t = new Token(ERR);
                        }
                    }else{
                        t = new Token(DIV);
                    }
                case '%':
                    t = new Token(MOD);scan();break;
                case '>':
                    t = new Token(scan('=')?GE:GT);break;
                case '<':
                    t = new Token(scan('=')?LE:LT);break;
                case '=':
                    t = new Token(scan('=')?EQU:ASSIGN);break;
                case '&':
                    t = new Token(scan('&')?AND:LEA);break;
                case '|':
                    t = new Token(scan('|')?OR:ERR);
                    if(t->tag == ERR){
                        LEXERROR(OR_NO_PAIR);
                    }
                    break;
                case '!':
                    t = new Token(scan('=')?NEQU:NOT);break;
                case ',':
                    t = new Token(COMMA);scan();break;
                case ':':
                    t = new Token(COLON);scan();break;
                case ';':
                    t = new Token(SEMICON);scan();break;
                case '(':
                    t = new Token(LPAREN);scan();break;
                case ')':
                    t = new Token(RPAREN);scan();break;
                case '[':
                    t = new Token(LBRACK);scan();break;
                case ']':
                    t = new Token(RBRACK);scan();break;
                case '{':
                    t = new Token(LBRACE);scan();break;
                case '}':
                    t = new Token(RBRACE);scan();break;
                case -1:
                    t = new Token(END);scan();break;
                default:
                    t = new Token(ERR);
                    LEXERROR(TOKEN_NO_EXIST);
                    scan();
            }
        }
        if(token)delete token;
        token = t;
        if(token&&token->tag!=ERR)
            return token;
        else
            continue;
    }
    //文件结束
    if(token)delete token;
    return token = new Token(END);
}