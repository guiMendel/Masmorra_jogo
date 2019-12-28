#ifndef PLAYER_C
#define PLAYER_C

#ifndef MASMORRA_H
#include "../masmorra_bib.h"
#endif

#ifndef MASMORRA_C
#include "../masmorra_bib.c"
#endif

// Funcoes auxiliares
    void c_setrace(int id, int pick) {
        ctr[id]->race = &race[pick];
        ctr[id]->a_con += race[pick].bonus[0];
        ctr[id]->a_str += race[pick].bonus[1];
        ctr[id]->a_dex += race[pick].bonus[2];
        ctr[id]->a_int += race[pick].bonus[3];
        ctr[id]->a_wis += race[pick].bonus[4];
        ctr[id]->a_cha += race[pick].bonus[5];
    }
// Fim - Funcoes auxiliares

void pc_init(int id) {
    ctr[id]->race = NULL;
    ctr[id]->class = -1;
    ctr[id]->sex = '-';

    strcpy(ctr[id]->name, "Sem Nome");

    ctr[id]->class_choices = (char*) malloc(sizeof(char));
    ctr[id]->class_choices[0] = '\0';

    ctr[id]->subclass[0] = '\0';

    ctr[id]->pass = list_new();
    ctr[id]->act = list_new();
    ctr[id]->state[0] = 1;
    ctr[id]->state[1] = 0;
    ctr[id]->prof_bonus = 2;

    ctr[id]->armor[0] = NULL;
    ctr[id]->armor[1] = NULL;
    ctr[id]->armor[2] = NULL;
    ctr[id]->armor[3] = NULL;
    ctr[id]->armor[4] = NULL;

    ctr[id]->hand_held[0] = NULL;
    ctr[id]->hand_held[1] = NULL;
    
    ctr[id]->other_equips = list_new();

    pthread_mutex_init(&m_ctr[id], NULL);
    
    pc_gains_act(id, ACT_ATAQUE_BASICO);
    pc_gains_act(id, ACT_PASSAR_TURNO);
    pc_gains_act(id, ACT_FUGIR);
}

void pc_race(int id, int dex) {
    int disadvantage = roll_die(20) - 3*mod(dex);
    if(disadvantage<1) disadvantage = 1;
    sleep(disadvantage/10.0);
    pthread_mutex_lock(&m_io);
        printf("Personagem de Jogador %d - Cheguei! Cheguei! Levei %d decissegundos\n", id, disadvantage);
    pthread_mutex_unlock(&m_io);
}

void pc_chat(int id) {
    // input do jogador
    char input[100];

    pthread_mutex_lock(&m_io);
        printf("O que deveria dizer o Personagem de Jogador %d?\n", id);
        scanf("%[^\n]", input);
        getchar();
    pthread_mutex_unlock(&m_io);

    pthread_mutex_lock(&m_ready);
        ready++;
        pthread_cond_wait(&c_event, &m_ready);
    pthread_mutex_unlock(&m_ready);

    pthread_mutex_lock(&m_io);
        printf("Personagem de Jogador %d - %s\n", id, input);
    pthread_mutex_unlock(&m_io);
}

