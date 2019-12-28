#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "../masmorra_bib.c"

typedef struct {
    int (*function)();
} type;

int a;
int action() {
    printf("Funcionou\n");
    return a;
}

int main() {
    a = 2;
    type tt;
    tt.function = action;
    printf("%d\n", (*tt.function)());
    return 0;
}