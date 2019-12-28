#ifndef CLASSES_C
#define CLASSES_C

#ifndef MASMORRA_H
#include "../masmorra_bib.h"
#endif

#ifndef MASMORRA_C
#include "../masmorra_bib.c"
#endif

// GUERREIRO
    void setup_guerreiro(int id, int self) {
        int roll = roll_dice(2, class[0].hp_die);
        ctr[id]->hp_max = class[self].hp_die + roll + 3*mod(ctr[id]->a_con);
        ctr[id]->hp = ctr[id]->hp_max;

        // Estilo de Luta

            if(ctr[id]->type == 1) {
                // se o jogador ainda esta escolhendo sua opção; se esta num ciclo que apenas confirma com o jogador sua escolha; se o jogador escolheu um bônus passivo (0) ou ativo (1)
                int picking = 1, confirm = 0, type;
                char msg[1200] = "\0", option[50];
                while(picking) {
                    clr_screen();
                    int input;
                    pc_sheet(id, 0, 0);
                    printf("CRIACAO DE PERSONAGEM - GUERREIRO, ESTILO DE LUTA\nSeu personagem adota um estilo de combate particular que sera sua especialidade. Escolha uma das opcões a seguir para visualizar mais informacoes sobre ela:\n\n");
                    printf("1 - Arquearia\n2 - Combate com Armas Grandes\n3 - Combate com Duas Armas\n4 - Defesa\n5 - Duelismo\n6 - Protecao\n");
                    printf("\n%s-> ", msg);
                    // le o estilo escolhido
                    if(!confirm) {
                        input = getint(1, 6);
                        // armazena as informacoes do estilo selecionado
                        {
                            switch(input) {
                                case 1:
                                    strcpy(option, PASS_ARQUEARIA);
                                    type = 0;
                                break;
                                case 2:
                                    strcpy(option, PASS_COMBATE_COM_ARMAS_GRANDES);
                                    type = 0;
                                break;
                                case 3:
                                    strcpy(option, PASS_COMBATE_COM_DUAS_ARMAS);
                                    type = 0;
                                break;
                                case 4:
                                    strcpy(option, PASS_DEFESA);
                                    type = 0;
                                break;
                                case 5:
                                    strcpy(option, PASS_DUELISMO);
                                    type = 0;
                                break;
                                case 6:
                                    strcpy(option, ACT_PROTECAO);
                                    type = 1;
                                break;
                            }
                        }
                        strcpy(msg, "---------------------------------------\n");
                        strcat(msg, option);
                        strcat(msg, "\n");
                        // ação
                        if(type)
                            strcat(msg, act(get_act_n(option))->descr);
                        // passiva
                        else
                            strcat(msg, pass(get_pass_n(option))->descr);
                        strcat(msg, "\nGostaria de selecionar este estilo? (s/n)\n");
                        confirm = 1;
                    }
                    else {
                        if(getv(SN_PROMPT, 0) == 's') {
                            // atribui o estilo
                            pc_chose(id, option);
                            // ação
                            if(type)
                                pc_gains_act(id, option);
                            // passiva
                            else
                                pc_gains_pass(id, option);
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
            else {
                switch(rand()%6) {
                    case 0: pc_chose(id, PASS_ARQUEARIA); pc_gains_pass(id, PASS_ARQUEARIA); break;
                    case 1: pc_chose(id, PASS_COMBATE_COM_ARMAS_GRANDES); pc_gains_pass(id, PASS_COMBATE_COM_ARMAS_GRANDES); break;
                    case 2: pc_chose(id, PASS_COMBATE_COM_DUAS_ARMAS); pc_gains_pass(id, PASS_COMBATE_COM_DUAS_ARMAS); break;
                    case 3: pc_chose(id, PASS_DEFESA); pc_gains_pass(id, PASS_DEFESA); break;
                    case 4: pc_chose(id, PASS_DUELISMO); pc_gains_pass(id, PASS_DUELISMO); break;
                    case 5: pc_chose(id, ACT_PROTECAO); pc_gains_act(id, ACT_PROTECAO);
                }
            }

        // Retomar o Folego
        
            pc_gains_act(id, ACT_RETOMAR_FOLEGO);

        // arquétipo Marcial

            if(ctr[id]->type == 1) {
                int picking = 1, confirm = 0, type;
                char msg[1200] = "\0", option[50];
                char txt[500], arq[50];
                while(picking) {
                    clr_screen();
                    int input;
                    pc_sheet(id, 0, 0);
                    printf("CRIACAO DE PERSONAGEM - GUERREIRO, arquétipo MARCIAL\nEscolha um arquétipo que rege as tecnicas marciais de seu personagem. Selecione uma das opcões a seguir para visualizar mais informacoes sobre ela:\n\n");
                    printf("1 - Campeão\n");
                    printf("\n%s-> ", msg);
                    // le o arquétipo escolhido
                    if(!confirm) {
                        input = getint(1, 1);
                        // armazena as informacoes do arquétipo selecionado
                        {
                            switch(input) {
                                case 1:
                                    strcpy(option, PASS_CRITICO_APRIMORADO);
                                    strcpy(arq, "Campeão");
                                    strcpy(txt, "O arquétipo Campeão foca no desenvolvimento da pura força física acompanhada por uma perfeição mortal. Aqueles que trilham o caminho desse arquétipo combinam rigorosos treinamentos com excelência física para desferir golpes devastadores.");
                                    type = 0;
                            }
                        }
                        strcpy(msg, "---------------------------------------\n");
                        sprintf(msg, "%s%s\n%s\nBônus do arquétipo:\n| %s: ", msg, arq, txt, option);
                        strcat(msg, pass(get_pass_n(option))->descr);
                        strcat(msg, "\nGostaria de selecionar este arquétipo? (s/n)\n");
                        confirm = 1;
                    }
                    else {
                        if(getv(SN_PROMPT, 0) == 's') {
                            // atribui o arquétipo
                            pc_chose(id, arq);
                            strcpy(ctr[id]->subclass, arq);
                            if(type)
                                pc_gains_act(id, option);
                            else
                                pc_gains_pass(id, option);
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
            else {
                switch(rand()%1) {
                    case 0: pc_chose(id, "Campeão"); strcpy(ctr[id]->subclass, "Campeão"); pc_gains_pass(id, PASS_CRITICO_APRIMORADO);
                }
            }

        // Define equipamento inicial

            if(ctr[id]->type == 1) {
                int picking = 1, confirm = 0;
                char msg[1200] = "\0", option[50];
                while(picking) {
                    clr_screen();
                    int input;
                    pc_sheet(id, 0, 0);
                    printf("CRIACAO DE PERSONAGEM - GUERREIRO, EQUIPAMENTO INICIAL\nDefina qual foi o equipamento de escolha de seu guerreiro ao partir para esta expedicao. Selecione uma das opcões a seguir para visualizar mais informacoes sobre ela:\n\n");
                    printf("1 - Espada Longa\n");
                    printf("\n%s-> ", msg);
                    // le o equipamento escolhido
                    if(!confirm) {
                        input = getint(1, 1) - 1;
                        // armazena as informacoes do equipamento selecionado
                        // {
                        //     switch(input) {
                        //         case 1:
                        //             strcpy(option, PASS_CRITICO_APRIMORADO);
                        //             strcpy(arq, "Campeão");
                        //             strcpy(txt, "O arquétipo Campeão foca no desenvolvimento da pura força física acompanhada por uma perfeição mortal. Aqueles que trilham o caminho desse arquétipo combinam rigorosos treinamentos com excelência física para desferir golpes devastadores.");
                        //     }
                        // }
                        strcpy(msg, "---------------------------------------\n");
                        sprintf(msg, "%s%s\n%s\nDado de Dano: 1d8\nPropriedades:\n| Versatil", msg, item[input].name, item[input].descr);
                        strcat(msg, "\nGostaria de selecionar este equipamento? (s/n)\n");
                        confirm = 1;
                    }
                    else {
                        if(getv(SN_PROMPT, 0) == 's') {
                            // atribui o equipamento
                            ctr[id]->hand_held[0] = (item_t*) malloc(sizeof(item_t));
                            *(ctr[id]->hand_held[0]) = item[0];
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
            else {
                switch(rand()%1) {
                    case 0: ctr[id]->hand_held[0] = (item_t*) malloc(sizeof(item_t));*(ctr[id]->hand_held[0]) = item[0]; break;
                }
            }
            ctr[id]->ac = 10 + mod(ctr[id]->a_dex);

        if(ctr[id]->type == 1) {
        // Prompt de confirmacao
            clr_screen();
            printf("FICHA DE PERSONAGEM\n");
            pc_sheet(id, 0, 0);
            printf("Pressione enter para prosseguir.\n");
            scanf("%*[^\n]");
            getchar();
        }
    }

    void choices_guerreiro(int id) {
        // executa para cada uma de suas escolhas
        // estilo de luta
        char choice[50];
        pc_choice_get(id, 0, choice);
        if(choice[0])
            printf("| Estilo de luta: %s\n", choice);
        else return;
        // arquétipo marcial
        pc_choice_get(id, 1, choice);
        if(choice[0])
            printf("| arquétipo marcial: %s\n", choice);
        else return;
        return;
    }
        
#endif