void pc_build(int id) {
    pc_init(id);
    // criacao manual
    if(ctr[id]->type == 1) {
        pthread_mutex_lock(&m_io);
        {
            // define os atributos
                clr_screen();
                printf("CRIACAO DE PERSONAGEM - ATRIBUTOS\nGostaria de distribuir os pontos de atributo do seu personagem %d? Caso opte por nao, eles serao distribuidos pelos dados. (s/n)\n-> ", id);
                {
                    char input;
                    input = getv(SN_PROMPT, 0);
                    if(input == 's') {
                        char warn[100] = "\0";
                        int cn = 8, st = 8, dx = 8, it = 8, ws = 8, ch = 8;
                        int distributing = 1, points = 27;
                        char s_att[50], s_val[50];
                        while(distributing) {
                            clr_screen();
                            printf("CRIACAO DE PERSONAGEM - ATRIBUTOS\nCustos de valores:\n| 05\tReembolsa 3\n| 06\tReembolsa 2\n| 07\tReembolsa 1\n| 08\t0\n| 09\t1\n| 10\t2\n| 11\t3\n| 12\t4\n| 13\t5\n| 14\t7\n| 15\t9\nEspecifique o indicie do atributo que quiser modificar, seguido por um espaco e o valor que gostaria de lhe atribuir. Para finalizar a atribuicao, digite 'fim'.\n\n1 - Constituicao: %d\n2 - Forca: %d\n3 - Destreza: %d\n4 - Inteligencia: %d\n5 - Sabedoria: %d\n6 - Carisma: %d\n\n%s(Pontos restantes: %d)-> ", cn, st, dx, it, ws, ch, warn, points);
                            strcpy(warn, "\0");
                            // le a primeira parte do input
                            scanf("%s", s_att);
                            // verifica se acabou
                            if(strcmp(s_att, "fim") == 0) {
                                distributing = 0;
                                getchar();
                            }
                            else {
                                // le o restante
                                scanf("%[^\n]", s_val);
                                // descarta o \n que sobrou no input
                                getchar();
                                
                                int att = atoi(s_att);
                                int val = atoi(s_val);
                                // valida atributo
                                if(att < 1 || att >= 7)
                                    strcpy(warn, "Por favor, insira um indice valido para o atributo.\n");
                                // valida valor
                                else if(val < 5 || val >= 16)
                                    strcpy(warn, "Por favor, insira um valor valido.\n");
                                // ambos validados
                                else {
                                    // o ponteiro target armazena a localizacao do atributo a ser modificado
                                    int old, *target, points_bkp = points;
                                    switch(att) {
                                        case 1: old = cn; target = &cn; break;
                                        case 2: old = st; target = &st; break;
                                        case 3: old = dx; target = &dx; break;
                                        case 4: old = it; target = &it; break;
                                        case 5: old = ws; target = &ws; break;
                                        case 6: old = ch; target = &ch;
                                    }
                                    *target = val;
                                    // "volta" o atributo para 8 e soma os pontos recuperados
                                    if(old <= 13)
                                        points += old - 8;
                                    else
                                        points += (old == 14? 7 : 9);
                                    // gasta os pontos para atualizar ao valor especificado
                                    if(val <= 13)
                                        points -= val - 8;
                                    else
                                        points -= (val == 14? 7 : 9);
                                    // verifica se os pontos totais foram zerados
                                    if(points < 0) {
                                        strcpy(warn, "Voce nao possui pontos suficientes para atribuir este valor.\n");
                                        *target = old;
                                        points = points_bkp;
                                    }
                                    else if(points == 0)
                                        strcpy(warn, "Todos seus pontos foram distribuidos. Reorganize suas atribuicoes ou digite 'fim' para prosseguir.\n");
                                }
                            }
                        }
                        ctr[id]->a_str = st;
                        ctr[id]->a_dex = dx;
                        ctr[id]->a_con = cn;
                        ctr[id]->a_int = it;
                        ctr[id]->a_wis = ws;
                        ctr[id]->a_cha = ch;
                    } else {
                        // distribuicao de atributos por dado
                        ctr[id]->a_str = roll_dice_advantage(4, 6, 3);
                        ctr[id]->a_dex = roll_dice_advantage(4, 6, 3);
                        ctr[id]->a_con = roll_dice_advantage(4, 6, 3);
                        ctr[id]->a_int = roll_dice_advantage(4, 6, 3);
                        ctr[id]->a_wis = roll_dice_advantage(4, 6, 3);
                        ctr[id]->a_cha = roll_dice_advantage(4, 6, 3);
                    }
                }

            // define o nome
                {
                    char input[50];
                    int sure = 0;
                    do {
                        clr_screen();
                        pc_sheet(id, 0, 0);
                        printf("CRIACAO DE PERSONAGEM - NOME\nInsira o nome de seu personagem:\n-> ");
                        scanf("%[^\n]", input);
                        getchar();
                        printf("Confirma '%s' como o nome de seu personagem? (s/n)\n-> ", input);
                        if(getv(SN_PROMPT, 0) == 's')
                            sure = 1;
                    } while(!sure);
                    strcpy(ctr[id]->name, input);
                }

            // define o sexo
                clr_screen();
                pc_sheet(id, 0, 0);
                printf("CRIACAO DE PERSONAGEM - SEXO\nSelecione o sexo de seu personagem:\nM - Masculino\nF - Feminino\n\n-> ");
                {
                    char val[2] = {'M', 'F'};
                    char input = getv(val, 0);
                    if(input == 'm')
                        ctr[id]->sex = 'o';
                    else
                        ctr[id]->sex = 'a';
                }
            
            // define a raca
                int picking = 1, confirm = 0;
                char msg[1200] = "\0";
                while(picking) {
                    clr_screen();
                    int input;
                    pc_sheet(id, 0, 0);
                    printf("CRIACAO DE PERSONAGEM - RACA\nSelecione uma das racas abaixo para visualizar mais informacoes sobre ela.\n\n");
                    for(int i = 0; i < race_i; i++) {
                        printf("%d - %s\n", i+1, race[i].name);
                    }
                    printf("\n%s-> ", msg);
                    // le a raca escolhida
                    if(!confirm) {
                        input = getint(1, race_i)-1;
                        strcpy(msg, "---------------------------------------\n");
                        strcat(msg, race[input].name);
                        strcat(msg, "\n");
                        strcat(msg, race[input].descr);
                        // impressao dos bonus
                        strcat(msg, "\nGarante os seguintes bonus ao seu personagem:");
                        if(race[input].bonus[0] != 0) {
                            strcat(msg, "\n- Constituicao: +");
                            sprintf(msg, "%s%d", msg, race[input].bonus[0]);
                        }
                        if(race[input].bonus[1] != 0) {
                            strcat(msg, "\n- Forca: +");
                            sprintf(msg, "%s%d", msg, race[input].bonus[1]);
                        }
                        if(race[input].bonus[2] != 0) {
                            strcat(msg, "\n- Destreza: +");
                            sprintf(msg, "%s%d", msg, race[input].bonus[2]);
                        }
                        if(race[input].bonus[3] != 0) {
                            strcat(msg, "\n- Inteligencia: +");
                            sprintf(msg, "%s%d", msg, race[input].bonus[3]);
                        }
                        if(race[input].bonus[4] != 0) {
                            strcat(msg, "\n- Sabedoria: +");
                            sprintf(msg, "%s%d", msg, race[input].bonus[4]);
                        }
                        if(race[input].bonus[5] != 0) {
                            strcat(msg, "\n- Carisma: +");
                            sprintf(msg, "%s%d", msg, race[input].bonus[5]);
                        }
                        strcat(msg, "\nGostaria de selecionar esta raca? (s/n)\n");
                        confirm = 1;
                    }
                    else {
                        if(getv(SN_PROMPT, 0) == 's') {
                            // atribui a raca
                            c_setrace(id, input);
                            // levanta a bandeira
                            picking = 0;
                        }
                        else {
                            confirm = 0;
                            msg[0] = '\0';
                        }
                    }
                }

            // define a classe
                picking = 1, confirm = 0;
                msg[0] = '\0';
                while(picking) {
                    clr_screen();
                    int input;
                    pc_sheet(id, 0, 0);
                    printf("CRIACAO DE PERSONAGEM - CLASSE\nSelecione uma das classes abaixo para visualizar mais informacoes sobre ela.\n\n");
                    for(int i = 0; i < class_i; i++) {
                        printf("%d - %s\n", i+1, class[i].name);
                    }
                    printf("\n%s-> ", msg);
                    // le a classe escolhida
                    if(!confirm) {
                        input = getint(1, class_i)-1;
                        strcpy(msg, "---------------------------------------\n");
                        strcat(msg, class[input].name);
                        strcat(msg, "\n");
                        strcat(msg, class[input].descr);
                        // dados de vida
                        sprintf(msg, "%s\nDado de vida: 1d%d\n", msg, class[input].hp_die);
                        // impressao dos testes de resistencia
                        strcat(msg, "Seu personagem se torna mais resiliente aos testes de:\n");
                        if(class[input].saving_throws[0] != 0) {
                            strcat(msg, "- Constituicao\n");
                        }
                        if(class[input].saving_throws[1] != 0) {
                            strcat(msg, "- Forca\n");
                        }
                        if(class[input].saving_throws[2] != 0) {
                            strcat(msg, "- Destreza\n");
                        }
                        if(class[input].saving_throws[3] != 0) {
                            strcat(msg, "- Inteligencia\n");
                        }
                        if(class[input].saving_throws[4] != 0) {
                            strcat(msg, "- Sabedoria\n");
                        }
                        if(class[input].saving_throws[5] != 0) {
                            strcat(msg, "- Carisma\n");
                        }
                        strcat(msg, "\nGostaria de selecionar esta classe? (s/n)\n");
                        confirm = 1;
                    }
                    else {
                        if(getv(SN_PROMPT, 0) == 's') {
                            // atribui a classe
                            ctr[id]->class = input;
                            (*class[input].setup)(id, input);
                            // levanta a bandeira
                            picking = 0;
                        }
                        else {
                            confirm = 0;
                            msg[0] = '\0';
                        }
                    }
                }

        }
        pthread_mutex_unlock(&m_io);
    }
    // criacao automatica
    else {
        // distribuicao de atributos por dado
            ctr[id]->a_str = roll_dice_advantage(4, 6, 3);
            ctr[id]->a_dex = roll_dice_advantage(4, 6, 3);
            ctr[id]->a_con = roll_dice_advantage(4, 6, 3);
            ctr[id]->a_int = roll_dice_advantage(4, 6, 3);
            ctr[id]->a_wis = roll_dice_advantage(4, 6, 3);
            ctr[id]->a_cha = roll_dice_advantage(4, 6, 3);

        // define o sexo
            if(rand()%2 == 0) ctr[id]->sex = 'o';
            else ctr[id]->sex = 'a';
        
        // define o nome
            sprintf(ctr[id]->name, "Aventureir%c %d", ctr[id]->sex, id + 1);

        // define a raca
            c_setrace(id, rand()%race_i);

        // define a classe
            int class_pick = rand()%class_i;
            ctr[id]->class = class_pick;
            (*class[class_pick].setup)(id, class_pick);
    }
}

