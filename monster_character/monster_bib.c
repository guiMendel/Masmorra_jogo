#ifndef MONSTER_C
#define MONSTER_C

#ifndef MASMORRA_H
#include "../masmorra_bib.h"
#endif

#ifndef MONSTER_SPAWNS_C
#include "monster_spawns.c"
#endif


void mc_init(int id) {
    mtr[id]->class[0] = '\0';
    mtr[id]->descr = NULL;
    mtr[id]->sex = '-';
    mtr[id]->hp = 1;
    mtr[id]->hp_max = 1;

    mtr[id]->state[0] = 1;
    mtr[id]->state[1] = 0;

    mtr[id]->pass = list_new();
    mtr[id]->act = list_new();

    mtr[id]->armor[0] = NULL;
    mtr[id]->armor[1] = NULL;
    mtr[id]->armor[2] = NULL;
    mtr[id]->armor[3] = NULL;
    mtr[id]->armor[4] = NULL;

    mtr[id]->hand_held[0] = NULL;
    mtr[id]->hand_held[1] = NULL;

    mtr[id]->other_equips = list_new();
    mtr[id]->inventory = list_new();
    pthread_mutex_init(&m_mtr[id], NULL);
}

void mc_race(int id, int dex) {
    int disadvantage = roll_die(20) - 3*mod(dex);
    if(disadvantage<1) disadvantage = 1;
    sleep(disadvantage/10);
    pthread_mutex_lock(&m_io);
        printf("Monstro %d - Bruargh! Blenrrugh! Mnreng bleh %d unglaumbf\n", id, disadvantage);
    pthread_mutex_unlock(&m_io);
}

void mc_chat(int id) {
    pthread_mutex_lock(&m_ready);
        ready++;
        pthread_cond_wait(&c_event, &m_ready);
    pthread_mutex_unlock(&m_ready);
    
    pthread_mutex_lock(&m_io);
        printf("Monstro %d - Mmrawmghuhnrm...\n", id);
    pthread_mutex_unlock(&m_io);
}

void mc_build(int id) {
    mc_init(id);
    // encontra a funcao responsavel por gerar este monstro
    for(int i = 0; i < mtr_gen_i; i++) {
        if(strcmp(mtr_factory[i].species, mtr[id]->species) == 0) {
            // gera o monstro
            (*mtr_factory[i].spawn)(id);
            mtr[id]->hp = mtr[id]->hp_max;
            return;
        }
    }
    {
        // levanta um erro se nao encontrou a funcao
        char msg[200];
        sprintf(msg, "ERRO: nao foi possivel encontrar uma funcao geradora para o monstro de especie '%s'\n", mtr[id]->species);
        raise_error(msg);
    }
}

void mc_inspect(int id) {
    pthread_mutex_lock(&m_io);
        printf("Monstro %d (%s-%s):\n", id, mtr[id]->species, mtr[id]->class);
        printf("CA: %d\n", mtr[id]->ac);
        printf("Pontos de Vida: %d / %d\n", mtr[id]->hp, mtr[id]->hp_max);
        if(mtr[id]->hand_held[0])
            printf("Mao principal: %s\n", mtr[id]->hand_held[0]->name);
        if(mtr[id]->hand_held[1])
            printf("Mao secundaria: %s\n", mtr[id]->hand_held[0]->name);
        if(mtr[id]->armor[0])
            printf("Cabeca: %s\n", mtr[id]->armor[0]->name);
        if(mtr[id]->armor[1])
            printf("Torso: %s\n", mtr[id]->armor[1]->name);
        if(mtr[id]->armor[2])
            printf("Maos: %s\n", mtr[id]->armor[2]->name);
        if(mtr[id]->armor[3])
            printf("Pernas: %s\n", mtr[id]->armor[3]->name);
        if(mtr[id]->armor[4])
            printf("Pes: %s\n", mtr[id]->armor[4]->name);
        printf("\tForca: %d\n\tDestreza: %d\n\tInteligencia: %d\n\tConstituicao: %d\n\tSabedoria: %d\n\tCarisma: %d\n", mtr[id]->a_str, mtr[id]->a_dex, mtr[id]->a_int, mtr[id]->a_con, mtr[id]->a_wis, mtr[id]->a_cha);
        printf("Passivas Ativas:\n");
        pass_t *p;
        for(int i = 0; (p = (pass_t*) list_get(mtr[id]->pass, i)); i++) {
            printf("| %s\n", p->name);
        }
    pthread_mutex_unlock(&m_io);
}

void mc_gains_pass(int id, char *name) {
    push(mtr[id]->pass, pass(get_pass_n(name)), sizeof(pass_t));
}

void mc_gains_act(int id, char *name) {
    push(mtr[id]->act, act(get_act_n(name)), sizeof(act_t));
}


void monster_title(monster_t *m, char *buffer) {
    sprintf(buffer, "%s%s%s", m->species, (m->class[0] != '\0'? " " : ""), m->class);
}

void mc_combat(int id) {
    // Gerenciar recursos e opcoes
        // VERIFICACAO DE TTL AQUI

        // verifica quais acoes estao disponiveis
        int *avlb_acts = (int*) malloc(sizeof(int) * list_size(mtr[id]->act));
        int i = 0, total_acts = 0;
        for(node_t *node = *mtr[id]->act;
            node != NULL;
            node = node->next, i++
            )
        {
            // pega a acao de indice i do jogador
            act_t current_act = *(act_t*)node->data;
            // se esta acao estiver disponivel, armazena seu indice
            if((*current_act.performable)(id))
                avlb_acts[total_acts++] = i;
        }

    // Decisao de acao
        // IMPLEMENTAR ESCOLHA INTELIGENTE DE ACAO
        act_t *chosen_act = (act_t*) list_get(mtr[id]->act, avlb_acts[rand()%total_acts]);
    
    // Executa a acao
        (*chosen_act->perform)(id, 'm');
        free(avlb_acts);
}

void mc_destroy(int id) {
    if(mtr[id]->descr)
        free(mtr[id]->descr);
    if(mtr[id]->hand_held[0])
        free(mtr[id]->hand_held[0]);
    if(mtr[id]->hand_held[1])
        free(mtr[id]->hand_held[1]);
    if(mtr[id]->armor[0])
        free(mtr[id]->armor[0]);
    if(mtr[id]->armor[1])
        free(mtr[id]->armor[1]);
    if(mtr[id]->armor[2])
        free(mtr[id]->armor[2]);
    if(mtr[id]->armor[3])
        free(mtr[id]->armor[3]);
    if(mtr[id]->armor[4])
        free(mtr[id]->armor[4]);
    free_list(mtr[id]->pass);
    free_list(mtr[id]->act);
    free_list(mtr[id]->other_equips);
    free_list(mtr[id]->inventory);
    free(mtr[id]);
    // previne um bug sinistro
    mtr[id] = NULL;
    monsters--;
    pthread_mutex_destroy(&m_mtr[id]);
}


#endif