#ifndef ACTIONS_C
#define ACTIONS_C

#ifndef MASMORRA_H
#include "../masmorra_bib.h"
#endif

// definição das macros
#define ACT_PROTECAO "Proteção"
#define ACT_SURTO_ACAO "Surto de Ação"
#define ACT_RETOMAR_FOLEGO "Retomar o Fôlego"
#define ACT_ATAQUE_BASICO "Ataque Básico"
#define ACT_PASSAR_TURNO "Passar Turno"
#define ACT_FUGIR "Fugir"

// ADICIONANDO UMA ACAO
// Basta adicionar uma funcao act, uma check e uma effects, e entao adicionar ela na definição de load_act, utilizando a função add_act
// A act define o comportamento da acao, e deve obrigatoriamente conter uma barreira antes da execucao de fato da acao
// A check é invocada para saber se a ação está disponível num evento e num contexto
// A effects descreve quais serão algumas características da realização desta ação, dado o contexto

// Ataque Basico

    int act_ataque_basico(int id, char c) {
        // GANCHOS DE PASSIVA AQUI

        // VERIFICAR QUANTAS CRIATURAS DE CADA ESPECIE TEM E ATRIBUIR A ELAS INDICES PARA DIFERENCIAR NA ESCOLHA DO ALVO, ASSIM COMO DECIDIR SE USA "um", OU "o" NA NARRACAO
        int target;
        if(c == 'p') {
                printf("Qual criatura sera o alvo?\n");
                int i, found;
                for(i = 0, found = 0; found < monsters; i++) {
                // verifica se este mosntro ainda esta vivo
                    if(mtr[i]) {
                        char app_hp[20];
                        char title[70];
                        char debug[10];
                        if(testing)
                            sprintf(debug, " (%d/%d)", mtr[i]->hp, mtr[i]->hp_max);
                        else
                            debug[0] = '\0';
                        apparent_health(mtr[i]->hp, mtr[i]->hp_max, app_hp);
                        monster_title(mtr[i], title);
                        printf("%d - %s: %s%s\n", i+1, title, app_hp, debug);
                        found++;
                    }
                }
                // recebe um indice de mosntro e verifica se ele ainda esta vivo
                while(!mtr[(target = getint(1, i)-1)])
                    printf("Por favor, selecione um monstro que ainda esteja vivo.\n-> ");
        }
        else {
            // IMPLEMENTAR AQUI O MONSTRO PODER ATACAR AQUELE QUE PARECE MAIS FERIDO
            // gera aleatoriamente um alvo ate gerar um q n esteja morto nem foragido
            int *avlb_targets = (int*) malloc(sizeof(int) * player_characters);
            int total_avlb_targets = 0;
            for(int i = 0, found = 0; found < player_characters; i++) {
            // verifica se este jogador ainda esta vivo
                if(ctr[i]) {
                    // verifica se o jogador nao esta foragido
                    if(ctr[i]->state[0])
                        avlb_targets[total_avlb_targets++] = i;
                    found++;
                }
            }
            if(total_avlb_targets == 0) target = -1;
            else target = avlb_targets[rand()%total_avlb_targets];
            free(avlb_targets);
        }

        // espera pela largada
        barrier(id, c);
        // largada
        // VERIFICAR PRECONDICOES DINAMICAS, COMO SE O ALVO JA ESTA MORTO OU SE FUGIU

        // target será -1 em casos de exceção
        if(target == -1) return 0;

        int attack_roll = roll_die(20);
        int str_bonus;
        if(c == 'p')
            str_bonus = mod(ctr[id]->a_str);
        else
            str_bonus = mod(mtr[id]->a_str);
        // VERIFICAR SE A ARMA TEM ACUIDADE E COMPARAR BONUS DE DEX COM STR

        int bonus = str_bonus;
        char title[70];
        if(c == 'p')
            monster_title(mtr[target], title);
        else
            monster_title(mtr[id], title);

        // PREVENIR CASO O JOGADOR NAO ESTEJA SEGURANDO NADA, DAR DANO DE ATAQUE DESARMADO
        // se agora este personagem ja estiver morto, nao realiza sua acao
        if(c == 'p') {
            // mortos não atacam
            if(ctr[id]->hp <= 0) return 0;
            // verifica se o alvo acabou de fugir
            if(mtr[target]->state[0] == 0) {
                pthread_mutex_lock(&m_io);
                    printf("%s tenta acertar %s %d, mas é lento e o monstro foge.\n", ctr[id]->name, title, target+1);
                pthread_mutex_unlock(&m_io);
                return 0;
            }
        }
        else {
            // mortos não atacam
            if(mtr[id]->hp <= 0) return 0;
            // verifica se o alvo acabou de fugir
            if(ctr[target]->state[0] == 0) {
                pthread_mutex_lock(&m_io);
                    printf("%s %d tenta acertar %s, mas é lento e o herói escapa.\n", title, id+1, ctr[target]->name);
                pthread_mutex_unlock(&m_io);
                return 0;
            }
        }
        
        if(c == 'p') {
            if(mtr[target]->ac >= attack_roll + bonus && attack_roll != 20) {
                pthread_mutex_lock(&m_io);
                    printf("%s tenta acertar %s %d, mas seu golpe e defletido.\n", ctr[id]->name, title, target+1);
                pthread_mutex_unlock(&m_io);
            }
            else {
                // rola os dados de dano
                int damage = roll_dice(ctr[id]->hand_held[0]->effect_value[0], ctr[id]->hand_held[0]->effect_value[1]);
                // adiciona os modificadores
                // ASSUME-SE QUE O JOGADOR E PROFICIENTE, DEPOIS IMPLEMENTAR SISTEMA DE PROFICIENCIA
                damage += 2 + mod(ctr[id]->a_str);
                if(attack_roll == 20) {
                    // critico
                    // descobre o dano
                    damage += ctr[id]->hand_held[0]->effect_value[0] * ctr[id]->hand_held[0]->effect_value[1];
                    // aplica o dano
                    pthread_mutex_lock(&m_mtr[target]);
                        mtr[target]->hp -= damage;
                        if(mtr[target]->hp <= 0)
                            flag_fallen_characters = 1;
                    pthread_mutex_unlock(&m_mtr[target]);
                    
                    pthread_mutex_lock(&m_io);
                        printf("Num golpe devastador, %s lanca %s %d a cambalear para tras, pintando o chao entre eles de vermelho.\n", ctr[id]->name, title, target+1);
                        if(testing) {
                            printf("---Dano: %d\n", damage);
                        }
                    pthread_mutex_unlock(&m_io);
                }
                else {
                    // nao critico
                    // aplica o dano
                    pthread_mutex_lock(&m_mtr[target]);
                        mtr[target]->hp -= damage;
                        if(mtr[target]->hp <= 0)
                            flag_fallen_characters = 1;
                    pthread_mutex_unlock(&m_mtr[target]);
                    
                    pthread_mutex_lock(&m_io);
                        printf("%s alveja %s %d, que urra em furia.\n", ctr[id]->name, title, target+1);
                        if(testing) {
                            printf("---Dano: %d\n", damage);
                        }
                    pthread_mutex_unlock(&m_io);

                }
            }
        }
        else {
            if(ctr[target]->ac >= attack_roll + bonus && attack_roll != 20) {
                pthread_mutex_lock(&m_io);
                    printf("%s %d tenta acertar %s, mas seu golpe e defletido.\n", title, id+1, ctr[target]->name);
                pthread_mutex_unlock(&m_io);

            }
            else {
                // rola os dados de dano
                int damage = roll_dice(mtr[id]->hand_held[0]->effect_value[0], mtr[id]->hand_held[0]->effect_value[1]);
                // adiciona os modificadores
                // ASSUME-SE QUE O JOGADOR E PROFICIENTE, DEPOIS IMPLEMENTAR SISTEMA DE PROFICIENCIA
                damage += 2 + mod(mtr[id]->a_str);
                if(attack_roll == 20) {
                    // critico
                    // descobre o dano
                    damage += mtr[id]->hand_held[0]->effect_value[0] * mtr[id]->hand_held[0]->effect_value[1];
                    // aplica o dano
                    pthread_mutex_lock(&m_ctr[target]);
                        ctr[target]->hp -= damage;
                        if(ctr[target]->hp <= 0)
                            flag_fallen_characters = 1;
                    pthread_mutex_unlock(&m_ctr[target]);
                    
                    pthread_mutex_lock(&m_io);
                        printf("Num golpe devastador, %s %d lanca %s a cambalear para tras, pintando o chao entre eles de vermelho.\n", title, id+1, ctr[target]->name);
                        if(testing) {
                            printf("---Dano: %d\n", damage);
                        }
                    pthread_mutex_unlock(&m_io);

                }
                else {
                    // nao critico
                    // aplica o dano
                    pthread_mutex_lock(&m_ctr[target]);
                        ctr[target]->hp -= damage;
                        if(ctr[target]->hp <= 0)
                            flag_fallen_characters = 1;
                    pthread_mutex_unlock(&m_ctr[target]);
                    
                    pthread_mutex_lock(&m_io);
                        printf("%s %d alveja %s, que urra em furia.\n", title, id+1, ctr[target]->name);
                        if(testing) {
                            printf("---Dano: %d\n", damage);
                        }
                    pthread_mutex_unlock(&m_io);
                }
            }
        }
        
        return 0;
    }
    int check_ataque_basico(int id) {
        return 1;
    }
    void effects_ataque_basico(int id, char *buffer) {
        // encontra o bonus de prof do jogador, mas insere 0 se ele nao for proficiente
        // INSERIR 0 SE O JOGADOR NAO FOR PROFICIENTE
        int prof_bonus = ctr[id]->prof_bonus;
        // encontra o modificador de ataque apropriado
        // DEFINIR QUE ELE VE SE E A DISTANCIA, CASO EM QUE USA DESTREZA, OU SE A ARMA POSSUI ACUIDADE E UM MAIOR BONUS DE DESTREZA
        int att_bonus = mod(ctr[id]->a_str);
        // encontra o bonus de dano. E o modificador de forca ou o de destreza se a arma possuir acuidade e o mod de destreza for maior
        // FAZER VERIFICACAO DE ACUIDADE
        int dmg_bonus = mod(ctr[id]->a_str);

        sprintf(buffer, "| Modificador da jogada de ataque: %d\n| Dano resultante: %dd%d + %d\n", prof_bonus + att_bonus, ctr[id]->hand_held[0]->effect_value[0], ctr[id]->hand_held[0]->effect_value[1], dmg_bonus);
    }