void pc_chose(int id, char *name) {
    int i;
    // encontra a posicao do '\0'
    for(i = 0; ctr[id]->class_choices[i]; i++);

    // aloca espaco para a nova string (tamanho buffer antes + tamanho da string + 1, o '\n')
    ctr[id]->class_choices = (char*) realloc(ctr[id]->class_choices, sizeof(char) * (strlen(ctr[id]->class_choices) + strlen(name) + 2));

    // escreve o nome
    sprintf(ctr[id]->class_choices, "%s%s\n", ctr[id]->class_choices, name);
}

void pc_choice_get(int id, int i, char *buffer) {
    // encontra a i-esima escolha
    int j = 0;
    for(int breaks_found = 0; breaks_found != i && ctr[id]->class_choices[j] != '\0'; j++) {
        if(ctr[id]->class_choices[j] == '\n')
            breaks_found++;
    }

    // se o indice era invalido, escreve apenas o '\0'
    if(ctr[id]->class_choices[j] == '\0') {
        *buffer = '\0';
        return;
    }

    // le da variavel choices, a partir do caracter j, ate encontrar o primeiro \n, e armazena o conteudo diretamente no buffer
    sscanf(&ctr[id]->class_choices[j], "%[^\n]", buffer);
}

void pc_gains_pass(int id, char *name) {
    int pid = get_pass_n(name);
    if(pid == -1) {
        printf("Erro: não foi possível encontrar uma passiva com o nome de '%s'\n", name);
    }
    push(ctr[id]->pass, list_get(pass_l, pid), sizeof(pass_t));
}

