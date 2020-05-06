#include <stdio.h>
#include "9cc.h"

void gen_lval(Node *node){
    if(node->kind != ND_LVAR)
        perror("代入の左辺値が変数ではありません");
    printf("#--gen_lval\n");
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

// コードジェネレータ
void gen(Node *node){

    if(node->kind == ND_BLOCK){
        printf("#--ND_BLOCK\n");

        for(int i = 0; node->block[i]; i++){
            printf("#node->block[%d]\n", i);
            gen(node->block[i]);
        }
        return;
    }
    
    if(node->kind == ND_RETURN){
        gen(node->lhs);
        printf("#--ND_RETURN\n");
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    }
    
    if(node->kind == ND_ELSE){  // if(A) B else C
        printf("#--ND_ELSE\n");
        printf("    push rax\n");
        gen(node->lhs->lhs);    // A
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .L.else.C\n");
        gen(node->lhs->rhs);// B
        printf("    jmp .L.else.end\n");
        printf("  .L.else.C:\n");
        gen(node->rhs);// C
        printf("  .L.else.end:\n");
        return;
    }

    if(node->kind == ND_IF){ //if(A) B
        printf("#--ND_IF\n");
        printf("    push rax\n");
        gen(node->lhs);//A
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .L.if.end\n");
        gen(node->rhs);//B

        printf("    .L.if.end:\n");
        return;
    }

    if(node->kind == ND_WHILE){ //while(A) B
        printf("#--ND_WHILE\n");
        printf("    push rax\n");
        printf("  .L.begin.while:\n");
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .L.end.while\n");
        gen(node->rhs);
        printf("    jmp .L.begin.while\n");
        printf("  .L.end.while:\n");
        return;
    }

    if(node->kind == ND_FOR){// for(A; B; C) D
        printf("#--ND_FOR\n");
        printf("    push rax\n");
        if(node->lhs->lhs->lhs->rhs != NULL)
            gen(node->lhs->lhs->lhs->rhs);//A
        printf("  .L.begin.for:\n");
        if(node->lhs->lhs->rhs != NULL){
            gen(node->lhs->lhs->rhs);//B
        } else{ 
            printf("    pop rax\n");
            printf("    push 1\n");
        }
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .L.end.for\n");
        gen(node->rhs);//D
        if(node->lhs->rhs != NULL)
            gen(node->lhs->rhs);//C
        printf("    jmp .L.begin.for\n");
        printf("  .L.end.for:\n");
        return;
    }

    switch (node->kind){
        case ND_NUM:
            printf("#--ND_NUM\n");
            printf("    push %d\n", node->val);
            return;
        case ND_LVAR:
            printf("#--ND_LVAR\n");
            gen_lval(node);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_ASSIGN:
            printf("#--ND_ASSIGN\n");
            gen_lval(node->lhs);
            gen(node->rhs);

            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");
            return;
    }


    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind){
        case ND_ADD:
            printf("#--ND_ADD\n");
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("#--ND_SUB\n");
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("#--ND_MUL\n");
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("#--ND_DIV\n");
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
        case ND_EQ:
            printf("#--ND_EQ\n");
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_NE:
            printf("#--ND_NE\n");
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LT:
            printf("#--ND_LT\n");
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LE:
            printf("#--ND_LE\n");
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
    }

    printf("    push rax\n");
}