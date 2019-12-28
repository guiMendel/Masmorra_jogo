#include <stdlib.h>
#include <stdio.h>
#include "masmorra_bib.c"

int main() {
    // modo de teste
    testing = 1;
    
    // "Carregando..."
        clr_screen();
        printf("Carregando...\n");
    // habilita a acentuação
    setlocale(LC_ALL, "Portuguese");
    init_handlers();
    init_lists();
    srand(time(NULL));

    // carrega as passivas do arquivo passives.txt
    load_pass();
    load_act();
    // // IMPLEMENTAR FUNCAO QUE LE DO ARQUIVOS actions.txt E INCLUI AS ACOES
    //     // enquanto nao implementado:
    //     // acoes do guerreiro
    //     {
    //         // IMPLEMENTAR TAL QUE AS PROPRIAS FUNCOES DE ADD_ACT JA ALOQUEM ESPACO PARA A MENSAGEM ENVIADA
    //         char txt[700], *descr;
    //         strcpy(txt, "O personagem realiza um ataque, com a arma equipada em sua mao principal, contra uma determinada criatura.");
    //         act_t a;
    //         strcpy(a.name, ACT_ATAQUE_BASICO);
    //         a.descr = (char*) malloc(sizeof(char) * (strlen(txt) + 1));
    //         strcpy(a.descr, txt);
    //         a.perform = act_ataque_basico;
    //         a.performable = check_ataque_basico;
    //         a.current_effects = effects_ataque_basico;
    //         add_act(a);

    //         strcpy(txt, "O personagem passa esta rodada sem realizar nenhuma acao.");
    //         strcpy(a.name, ACT_PASSAR_TURNO);
    //         a.descr = (char*) malloc(sizeof(char) * (strlen(txt) + 1));
    //         strcpy(a.descr, txt);
    //         a.perform = act_passar_turno;
    //         a.performable = check_passar_turno;
    //         a.current_effects = effects_passar_turno;
    //         add_act(a);

    //         strcpy(txt, "O personagem tenta escapar do combate.\nQuanto mais monstros ainda houver em combate, menor a chance de sucesso.\n");
    //         strcpy(a.name, ACT_FUGIR);
    //         a.descr = (char*) malloc(sizeof(char) * (strlen(txt) + 1));
    //         strcpy(a.descr, txt);
    //         a.perform = act_fugir;
    //         a.performable = check_fugir;
    //         a.current_effects = effects_fugir;
    //         add_act(a);
    //     }

    // // IMPLEMENTAR FUNCAO QUE LE DO ARQUIVOS items.txt E INCLUI OS ITENS
        // enquanto nao implementado:
        // cria a espada longa
        {
            char txt[150];
            strcpy(txt, "Uma espada balanceada, mais leve do que aparenta e bem afiada. A lamina marcada denuncia seus anos de uso.");
            // garante o bom funcionamento da funcao malloc
            item[0].descr = (char *) malloc(sizeof(char) * (strlen(txt) + 1));
            strcpy(item[0].descr, txt);
            strcpy(item[0].name, "Espada Longa");
            item[0].equippable = 1;
            item[0].consumable = 0;
            item[0].effect_value[0] = 1;
            item[0].effect_value[1] = 8;
            strcpy(txt, "Versatil-10\n");
            item[0].properties = (char*) malloc(sizeof(char) * (strlen(txt)+1));
            strcpy(item[0].properties, txt);
            item_i++;
        }

    // IMPLEMENTAR FUNCAO QUE LE DO ARQUIVOS races.txt E INCLUI AS RACAS
        // enquanto nao implementado:
        // adiciona a raca humana
        strcpy(race[race_i++].name, "Humano");
        {
            char *descr = "Os seres mais comuns da regiao. De alturas e cores variadas, seus comportamentos e ambicoes sao tao imprevisiveis quanto seus humores ao acordar.";
            race[0].descr = (char*) malloc(sizeof(char)*(strlen(descr) + 1));
            strcpy(race[0].descr, descr);
        }
        race[0].bonus[0] = 1;
        race[0].bonus[1] = 1;
        race[0].bonus[2] = 1;
        race[0].bonus[3] = 1;
        race[0].bonus[4] = 1;
        race[0].bonus[5] = 1;
        race[0].unissex = 0;
        race[0].id = 0;

    // IMPLEMENTAR FUNCAO QUE LE DO ARQUIVOS classes.txt E INCLUI AS CLASSES
        // enquanto nao implementado:
        // adiciona a classe guerreiro
        strcpy(class[class_i++].name, "Guerreiro");
        {
            char *descr = "Cavaleiros em missoes, lordes conquistadores, campeões reais, infantaria de elite, mercenarios rigidos, bandidos reis ou muitos outros casos, guerreiros compartilham de uma maestria com armas e armaduras sem precedentes, bem como um vasto conhecimento e passivas em combate. E eles estao bem familiarizados com a morte, seja simplesmente conhecendo-a ou desafiando-a cara a cara.";
            class[0].descr = (char*) malloc(sizeof(char)*(strlen(descr) + 1));
            strcpy(class[0].descr, descr);
        }
        class[0].unissex = 0;
        class[0].hp_die = 10;
        class[0].saving_throws[0] = 1;
        class[0].saving_throws[1] = 1;
        class[0].saving_throws[2] = 0;
        class[0].saving_throws[3] = 0;
        class[0].saving_throws[4] = 0;
        class[0].saving_throws[5] = 0;
        class[0].setup = &setup_guerreiro;
        class[0].print_choices = &choices_guerreiro;
        class[0].id = 0;

    // IMPLEMENTAR FUNCAO QUE LE DO ARQUIVOS monsters.txt E INCLUI OS MONSTROS
        // adiciona o goblin
        factory_insert(spawn_goblin, "Goblin");
    // 

    // Tela de inicio
        clr_screen();
        printf("MASMORRA pre-alfa\nPressione enter para comecar\n");
        wait_for_return_key();

    // Menu Principal
        int playing = 1;
        while(playing) {
            clr_screen();
            printf("MASMORRA pre-alfa\nSelecione sua opção\n1 - Jogar como aventureiro\n2 - Jogar como mestre (ainda indisponivel)\n3 - Assistir um jogo (ainda indisponivel)\n0 - Sair\n-> ");
            switch(getint(0, 1)) {
                case 1: play_adventurer(); break;
                case 0: playing = 0;
            }
        }
        printf("Obrigado por jogar Masmorra.\n");
    
    free_all_resources();
    
    destroy_handlers();
    return 0;
}