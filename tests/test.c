#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <locale.h>
#include "../misc/linked_list.c"
#include "../masmorra_bib.c"

int main() {
    FILE *fp = fopen("habilities/passives.txt", "r");

    pass_l = list_new();

    char read[1500];
    int n;
    // fase 0: ler nome. fase 1: ler descrição
    int phase = 0;
    // le linha por linha. Coloca o que leu em read, e quantos caracteres leu em n.
    while((n = fscanf(fp, "%[^\n]", read)) != EOF) {
        // retira o \n do caminho
        fgetc(fp);
        // reconhece o comando de adicionar nova passiva
        if(strcmp(read, "add") == 0) {
            pass_t new_pass;
            // inicializa a passiva com valores neutros
            {
                new_pass.descr = NULL;
                new_pass.name[0] = '\0';
            }
            push(pass_l, (void*)&new_pass, sizeof(pass_t));
            pass_i++;
        }
        // verifica se algo foi lido
        else if(n > 0) {
            if(phase == 0) {
                // leu o nome
                strcpy(pass(0)->name, read);
                phase = 1;
            }
            else {
                pass(0)->descr = malloc(sizeof(char) * (strlen(read) + 1));
                strcpy(pass(0)->descr, read);
                phase = 0;
            }
        }
    }
    
    printf("%s: %s\n%s: %s\n", pass(0)->name, pass(0)->descr, pass(1)->name, pass(1)->descr);
    
    fclose(fp);
}