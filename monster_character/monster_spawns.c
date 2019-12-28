#ifndef MONSTER_SPAWNS_C
#define MONSTER_SPAWNS_C

#ifndef ACTIONS_C
#include "../actions.c"
#endif


#ifndef MASMORRA_H
#include "../masmorra_bib.h"
#endif


// Goblin
int spawn_goblin(int id) {
    mtr[id]->ac = 6;

    mtr[id]->hp_max = roll_dice(2, 6);

    mtr[id]->a_str = 8;
    mtr[id]->a_dex = 14;
    mtr[id]->a_con = 10;
    mtr[id]->a_int = 10;
    mtr[id]->a_wis = 8;
    mtr[id]->a_cha = 8;

    mtr[id]->hand_held[0] = (item_t *) malloc(sizeof(item_t));
    *mtr[id]->hand_held[0] = item[0];

    mc_gains_act(id, ACT_ATAQUE_BASICO);

    return 0;
}

#endif