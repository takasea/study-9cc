#include <stdio.h>

int foo(){
    printf("foo:OK\n");
}

int foo_two(int i, int j){
    printf("foo_two:%d, %d\n",i,j);
}

int foo_three(int i, int j, int k){
    printf("foo_three:%d, %d, %d\n", i, j, k);
}

int foo_six(int i, int j, int k, int l, int m, int n){
    printf("foo_six:%d, %d, %d, %d, %d, %d\n", i, j, k, l, m, n);
}

int foo_seven(int i, int j, int k, int l, int m, int n, int o){
    printf("foo_six:%d, %d, %d, %d, %d, %d, %d\n", i, j, k, l, m, n, o);
}