void pc_gains_act(int id, char *name) {
    int aid = get_act_n(name);
    if(aid == -1) {
        printf("Erro: não foi possível encontrar uma ação com o nome de '%s'\n", name);
    }
    push(ctr[id]->act, list_get(act_l, aid), sizeof(act_t));
}

int player_title(int id, char *buffer) {
    int class_defined = 0;
    char c[50];
    if(ctr[id]->class == -1)
        sprintf(buffer, "Classe não definida");
    else {
        strcpy(c, class[ctr[id]->class].name);
        if(!class[ctr[id]->class].unissex)
            c[strlen(c)-1] = ctr[id]->sex;
        sprintf(buffer, "%s %s", c, ctr[id]->subclass);
        class_defined = 1;
    }

    return class_defined;
}

void pc_sheet(int id, int margin_top, int margin_bot) {
    pthread_mutex_lock(&m_io);
        while(margin_top--) printf("\n");
        char title[200];
        // descobre se o jogador ja tem classe
        int class_defined = player_title(id, title);
        // descobre se o jogador tem raça
        {
            char aux[100];
            strcpy(aux, title);
            if(ctr[id]->race == NULL)
                sprintf(title, "Sem raça, %s", aux);
            else if(ctr[id]->race->unissex)
                sprintf(title, "%s, %s", ctr[id]->race->name, aux);
            else {
                char name[50];
                strcpy(name, ctr[id]->race->name);
                name[strlen(name)-1] = ctr[id]->sex;
                sprintf(title, "%s, %s", name, aux);
            }
        }
        // imprime nome, raça e classe
        printf("%s (%s):\n", ctr[id]->name, title);
        // imprime o sexo
        printf("Sexo: ");
        if(ctr[id]->sex == 'a')
            printf("Feminino\n");
        else if(ctr[id]->sex == 'o')
            printf("Masculino\n");
        else
            printf("Nao definido\n");
        // imprime os atributos
        printf("Atributos:\n| Constituicao - %d\n| Forca - %d\n| Destreza - %d\n| Inteligencia - %d\n| Sabedoria - %d\n| Carisma - %d\n---------------------------------------\n", ctr[id]->a_con, ctr[id]->a_str, ctr[id]->a_dex, ctr[id]->a_int, ctr[id]->a_wis, ctr[id]->a_cha);
        if(class_defined) {
            // imprime ca
            printf("CA: %d\n", ctr[id]->ac);
            // imprime os pontos de vida
            printf("Pontos de Vida: %d / %d\n", ctr[id]->hp, ctr[id]->hp_max);
            // imprime os equipamentos
            printf("Equipamento:\n");
            if(ctr[id]->hand_held[0])
                printf("| Mao principal: %s\n", ctr[id]->hand_held[0]->name);
            if(ctr[id]->hand_held[1])
                printf("| Mao secundaria: %s\n", ctr[id]->hand_held[0]->name);
            if(ctr[id]->armor[0])
                printf("| Cabeca: %s\n", ctr[id]->armor[0]->name);
            if(ctr[id]->armor[1])
                printf("| Torso: %s\n", ctr[id]->armor[1]->name);
            if(ctr[id]->armor[2])
                printf("| Maos: %s\n", ctr[id]->armor[2]->name);
            if(ctr[id]->armor[3])
                printf("| Pernas: %s\n", ctr[id]->armor[3]->name);
            if(ctr[id]->armor[4])
                printf("| Pes: %s\n", ctr[id]->armor[4]->name);
            // outros equipamentos
            if(*ctr[id]->other_equips) {
                printf("| Outros:\n");
                item_t *it;
                for(int i = 0; (it = (item_t*) list_get(ctr[id]->other_equips, i)); i++) {
                    printf("|   %d - %s\n", i + 1, it->name);
                }
            }
            printf("Habilidades:\n");
            act_t *ac;
            for(int i = 0; (ac = (act_t*) list_get(ctr[id]->act, i)); i++) {
                printf("| %s\n", ac->name);
            }
            printf("Passivas Ativas:\n");
            pass_t *p;
            for(int i = 0; (p = (pass_t*) list_get(ctr[id]->pass, i)); i++) {
                printf("| %s\n", p->name);
            }
            printf("Escolhas de Classe:\n");
            (*class[ctr[id]->class].print_choices)(id);
            printf("---------------------------------------\n");
        }
        while(margin_bot--) printf("\n");
    pthread_mutex_unlock(&m_io);
}

