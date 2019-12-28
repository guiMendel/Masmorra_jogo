#ifndef MASMORRA_C
#define MASMORRA_C

#ifndef MASMORRA_H
#include "masmorra_bib.h"
#endif

#ifndef ACTIONS_C
#include "habilities/actions.c"
#endif

#ifndef PASSIVES_C
#include "habilities/passives.c"
#endif

#ifndef CLASSES_C
#include "player_character/classes_bib.c"
#endif

#ifndef PLAYER_C
#include "player_character/player_bib.c"
#endif

#ifndef MONSTER_C
#include "monster_character/monster_bib.c"
#endif

#ifndef LINKED_LIST_C
#include "misc/linked_list.c"
#endif

#ifndef ADVENTURER_C
#include "game_modes/adventurer.c"
#endif


////////////////////////////// VARIAVEIS GLOBAIS //////////////////////////////

    int ti = 0, monsters = 0, player_characters = 0, testing = 0, race_i = 0, class_i = 0, pass_i = 0, act_i = 0, item_i = 0, mtr_model_i = 0, mtr_gen_i = 0, flag_fallen_characters = 0, event = EVENT_PASS, ready = 0, new_ready = 0;
    char *errors_msg = NULL, *debug_msg = NULL;

////////////////////////////// FUNCOES DE THREADS //////////////////////////////

    void *player_character(void *arg) {
        // armazena o indice do personagem correspondente
        int id = *(int*) arg;
        free(arg);

        pthread_mutex_lock(&m_new_ready);
            new_ready++;
            pthread_cond_wait(&c_new, &m_new_ready);
        pthread_mutex_unlock(&m_new_ready);

        // constroi o personagem
        pc_build(id);

        // define se a execucao da thread continua
        int active = 1;
        
        while(active) {    
            // avisa a main que esta preparada e dorme
            pthread_mutex_lock(&m_ready);
                ready++;
                pthread_cond_wait(&c_event, &m_ready);
            pthread_mutex_unlock(&m_ready);

            switch(event) {
                case -3: pc_race(id, ctr[id]->a_dex); break;
                case -2: pc_chat(id); break;
                case -1:
                    pthread_mutex_lock(&m_io);
                        printf("Personagem de Jogador %d - Hello there\n", id);
                    pthread_mutex_unlock(&m_io);
                break;
                case EVENT_STATUS: pc_sheet(id, 1, 0); break;
                case EVENT_COMBAT: if(ctr[id]->state[0]) pc_combat(id); break;
                case EVENT_CLEAR_BODIES:
                    if(ctr[id]->hp <= 0) {
                        active = 0;
                        pthread_mutex_lock(&m_io);
                            printf("%s cai ao chao, inerte.\n", ctr[id]->name);
                        pthread_mutex_unlock(&m_io);
                    }
                break;
                case EVENT_END: active = 0;
            }
        }
        
        pc_destroy(id);
        return NULL;
    }

    void *monster(void *arg) {
        // armazena sua id
        int id = *(int*) arg;
        free(arg);

        pthread_mutex_lock(&m_new_ready);
            new_ready++;
            pthread_cond_wait(&c_new, &m_new_ready);
        pthread_mutex_unlock(&m_new_ready);

        // constroi o personagem
        mc_build(id);

        // define se a execucao da thread continua
        int active = 1;
        
        while(active) {    
            // avisa a main que esta preparada e dorme
            pthread_mutex_lock(&m_ready);
                ready++;
                pthread_cond_wait(&c_event, &m_ready);
            pthread_mutex_unlock(&m_ready);

            if(mtr[id]->state[0])
            switch(event) {
                case -3: mc_race(id, ctr[id]->a_dex); break;
                case -2: mc_chat(id); break;
                case -1:
                    pthread_mutex_lock(&m_io);
                        printf("Monstro %d - Brrraaawrrrgh!\n", id);
                    pthread_mutex_unlock(&m_io);
                break;
                case EVENT_STATUS:
                    if(testing) {
                        mc_inspect(id);
                    }
                break;
                case EVENT_COMBAT: mc_combat(id); break;
                case EVENT_CLEAR_BODIES:
                    if(mtr[id]->hp <= 0) {
                        active = 0;
                        pthread_mutex_lock(&m_io);
                            char title[50];
                            monster_title(mtr[id], title);
                            printf("O %s %d cai ao chao, inerte.\n", title, id+1);
                        pthread_mutex_unlock(&m_io);
                    }
                break;
                case EVENT_END: active = 0;
            }
            else active = 0;
        }
        
        mc_destroy(id);
        return NULL;
    }