// Passar Turno

    int act_passar_turno(int id, char c) {
        // espera pela largada
        barrier(id, c);
        // largada

        if(c == 'p') {
            pthread_mutex_lock(&m_io);
                printf("%s apenas hesita.\n", ctr[id]->name);
            pthread_mutex_unlock(&m_io);
        }
        else {
            char title[50];
            monster_title(mtr[id], title);
            pthread_mutex_lock(&m_io);
                printf("%s %d apenas hesita.\n", title, id+1);
            pthread_mutex_unlock(&m_io);
        }

        return 0;
    }
    int check_passar_turno(int id) {
        return 1;
    }
    void effects_passar_turno(int id, char *buffer) {
        buffer[0] = '\0';
        return;
    }

// Fugir

    int act_fugir(int id, char c) {
        // calcula a cd do teste de destreza
        int cd;
        if(c == 'p') {
            if(monsters > 10)
                cd = 15;
            else if(monsters > 6)
                cd = 14;
            else if(monsters > 4)
                cd = 13;
            else if(monsters > 2)
                cd = 12;
            else
                cd = 11;
        }
        else {
            if(player_characters > 4)
                cd = 18;
            else if(player_characters > 3)
                cd = 17;
            else if(player_characters > 2)
                cd = 16;
            else
                cd = 15;
        }
        
        // espera pela largada
        barrier(id, c);
        // largada

        if(c == 'p') {
            // mortos não fogem
            if(ctr[id]->hp <= 0) return 0;
        }
        else {
            // mortos não fogem
            if(mtr[id]->hp <= 0) return 0;
        }

        if(c == 'p') {
            if(roll_die(20) + mod(ctr[id]->a_dex) > cd) {
                ctr[id]->state[0] = 0;
                pthread_mutex_lock(&m_io);
                    printf("%s escapa da batalha.\n", ctr[id]->name);
                pthread_mutex_unlock(&m_io);
            }
            else {
                // define qual monstro foi responsável por impedir a fuga do personagem
                int dickhead = -1;
                for(int i = 0, found = 0; found < monsters; i++) {
                // verifica se este monstro ainda esta vivo
                    if(mtr[i]) {
                        if(dickhead == -1)
                            dickhead = i;
                        else if(roll_die(monsters) == 1)
                            dickhead = i;
                        found++;
                    }
                }
                char title[50];
                monster_title(mtr[dickhead], title);
                pthread_mutex_lock(&m_io);
                    printf("%s %d impede %s de escapar.\n", title, dickhead+1, ctr[id]->name);
                pthread_mutex_unlock(&m_io);
            }
        }
        else {
            char title[50];
            monster_title(mtr[id], title);
            if(roll_die(20) + mod(mtr[id]->a_dex) > cd) {
                mtr[id]->state[0] = 0;
                pthread_mutex_lock(&m_io);
                    printf("%s %d escapa da batalha.\n", title, id);
                pthread_mutex_unlock(&m_io);
            }
            else {
                int dickhead = -1;
                for(int i = 0, found = 0; found < player_characters; i++) {
                // verifica se este jogador ainda esta vivo
                    if(ctr[i]) {
                        if(dickhead == -1)
                            dickhead = i;
                        else if(roll_die(player_characters) == 1)
                            dickhead = i;
                        found++;
                    }
                }
                pthread_mutex_lock(&m_io);
                    printf("%s impede %s %d de escapar.\n", ctr[dickhead]->name, title, id+1);
                pthread_mutex_unlock(&m_io);
            }
        }

        return 0;
    }
    int check_fugir(int id) {
        return 1;
    }
    void effects_fugir(int id, char *buffer) {
        buffer[0] = '\0';
        return;
    }
