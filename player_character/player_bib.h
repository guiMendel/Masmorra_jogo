#ifndef PLAYER_BIB_H
#define PLAYER_BIB_H

#ifndef MASMORRA_H
#include "../masmorra_bib.h"
#endif

// Funcoes de teste

    void pc_race(int id, int dex);
    void pc_chat(int id);

// responsável por montar o personagem
void pc_build(int id);
// adiciona ao vetor de escolhas de classe d eum personagem a string providenciada
void pc_chose(int id, char *name);
// escreve em buffer a escolha de indice i. Se nao houver escolha com este indice, escreve '\0' no buffer
void pc_choice_get(int id, int i, char *buffer);
// inicializa os valores da struct do jogador com valores neutros
void pc_init(int id);
// imprime na tela informacoes sobre este personagem. As margens definem quantas linhas em branco devem haver antes da impressao e apos
void pc_sheet(int id, int margin_top, int margin_bot);
// imprime informações de combate
void pc_sheet_combat(int id, int margin_top, int margin_bot);
// gera uma string com a classe e subclasse do personagem flexionadas de acordo com seu sexo
int player_title(int id, char *buffer);
// realiza o turno do combate do personagem
void pc_combat(int id);

// atribui uma passiva a um jogador
void pc_gains_pass(int id, char *name);
// atribui uma acao a um jogador
void pc_gains_act(int id, char *name);

// libera os recursos do personagem
void pc_destroy(int id);


#endif