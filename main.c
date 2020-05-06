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
    program();


    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    //変数分の領域を確保
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", count_lvar()*8);

    //先頭の式から順にコード生成
    for(int i = 0; code[i]; i++){
        printf("#code[%d]\n", i);
        gen(code[i]);

        //式の評価結果としてスタックに１つの値が残っているはずなのでスタックが溢れないようにポップしておく
        printf("    pop rax\n");
    }

    // スタックトップに式全体の値が残っているはずなので
    // それをRAXにロードして関数からの返り値とする
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}