// Proteção

    int act_protecao(int id, char c) {
        return 0;
    }
    int check_protecao(int id) {
        return 1;
    }
    void effects_protecao(int id, char *buffer) {
        buffer[0] = '\0';
    }
// Retomar o Fôlego"Retomar o Fôlego"

    int act_retomar_folego(int id, char c) {
        return 0;
    }
    int check_retomar_folego(int id) {
        return 1;
    }
    void effects_retomar_folego(int id, char *buffer) {
        buffer[0] = '\0';
    }
// Surto de Ação

    int act_surto_acao(int id, char c) {
        return 0;
    }
    int check_surto_acao(int id) {
        return 1;
    }
    void effects_surto_acao(int id, char *buffer) {
        buffer[0] = '\0';
    }
// Exemplo

    int act_ex(int id, char c) {
        return 0;
    }
    int check_ex(int id) {
        return 1;
    }
    void effects_ex(int id, char *buffer) {
        buffer[0] = '\0';
    }
    
void add_act(char *name, char *desc, int (*perform)(int, char), int (*performable)(int), void (*effects)(int, char*)) {
    act_t new_act;
    strcpy(new_act.name, name);
    new_act.descr = (char*) malloc(sizeof(char) * (strlen(desc) + 1));
    strcpy(new_act.descr, desc);
    new_act.perform = perform;
    new_act.performable = performable;
    new_act.current_effects = effects;
    push(act_l, (void*) &new_act, sizeof(act_t));
    act_i++;
}

