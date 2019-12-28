#ifndef PASSIVES_C
#define PASSIVES_C

#ifndef MASMORRA_H
#include "../masmorra_bib.h"
#endif

// definição dos macros
#define PASS_ARQUEARIA "Arquearia"
#define PASS_COMBATE_COM_ARMAS_GRANDES "Combate com Armas Grandes"
#define PASS_COMBATE_COM_DUAS_ARMAS "Combate com Duas Armas"
#define PASS_DEFESA "Defesa"
#define PASS_DUELISMO "Duelismo"
#define PASS_CRITICO_APRIMORADO "Critico Aprimorado"

void add_pass(char *name, char *desc) {
    pass_t new_pass;
    strcpy(new_pass.name, name);
    new_pass.descr = (char*) malloc(sizeof(char) * (strlen(desc) + 1));
    strcpy(new_pass.descr, desc);
    push(pass_l, (void*) &new_pass, sizeof(pass_t));
    pass_i++;
}

void load_pass() {
    add_pass(PASS_ARQUEARIA, "Você ganha +2 de bônus nas jogadas de ataque realizadas com uma arma de ataque a distância.");
    add_pass(PASS_COMBATE_COM_ARMAS_GRANDES, "Quando você rolar um 1 ou um 2 num dado de dano de um ataque com arma corpo-a-corpo que esteja empunhando com duas maos, você pode rolar o dado novamente e usar a nova rolagem, mesmo que resulte em 1 ou 2. A arma deve ter a propriedade duas mãos ou versátil para ganhar esse beneficio.");
    add_pass(PASS_COMBATE_COM_DUAS_ARMAS, "Quando voce estiver engajado em uma luta com duas armas, voce pode adicionar o seu modificador de habilidade de dano na jogada de dano de seu segundo ataque.");
    add_pass(PASS_DEFESA, "Enquanto estiver usando armadura, voce ganha +1 de bonus em sua CA.");
    add_pass(PASS_DUELISMO, "Quando voce empunhar uma arma de ataque corpo-a-corpo em uma mao e nenhuma outra arma, voce ganha +2 de bonus nas jogadas de dano com essa arma.");
    add_pass(PASS_CRITICO_APRIMORADO, "Seus golpes adquirem uma margem de acerto critico de 19 a 20 nas jogadas de ataque.");
}

#endif