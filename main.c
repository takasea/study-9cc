#include <stdio.h>
#include "9cc.h"


char *user_input;
Token *token;
Node *code[100];

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の個数がただしくありません\n");
        return 1;
    }

    user_input = argv[1];

    //パースする
    token = tokenize(user_input);
    Node *node = expr();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // 抽象構文木を下りながらコード生成
    gen(node);

    // スタックトップに式全体の値が残っているはずなので
    // それをRAXにロードして関数からの返り値とする

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}