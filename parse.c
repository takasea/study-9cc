#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "9cc.h"

LVar *locals;

// エラーを報告するための関数
void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ " );
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号(TK_RESERVED)のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
int consume(char *op)
{
    if (token->kind != TK_RESERVED | strlen(op) != token->len || memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

Token *consume_ident(){
    if(token->kind != TK_INDENT)
        return NULL;
    Token *tok = token;
    token = token->next; 
    return tok;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op)
{
    if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len))
        error_at(token->str, "'%s'ではありません", op);
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number()
{
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

int at_eof()
{
    return token->kind == TK_EOF;
}

//次のトークンがcurかどうか調べる
int check_next_token(char *cur){
    if(!strncmp(token->next->str, cur, token->next->len))
        return true;
    return false;
}

int check_token_kind(TokenKind cur_kind){
    if(token->kind == cur_kind)
        return true;
    return false;
}

//与えられた文字が英数字かアンダースコアか判定
int is_alnum(char c){
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9' || (c == '_'));
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

//入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p)
{
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p)
    {
        // 空白文字をスキップ
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if(!strncmp(p, ">=", 2) || !strncmp(p, "<=", 2) || !strncmp(p, "==", 2) || !strncmp(p, "!=", 2)){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '<' || *p == '>' || *p == '=' || *p == ';' || *p == '{'|| *p == '}'|| *p == ',')
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if(!strncmp(p, "return", 6) && !is_alnum(p[6])){
            cur = new_token(TK_RESERVED, cur, p, 6);
            p += 6;
            continue;
        }

        if(!strncmp(p, "if", 2) && !is_alnum(p[2])){
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if(!strncmp(p, "else", 4) && !is_alnum(p[4])){
            cur = new_token(TK_RESERVED, cur, p, 4);
            p += 4;
            continue;
        }

        if(!strncmp(p, "while", 5) && !is_alnum(p[5])){
            cur = new_token(TK_RESERVED, cur, p, 5);
            p += 5;
            continue;
        }

        if(!strncmp(p, "for", 3) && !is_alnum(p[3])){
            cur = new_token(TK_RESERVED, cur, p, 3);
            p += 3;
            continue;
        }


        //変数の場合
        if (('a' <= *p && *p <= 'z') || *p=='_'){
            int ident_count = 0;
            while(('a' <= *p && *p <= 'z') || *p=='_'){
                ident_count++;
                p++;
            }
            cur = new_token(TK_INDENT, cur, p-ident_count, ident_count);
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

// 二項演算子を扱うノードの作成
Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// 数値を扱うノードの作成
Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}


// 変数を名前で検索する。見つからなかった場合はNULLを返す
LVar *find_lvar(Token *tok){
    for(LVar *var = locals; var; var = var->next)
        if(var->len == tok->len && !memcmp(tok->str, var->name, var->len))
            return var;
    return NULL;
}

// 変数の数をintで返す
int count_lvar(){
    int count = 0;
    LVar *lvar = locals;

    if(lvar == NULL)
        return 0;
    
    while(lvar != NULL){
        count++;
        lvar = lvar->next;
    }
    return count - 1;
}


void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();


// 生成規則 program = stmt*
void program(){
    int i = 0;
    while(!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}


// 生成規則 stmt =   expr ";"  
//                  | "{" stmt* "}"
//                  | "if" "(" expr ")" stmt ("else" stmt)?
//                  | "while" "(" expr ")" stmt 
//                  | "for" "(" expr? ";" expr? ";" expr? ";" ")" stmt
//                  | "return" expr ";"
Node *stmt(){
    Node *node;

    //block
    if(consume("{")){
        if(consume("}"))
            return new_node_num(0);

        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_BLOCK;
        int i = 0;
        while(!consume("}")){
            node->block[i++] = stmt();
        }
        node->block[i] = NULL;
        return node;
    }
    
    if(consume("return")){
        node = new_node(ND_RETURN, expr(), NULL);
        expect(";");
        return node;
    }

    if (consume("if")) {
        expect("(");
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        if(consume("else"))
            node = new_node(ND_ELSE, node, stmt());
        return node;
    }

    if(consume("while")){
        expect("(");
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        return node;
    }

    //"for" "(" expr? ";" expr? ";" expr? ";" ")" stmt
    if(consume("for")){ //todo:コンパクトに書く

        if(check_next_token(";")){
            expect("(");
            node = new_node(ND_FOR, NULL, NULL);
        }else{
            expect("(");
            node = new_node(ND_FOR, NULL, expr());
        }

        if(check_next_token(";")){
            expect(";");
            node = new_node(ND_FOR, node, NULL);
        }else{
            expect(";");
            node = new_node(ND_FOR, node, expr());
        }

        if(check_next_token(")")){
            expect(";");
            node = new_node(ND_FOR, node, NULL);
        }else{
            expect(";");
            node = new_node(ND_FOR, node, expr());
        }


        expect(")");
        node = new_node(ND_FOR, node, stmt());
        return node;
    }

    node = expr();
    expect(";");
    return node;    
}

// 生成規則 expr = assign
Node *expr(){
    return assign();
}

// 生成規則 assign = equality("=" assign)?
Node *assign(){
    Node *node = equality();
    if(consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

// 生成規則 equality = relational("==" relational | "!=" relational)*
Node *equality(){
    Node *node = relational();

    for(;;){
        if(consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if(consume("!="))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}
// 生成規則 relational = add("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(){
    Node *node = add();

    for(;;){
        if(consume("<="))
            node = new_node(ND_LE, node, add());
        else if(consume("<"))
            node = new_node(ND_LT, node, add());
        else if(consume(">="))
            node = new_node(ND_LE, add(), node);
        else if(consume(">"))
            node = new_node(ND_LT, add(), node);
        else
            return node;
    }
}

// 生成規則 add = mul( "+" mul | "-" mul )*
Node *add(){
    Node *node = mul();
    for(;;){
        if(consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if(consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}


// 生成規則 mul = unary( "*" unary | "/" unary )*
Node *mul(){
    Node *node = unary();

    for(;;){
        if(consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if(consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

//生成規則 unary = ( "+" | "-" )? unary
Node *unary(){

    if(consume("+"))
        return primary();
    if(consume("-"))
        return new_node(ND_SUB, new_node_num(0), unary());
    return primary();
}

// 生成規則 primary = num 
//                  | ident( "(" ")" )?
//                  | "(" expr ")"
Node *primary(){
    // 次のトークンが"("なら、"(" expr ")"のはず
    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }

    // 変数と関数
    Token *tok = consume_ident();
    if(tok){
        Node *node = calloc(1, sizeof(Node));
        if(consume("(")){
            //関数
            node->kind = ND_FUNCTION;
            node->label = tok->str;
            node->fn_len = tok->len;
            if(check_token_kind(TK_NUM))
                node->rhs = new_node_num(expect_number());
            node->lhs = NULL;
            while(consume(",")){                
                node = new_node(ND_FUNCTION, node, new_node_num(expect_number()));
            }
            expect(")");
        }else{
            //変数
            node->kind = ND_LVAR;
            LVar *lvar = find_lvar(tok);
            if(lvar){
                node->offset = lvar->offset;
            } else{
            // localsの先頭
                if(!locals)
                    locals = calloc(1,sizeof(LVar));
            
                lvar = calloc(1, sizeof(lvar));
                lvar->next = locals;
                lvar->name = tok->str;
                lvar->len = tok->len;
                lvar->offset = locals->offset + 8;
                node->offset = lvar->offset;
                locals = lvar;
            }
        }
        
        return node;
    }

    // そうでなければ数値のはず
    return new_node_num(expect_number());
}