void pc_sheet_combat(int id, int margin_top, int margin_bot) {
    pthread_mutex_lock(&m_io);
        while(margin_top--) printf("\n");
        printf("%s:\n", ctr[id]->name);
        printf("CA: %d\n", ctr[id]->ac);
        printf("Pontos de Vida: %d / %d\n", ctr[id]->hp, ctr[id]->hp_max);
        printf("Armas:\n");
        if(ctr[id]->hand_held[0])
            printf("| Mao principal: %s\n", ctr[id]->hand_held[0]->name);
        if(ctr[id]->hand_held[1])
            printf("| Mao secundaria: %s\n", ctr[id]->hand_held[0]->name);
        printf("Passivas Ativas:\n");
        pass_t *p;
        for(int i = 0; (p = (pass_t*) list_get(ctr[id]->pass, i)); i++) {
            printf("| %s\n", p->name);
        }
        printf("---------------------------------------\n");
        while(margin_bot--) printf("\n");
    pthread_mutex_unlock(&m_io);
}

void pc_combat(int id) {
    // Gerenciar recursos e opcoes
        // VERIFICACAO DE TTL AQUI

        // verifica quais acoes estao disponiveis
        int *avlb_acts = (int*) malloc(sizeof(int) * list_size(ctr[id]->act));
        int i = 0, total_acts = 0;
        for(node_t *node = *ctr[id]->act;
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
    pthread_mutex_lock(&m_io);
        int picking = 1, confirm = 0;
        char msg[1200] = "\0";
        act_t *chosen_act;
        int detailed_view = 0;
        while(picking) {
            clr_screen();
            if(detailed_view)
                pc_sheet(id, 0, 0);
            else
                pc_sheet_combat(id, 0, 0);
            printf("COMBATE - ESCOLHA DE ACAO\nEscolha uma das acoes abaixo para ver detalhes sobre ela, e confirma a que este heroi executara no seu turno.\n\n");
            for(int i = 0; i < total_acts; i++) {
                // encontra a i-esima acao na ordem em que sao especificadas pelo vetor avlb_acts
                act_t *current_act = (act_t*) list_get(ctr[id]->act, avlb_acts[i]);
                printf("| %d - %s\n", i+1, current_act->name);
            }
            printf("-----------\n| 0 - %s\n%s-> ", (detailed_view? "Ver menos detalhes sobre o personagem" : "Ver mais detalhes sobre o personagem"), msg);
            int input;
            // le a acao escolhida
            if(!confirm) {
                // os valores menores do que 1 são opções diferentes, como ver mais detalhes sobre o personagem
                // IMPLEMENTAR UM MENU MAIS COMPLEXO, COM NIVEIS DIFERENTES. APRESENTAR, POR EXEMPLO, "VER MAIS DETALHES" E "DEFINIR AÇÃO DO TURNO" E "DEFINIR AÇÃO BÔNUS DO TURNO"
                input = getint(0, total_acts)-1;
                // verifica se foi de fato uma ação escolhida
                if(input >= 0) {
                    // pega a acao escolhida
                    chosen_act = (act_t*) list_get(ctr[id]->act, avlb_acts[input]);

                    strcpy(msg, "---------------------------------------\n");
                    strcat(msg, chosen_act->name);
                    strcat(msg, "\n");
                    strcat(msg, chosen_act->descr);
                    strcat(msg, "\n");
                    // le os atuais efeitos que esta acao causa
                    {
                        char aux[700];
                        (*chosen_act->current_effects)(id, aux);
                        // ja vem com um '\n'
                        strcat(msg, aux);
                    }
                    strcat(msg, "\nRealizara esta acao? (s/n)\n");
                    confirm = 1;
                }
                else {
                    switch(input) {
                        case -1: if(detailed_view) detailed_view = 0; else detailed_view = 1; break;
                    }
                }
            }
            else {
                if(getv(SN_PROMPT, 0) == 's') {
                    // levanta a bandeira
                    picking = 0;
                }
                else {
                    confirm = 0;
                    msg[0] = '\0';
                }
            }
        }
    
    // Executa a acao
        (*chosen_act->perform)(id, 'p');
        free(avlb_acts);
}

void pc_destroy(int id) {
    free(ctr[id]->class_choices);
    if(ctr[id]->hand_held[0])
        free(ctr[id]->hand_held[0]);
    if(ctr[id]->hand_held[1])
        free(ctr[id]->hand_held[1]);
    if(ctr[id]->armor[0])
        free(ctr[id]->armor[0]);
    if(ctr[id]->armor[1])
        free(ctr[id]->armor[1]);
    if(ctr[id]->armor[2])
        free(ctr[id]->armor[2]);
    if(ctr[id]->armor[3])
        free(ctr[id]->armor[3]);
    if(ctr[id]->armor[4])
        free(ctr[id]->armor[4]);
    free_list(ctr[id]->pass);
    free_list(ctr[id]->act);
    free_list(ctr[id]->other_equips);
    free(ctr[id]);
    // previne um bug sinistro
    ctr[id] = NULL;
    player_characters--;
    pthread_mutex_destroy(&m_ctr[id]);
}



#endif