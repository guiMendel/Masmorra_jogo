#ifndef MONSTER_BIB_H
#define MONSTER_BIB_H

#ifndef MASMORRA_H
#include "../masmorra_bib.h"
#endif

// funcoes de teste

    void mc_race(int id, int dex);
    void mc_chat(int id);

// reponsável por gerar o monstro e suas características e o colocar no vetor de monstros
void mc_build(int id);
// inicializa a struct do monstro id com valores neutros
void mc_init(int id);
// imprime informacoes sobre o monstro
void mc_inspect(int id);
// adiciona a passiva especificada ao monstro
void mc_gains_pass(int id, char *name);
// adiciona a ação especificada ao monstro
void mc_gains_act(int id, char *name);
// armazena na string a especie e classe do monstro
void monster_title(monster_t *m, char *buffer);
// realiza o turno do combate do mosnto
void mc_combat(int id);

// libera todos os recursos do monstros e destroi seu mutex
void mc_destroy(int id);



#endif