////////////////////////////// FUNCOES DA MAIN //////////////////////////////
    //////////////// EVENTOS
        int event_greet() {
            // espera as threads estarem prontas
            wait_characters();
            // reseta o valor de ready enquanto as threads ainda dormem
            pthread_mutex_lock(&m_ready);
                ready = 0;
            pthread_mutex_unlock(&m_ready);
            printf("Main - Digam oi, threads!\n");
            event = -1;
            if(pthread_cond_broadcast(&c_event)) return -1;
            
            return 0;
        }
        int event_chat() {
            // espera as threads estarem prontas
            wait_characters();
            // reseta o valor de ready enquanto as threads ainda dormem
            pthread_mutex_lock(&m_ready);
                ready = 0;
            pthread_mutex_unlock(&m_ready);
            printf("Main - Threads, me contem algo interessante.\n");
            event = -2;
            if(pthread_cond_broadcast(&c_event)) return -1;
            
            // espera o jogador fornecer todas as mensagens
            wait_characters();
            ready = 0;
            if(pthread_cond_broadcast(&c_event)) return -1;
            
            return 0;
        }
        int event_status() {
            // espera as threads estarem prontas
            wait_characters();
            // reseta o valor de ready enquanto as threads ainda dormem
            pthread_mutex_lock(&m_ready);
                ready = 0;
            pthread_mutex_unlock(&m_ready);
            printf("Estado dos personagens:\n");
            event = EVENT_STATUS;
            if(pthread_cond_broadcast(&c_event)) return -1;

            return 0;
        }
        int event_race() {
            // espera as threads estarem prontas
            wait_characters();
            // reseta o valor de ready enquanto as threads ainda dormem
            pthread_mutex_lock(&m_ready);
                ready = 0;
            pthread_mutex_unlock(&m_ready);
            printf("\nMain - quem chegar por ultimo o mestre pega!\n");
            event = -3;
            if(pthread_cond_broadcast(&c_event)) return -1;

            return 0;
        }
        int event_end() {
            // espera as threads estarem prontas
            while(ready != player_characters + monsters && ready != ti) sched_yield();
            // reseta o valor de ready enquanto as threads ainda dormem
            pthread_mutex_lock(&m_ready);
                ready = 0;
            pthread_mutex_unlock(&m_ready);
            event = EVENT_END;
            if(pthread_cond_broadcast(&c_event)) return -1;

            return 0;
        }
        int event_combat() {
            wait_characters();
            printf("Os herois se deparam com um grupo de %d monstros!\nPressione enter para iniciar o combate.\n", monsters);
            wait_for_return_key();
            event = EVENT_COMBAT;
            // determina se o evento continua
            // int round = 1;
            int cont = 1;
            for(int round = 1;
                    cont && player_characters > 0 && monsters > 0;
                    round++
                )
            {
                // clear_messages();
                // Fase 1 do combate:
                //  - Main: Determinar eventos de inicio de rodada, como colapso do teto ou chegada de reforcos
                round_start(round);
                pthread_mutex_lock(&m_ready);
                    ready = 0;
                pthread_mutex_unlock(&m_ready);
                if(pthread_cond_broadcast(&c_event)) return -1;
                //  - Threads: Ao entrarem em suas funcoes de combate, gerenciar recursos, como determinar quais acoes estarao disponiveis e recalcular o ttl das acoes e passivas remanescentes
                //  - Threads: Decidir qual sera a acao tomada no turno
                //  - Threads (dentro da acao): Calcular o tempo que ira dormir ao sinal da largada
                //  - Threads (dentro da acao): Se encontram na barreira
                // Fase 2 do combate:
                //  - Main: da a largada
                wait_characters();
                clr_screen();
                pthread_mutex_lock(&m_ready);
                    ready = 0;
                pthread_mutex_unlock(&m_ready);
                if(pthread_cond_broadcast(&c_event)) return -1;
                //  - Threads (dentro da acao): Ao sinal da largada, dormem o tempo que definiram
                //  - Threads (dentro da acao): Ao fim de seu sono, executam imediatamente suas acoes predeterminadas e reacoes desencadeadas pelas acoes
                //  - Threads: Finalizam sua funcoes de combate e retornam ao posto de espera da variavel condicional
                //  - Main: Analisa o fim da rodada, quais criaturas morreram, etc
                wait_characters();
                cont = round_end(round);
                //  - Main: Inicia a proxima iteracao ou finaliza o evento
            }
            event = EVENT_PASS;
            if(player_characters == 0) {
                printf("Todos os herois pereceram...\nPressione enter para continuar.\n");
                wait_for_return_key();
            }
            else if(cont) {
                printf("Os herois conseguem enfim derrotar os monstros e prosseguem a explorar a masmorra!\nPressione enter para continuar.\n");
                wait_for_return_key();
            }
            else {
                printf("Todos os heróis conseguiram escapar do encontro.\nPressione enter para continuar.\n");
                wait_for_return_key();
            }
            // retira o estado de foragido dos jogadores
            for(int i = 0, found = 0; found < player_characters; i++) {
                if(ctr[i]) {
                    if(ctr[i]->state[0] == 0) 
                        ctr[i]->state[0] = 1;
                    found++;
                }
            }
            return 0;
        }

    void init_lists() {
        pass_l = list_new();
        act_l = list_new();
    }

    pass_t *pass(int id) {
        return (pass_t*) list_get(pass_l, id);
    }

    act_t *act(int id) {
        return (act_t*) list_get(act_l, id);
    }


    void round_start(int round) {
        return;
    }

    int round_end(int round) {
        // vai verificar se todos os jogadores fugiram
        int fled = 0;
        // limpa o campo dos monstros mortos
        if(flag_fallen_characters) {
            pthread_mutex_lock(&m_ready);
                ready = 0;
            pthread_mutex_unlock(&m_ready);
            event = EVENT_CLEAR_BODIES;
            flag_fallen_characters = 0;
            if(pthread_cond_broadcast(&c_event)) return -1;
            wait_characters();
            event = EVENT_COMBAT;
        }

        printf("---------------------------------------\nRESULTADOS DA RODADA %d\nHerois:\n", round);
        for(int i = 0, found = 0; found < player_characters; i++) {
            if(ctr[i]) {
                if(ctr[i]->state[0]) {
                    char state[50];
                    apparent_health(ctr[i]->hp, ctr[i]->hp_max, state);
                    char debug[10];
                    if(testing)
                        sprintf(debug, " (%d/%d)", ctr[i]->hp, ctr[i]->hp_max);
                    else
                        debug[0] = '\0';
                    printf("| %s - %s%s\n", ctr[i]->name, state, debug);
                }
                else {
                    printf("| %s - Foragido\n", ctr[i]->name);
                    fled++;
                }
                found++;
            }
        }
        printf("Monstros:\n");
        for(int i = 0, found = 0; found < monsters; i++) {
            // verifica se este mosntro ainda esta vivo
            if(mtr[i]) {
                char state[50];
                apparent_health(mtr[i]->hp, mtr[i]->hp_max, state);
                char debug[10];
                if(testing)
                    sprintf(debug, " (%d/%d)", mtr[i]->hp, mtr[i]->hp_max);
                else
                    debug[0] = '\0';
                char title[50];
                monster_title(mtr[i], title);
                printf("| %s %d - %s%s\n", title, i+1, state, debug);
                found++;
            }
        }
        printf("\nPressione enter para continuar.\n");
        wait_for_return_key();
        // se todos fugiram, encerrar combate
        if(fled == player_characters) {
            // coloca os mosntros no estado de foragido para que encerrem suas execução
            for(int i = 0, found = 0; found < monsters; i++) {
                // verifica se este mosntro ainda esta vivo
                if(mtr[i]) {
                    mtr[i]->state[0] = 0;
                    found++;
                }
            }
            // limpa os monstros
            pthread_mutex_lock(&m_ready);
                ready = 0;
            pthread_mutex_unlock(&m_ready);
            event = EVENT_PASS;
            if(pthread_cond_broadcast(&c_event)) return -1;
            wait_characters();
            event = EVENT_COMBAT;
            return 0;
        }
        return 1;
    }


    int free_all_resources() {
        // limpa as raças
        for(int i = 0; i < race_i; i++) {
            free(race[i].descr);
        }

        // limpa as classes
        for(int i = 0; i < class_i; i++) {
            free(class[i].descr);
        }

        // limpa os itens
        for(int i = 0; i < item_i; i++) {
            free(item[i].descr);
            free(item[i].properties);
        }

        // limpa as passivas
        for(int i = 0; i < pass_i; i++) {
            free(pass(i)->descr);
        }

        // limpa as ações
        for(int i = 0; i < act_i; i++) {
            free(act(i)->descr);
        }

        free(mtr_factory);
        if(errors_msg != NULL) free(errors_msg);
        if(debug_msg != NULL) free(debug_msg);

        return 0;
    }

    int create_pc(int manual, int n) {
        if(monsters != 0) {
            printf("ERRO - Nao e possivel criar um novo personagem de jogador enquanto ainda existirem personagens monstros ativos.\n");
            return -1;
        }
        
        for(int i = ti; i-ti < n; i++) {
            ctr[i] = (character_t*) malloc(sizeof(character_t));
            ctr[i]->type = manual;
        }
        if(create_threads(n, player_character) != 0) return -1;
        while(new_ready != n) sched_yield();
        pthread_mutex_lock(&m_new_ready);
            new_ready = 0;
        pthread_mutex_unlock(&m_new_ready);
        if(pthread_cond_broadcast(&c_new) != 0) return -1;
        
        return 0;
    }
    int create_mc(char *name, int n) {
        for(int i = monsters; i-monsters < n; i++) {
            mtr[i] = (monster_t*) malloc(sizeof(monster_t));
            sscanf(name, "%[^,], %[^\n]", mtr[i]->species, mtr[i]->class);
            if(testing) printf("---Criando um monstro da especie '%s', classe '%s'\n", mtr[i]->species, mtr[i]->class);
        }
        if(create_threads(n, monster) != 0) return -1;
        while(new_ready != n) sched_yield();
        pthread_mutex_lock(&m_new_ready);
            new_ready = 0;
        pthread_mutex_unlock(&m_new_ready);
        if(pthread_cond_broadcast(&c_new) != 0) return -1;
        
        return 0;
    }

    int factory_insert(int (*spawn)(int), char *species) {
        if((mtr_factory = (mtr_gen_t*) realloc(mtr_factory, sizeof(mtr_gen_t) * (mtr_gen_i + 1))) == NULL)
            return -1;
        strcpy(mtr_factory[mtr_gen_i].species, species);
        mtr_factory[mtr_gen_i].spawn = spawn;
        mtr_gen_i++;

        return 0;
    }

    int get_pass_n(char *name) {
        for(int i = 0; i < pass_i; i++) {
            if(strcmp(pass(i)->name, name) == 0) return i;
        }
        return -1;
    }

    int get_act_n(char *name) {
        for(int i = 0; i < act_i; i++) {
            char temp_name[150];
            strcpy(temp_name, act(i)->name);
            if(strcmp(temp_name, name) == 0) return i;
        }
        return -1;
    }

    int init_handlers() {
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        if(pthread_mutex_init(&m_io, &attr)) return -1;
        if(pthread_mutex_init(&m_ready, &attr)) return -1;
        if(pthread_mutex_init(&m_new_ready, &attr)) return -1;
        if(pthread_cond_init(&c_event, NULL)) return -1;
        if(pthread_cond_init(&c_new, NULL)) return -1;
        return 0;
    }

    int destroy_handlers() {
        if(pthread_mutex_destroy(&m_io)) return -1;
        if(pthread_mutex_destroy(&m_ready)) return -1;
        if(pthread_mutex_destroy(&m_new_ready)) return -1;
        if(pthread_cond_destroy(&c_event)) return -1;
        if(pthread_cond_destroy(&c_new)) return -1;
        pthread_mutexattr_destroy(&attr);
        return 0;
    }

    int create_threads(int n, void *f) {
        for(int oldi = ti; ti < oldi+n; ti++) {
            // vai ser passado para a thread saber sua id
            int *id = (int*) malloc(sizeof(int));
            if(f == player_character) *id = player_characters++;
            else if(f == monster) *id = monsters++;
            // se nao for monstro nem jogador, provavelmente e uma thread de teste
            else *id = ti;
            if(pthread_create(&ts[ti], NULL, f, (void*) id) != 0) return -1;
        }
        return 0;
    }

    int join_threads(int n) {
        for(int oldi = ti; ti > oldi-n; ti--) {
            if(pthread_join(ts[ti-1], NULL) != 0) return -1;
        }
        return 0;
    }

    int roll_dice_advantage(int total, int sides, int effective) {
        int val[50], best[50];
        // inicializa o vetor best com 0s

        for(int i = 0; i < effective; i++) {
            best[i] = 0;
        }
        for(int i = 0; i < total; i++) {
            int result = roll_die(sides);
            // armazena o resultado se for um dos maiores
            for(int j = 0; j < effective; j++) {
                if(result > best[j]) {
                    int aux = best[j];
                    best[j] = result;
                    if((result = aux) == 0) break;
                }
            }
        }
        int ret = 0;
        for(int i = 0; i < effective; i++) {
            ret += best[i];
        }
        return ret;
    }

    int roll_die(int sides) {
        return 1+rand()%sides;
    }

    int roll_dice(int n, int sides) {
        int sum = 0;
        for(int i = 0; i < n; i++) {
            sum += roll_die(sides);
        }

        return sum;
    }

    void apparent_health(int hp, int total, char *buffer) {
        float fraction = ((float)hp)/((float)total);
        if(fraction > 2.0/3.0)
            strcpy(buffer, "Aparentemente bem");
        else if(fraction > 1.0/3.0)
            strcpy(buffer, "Ferido");
        else
            strcpy(buffer, "Gravemente ferido");
    }

    void barrier(int id, char c) {
        // calcula o tempo que ira dormir
        int disadvantage;
        if(c == 'p')
            disadvantage = roll_die(20) - 3*mod(ctr[id]->a_dex);
        else
            disadvantage = roll_die(20) - 3*mod(mtr[id]->a_dex);
        if(disadvantage<0) disadvantage = 0;

        if(c == 'p') pthread_mutex_unlock(&m_io);
        
        // barreira de sincronizacao
        pthread_mutex_lock(&m_ready);
            ready++;
            pthread_cond_wait(&c_event, &m_ready);
        pthread_mutex_unlock(&m_ready);

        // dormir
        sleep(disadvantage/10.0);
    }
    
    char getv(char *val, int capital) {
        char a, b[50];
        char input[50];
        int burro = 0, invalid = 1;

        // diminui a caixa das letras
        if(!capital) {
            for(int i = 0; val[i]; i++){
                b[i] = tolower(val[i]);
            }
        } else {
            strcpy(b, val);
        }

        
        do {
            if(burro) printf("Por favor, digite uma resposta valida!\n-> ");
            else burro = 1; // Sistema anti-burragem
            scanf("%[^\n]", input);
            getchar();

            // se nao tiver so um caracter, ja esta errado
            if(!input[1]) {

                // ignora as maiusculas
                if(!capital) {
                    a = tolower(input[0]);
                } else {
                    a = input[0];
                }

                // realiza a validacao
                for(int i = 0; b[i] && invalid; i++) {
                    if(a == b[i]) invalid = 0;
                }
            }

        } while(invalid);
        return a;
    }

    int getint(int min, int max) {
        int burro = 1, ret;
        char warn[100] = "\0";
        do {
            char input[50];
            printf("%s", warn);
            scanf("%[^\n]", input);
            getchar();
            ret = atoi(input);
            if(input[0] != '0' && ret == 0) {
                strcpy(warn, "Por favor, digite uma resposta valida!\n-> ");
            }
            else if(ret < min || ret > max) {
                strcpy(warn, "Por favor, digite um numero dentro do alcance permitido\n-> ");
            }
            else burro = 0;
        } while(burro);
        return ret;
    }

    void raise_error(char *msg) {
        // inserindo a primeira mensagem?
        if(errors_msg == NULL) {
            errors_msg = (char*) malloc(sizeof(char) * (strlen(msg) + 1));
            errors_msg[0] = '\0';
        }
        else
            errors_msg = (char*) realloc(errors_msg, sizeof(char) * (strlen(errors_msg) + strlen(msg) + 1));
        sprintf(errors_msg, "%s%s\n", errors_msg, msg);
    }

    void clr_screen() {
        if(!testing)
            system("clear");
        if(errors_msg != NULL)
            printf("%s!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", errors_msg);
        if(debug_msg != NULL) {
            printf("%s########################################\n", debug_msg);
        }
    }
#endif