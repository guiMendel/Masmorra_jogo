#ifndef CLASSES_BIB_H
#define CLASSES_BIB_H

#ifndef MASMORRA_H
#include "../masmorra_bib.h"
#endif

// GUERREIRO
    // atribui a classe guerreiro ao personagem id. Interage com o usuario dependendo do tipo do personagem. self e o indice da classe guerreiro no vetor de classes
    void setup_guerreiro(int id, int self);
    // recebe um índice de personagem de classe guerreiro e imprime na tela quais escolhas este personagem tomou na criação de classe
    void choices_guerreiro(int id);

#endif