void load_act() {
    add_act(ACT_ATAQUE_BASICO, "O personagem realiza um ataque, com a arma equipada em sua mao principal, contra uma determinada criatura.", act_ataque_basico, check_ataque_basico, effects_ataque_basico);

    add_act(ACT_PASSAR_TURNO, "O personagem passa esta rodada sem realizar nenhuma ação.", act_passar_turno, check_passar_turno, effects_passar_turno);

    add_act(ACT_FUGIR, "O personagem tenta escapar do combate.\nQuanto mais monstros ainda houver em combate, menor a chance de sucesso.", act_fugir, check_fugir, effects_fugir);

    add_act(ACT_PROTECAO, "No seu turno, caso possua um escudo, voce pode escolher um personagem nao hostil. Qualquer outro personagem que tentar ataca-lo sofre desvantagem.", act_protecao, check_protecao, effects_protecao);

    add_act(ACT_SURTO_ACAO, "Voce pode forçar o seu limite para além do normal por um momento. Durante o seu turno, voce pode realizar uma ação adicional juntamente com sua ação normal e possivel ação bonus.\nUma vez que usar essa caracteristica, voce precisará terminar um descanso curto ou longo para usá-la novamente.", act_surto_acao, check_surto_acao, effects_surto_acao);

    add_act(ACT_RETOMAR_FOLEGO, "Voce possui uma reserva de estamina e pode usa-la para proteger a si mesmo contra danos. No seu turno, voce pode usar uma acao bonus para recuperar pontos de vida igual a 1d10 + seu nivel.\nUma vez que usar essa caracteristica, voce precisara terminar um descanso curto ou longo para usa-la novamente.", act_retomar_folego, check_retomar_folego, effects_retomar_folego);
}

#endif