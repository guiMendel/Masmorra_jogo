#ifndef ADVENTURER_C
#define ADVENTURER_C

#ifndef ADVENTURER_H
#include "adventurer.h"
#endif

void play_adventurer() {
    // numero de herois
    int heroes,
    // numero de herois personalizados
    custom,
    // tipo de cenario
    scenario;

    
    clr_screen();
    printf("MODO AVENTUREIRO - CRIAÇÃO DE JOGO\nQuantos heróis fazem parte de sua expedição? (1 a 5)\n-> ");
    // le N do jogador
        heroes = getint(1, 5);
        if(heroes != 1) {
            printf("Quantos dos %d personagens você gostaria de customizar?\n", heroes);
            custom = getint(0, heroes);
        }
        else {
            printf("Gostaria de customizar seu lobo solitário?\n");
            char input = getv(SN_PROMPT, 0);
            if(input == 's')
                custom = 1;
            else
                custom = 0;
        }
        


    // cria N personagens de jogador
        create_pc(1, custom);
        create_pc(0, heroes - custom);

    // mostra na tela informacoes sobre os n herois
        wait_characters();
        clr_screen();
        printf("Revise as fichas de seus personagens:\n");
        event_status();
        wait_characters();
        printf("Pressione enter para continuar.\n");
        wait_for_return_key();

    // seleciona o ambiente
    // PROGRAMADO RIGIDAMENTE PARA ACEITAR SOMENTE MASMORRA CAVERNOSA. TORNAR MAIS FELXIVEL!
        clr_screen();
        printf("Selecione qual cenário sua expedicao irá explorar:\n1 - Masmorras cavernosas\n-> ");
        scenario = getint(SC_CAVE_DUNGEON, SC_CAVE_DUNGEON);

    // inicia o ciclo de encontros
        // IMPLEMENTAR OUTROS ENCONTROS ALEM DO COMBATE
        int exploring = 1, room = 1;
        while(exploring) {
            clr_screen();
            printf("Os heróis descendem os escuros e gelados corredores da masmorra, e cada passo que dão os aproximam dos inquietantes sons que ecoam das profundezas da caverna...\n");
            sleep(2);
            // IMPLEMENTAR GERAÇÃO INTELIGENTE DE ENCONTRO
            int n_mtrs = roll_dice(player_characters, 2+room);
            create_mc("Goblin", n_mtrs);
            event_combat();
            join_threads(n_mtrs);
            
            if(player_characters) {
                // confirma se o jogador quer continuar
                clr_screen();
                event_status();
                wait_characters();
                printf("Gostaria de continuar explorando a masmorra e seguir para a área %d? (s/n)\n-> ", room + 1);
                char input = getv(SN_PROMPT, 0);
                if(input == 'n')
                    exploring = 0;
                else
                    room++;
            }
            else
                exploring = 0;
        }
        if(player_characters) {
            clr_screen();
            printf("A expedição foi um sucesso, e os heróis retornam, cansados, aos confortantes campos verdejantes da superfície!\nPressione enter para continuar.\n");
            wait_for_return_key();
        }
        else {
            clr_screen();
            printf("Infelizmente, a expedição foi um fracasso.\nTalvez a próxima tenha mais sorte...\nPressione enter para continuar.\n");
            wait_for_return_key();
        }
        event_end();
        join_threads(ti);
}


#endif