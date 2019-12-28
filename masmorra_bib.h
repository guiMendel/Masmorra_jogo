#ifndef MASMORRA_H
#define MASMORRA_H

#include <ctype.h>
#include <time.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
// HABILITA ACENTOS!
#include <locale.h>


#ifndef LINKED_LIST_C
#include "misc/linked_list.c"
#endif

#define MAX_MTRS 100
#define MAX_CTRS 100

#define EVENT_COMBAT 2
#define EVENT_STATUS 1
#define EVENT_END 0
#define EVENT_CLEAR_BODIES 3
#define EVENT_PASS -99

#define SN_PROMPT "sn"

//////////////// DESCRICAO DE ENTIDADES

    // define as passivas que cada personagem pode ter. Deve ser populado no incio do programa pelo arquivo masmorra_efeitos.txt, junto com as classes que as garantem
    // as passivas estao diretamente atreladas as acoes, pois elas so sao desencadeadas por uma acao
    typedef struct passive_type {
        // nome da passiva
        char name[50];
        // descricao da passiva
        char *descr;
    } pass_t;
    // define as acoes que cada personagem pode executar. Sempre corresponde a exatamente uma funcao, que realiza de fato a acao. Deve ser populado no incio do programa pelo arquivo masmorra_efeitos.txt, junto com as classes que as garantem
    typedef struct action_type {
        // indice da acao no vetor act. As acoes devem saber os ids das acoes deste vetor para que suas funcoes funcionem
        int id;
        // nome da acao
        char name[50];
        // descricao da acao
        char *descr;

        // executa a acao (id e o id do personagem que invoca a acao e c sera 'p' se for jogador e 'm' se for mosntro)
        int (*perform)(int id, char c);
        // verifica se as condicoes para que esta acao esteja disponivel conferem
        int (*performable)(int id);
        // insere em 'buffer' uma descricao dos efeitos que essa acao atualmente tem.
        // Um ataque, por exemplo, analisa o estado do personagem que a invocaria para descrever qual seria o dano resultante.
        void (*current_effects)(int id, char *buffer);
    } act_t;
    // define as racas jogaveis. Deve ser populado no inicio do programa pelo arquivo masmorra_racas.txt
    typedef struct race_type {
        // identificador da raca (indice no vetor de racas)
        int id;
        // nome da raca
        char name[50];
        // descricao da raca (deve ser alocado)
        char *descr;
        // define se o nome da raca se altera com o sexo do personagem
        int unissex;
        // define os bonus que cada atributo recebe. Os indices correspindem a:
        /*
        0-Constituicao
        1-Forca
        2-Destreza
        3-Inteligencia
        4-Sabedoria
        5-Carisma
        */
        int bonus[6];
    } race_t;
    // define as classes possiveis. Deve ser populado no incio do programa pelo arquivo masmorra_classes.txt
    typedef struct class_type {
        // identificador da classe (indice no vetor de classes)
        int id;
        // nome da classe
        char name[50];
        // descricao da classe
        char *descr;
        // define se o nome da classe se altera com o sexo do personagem
        int unissex;
        // define o dado de vida
        int hp_die;
        // cada atributo q tiver indica bonus de +2 num teste dele
        int saving_throws[6];
        // funcao executada ao atribuir esta classe ao personagem. Solicita ao jogador todas as escolhas de classe que ele deve fazer, como subclasses e equipamentos
        void (*setup)(int id, int self);
        // imprime na tela as escolhas que o personagem correspondente ao id tomou ao receber esta classe
        void (*print_choices)(int id);
    } class_t;
    // define os itens
    typedef struct item_type {
        // define se o item e equipavel e onde o item seria equipavel
        /*
        0 - nao equipavel
        1 - equipavel e uma das maos
        2 - equipavel em duas maos
        3 - cabeca
        4 - torso
        5 - maos
        6 - pernas
        7 - pes
        8 - equipavel como "outros"
        */
        int equippable;
        // define se o item e consumivel (uma vez utilizado, some)
        int consumable;
        // nome do item
        char name[50];
        // descricao do item
        char *descr;

        // caso o item seja equipavel em mao, representa seu dano. Caso seja uma arma, [0] representa quantos dados de dano resulta, e [1] representa qual seu dado de dano
        int effect_value[2];
        // vetor de propriedades do objeto. As propriedades sao interpretadas pelas acoes que dizem respeito a elas
        // os '\n' separam diferentes propriedades
        char *properties;

        // lista as acoes que o item garante ao personagem quando equipado
        act_t *equip_act;
        // lista as passivas que o item garante ao personagem quando equipado
        pass_t *equip_pass;
        // lista as acoes que o item sempre garante ao personagem
        act_t *act;
    } item_t;
    // define a estrutura de um personagem do jogo
    typedef struct character_type {
        // INFORMACOES BASICAS
        
            // define que tipo de personagem e
            /*
            0 - Personagem jogavel criado automaticamente
            1 - Personagem jogavel criado pelo jogador
            */
            int type;
            // nome do personagem
            char name[50];
            // raca do personagem
            race_t *race;
            // corresponde a um indice de class, a classe do personagem
            int class;
            // subclasse do personagem. Funcao apenas estetica, os bonus da subclasse sao atribuidos a partir de sua aparicao no vetor class_choices
            char subclass[50];
            // cada string descreve uma escolha que o jogador fez ao selecionar a classe desse personagem. Os '\n' separam as escolhas
            char *class_choices;
            // sexo do personagem (o - masculino, a - feminino)
            char sex;
        // ESTADO DO PERSONAGEM

            // classe de armadura
            int ac;
            // pontos de vida atuais do personagem
            int hp;
            // pontos de vida maximos do personagem
            int hp_max;
            /* estado fisico do personagem. 1s indicam que o estado esta ativo, 0 que esta inativo. Indices:
            0 - Presença: 0 foragido; 1 em batalha
            1 - Proteção: -1 vulneravel (atacantes recebem vantagem); 0 normal; 1 protegido (atacantes sofrem desvantagem)
            */
            int state[2];
            // bonus de proficiencia
            int prof_bonus;
            // vetor de acoes que este personagem pode executar
            node_t **act;
            // vetor de indices das passivas que o personagem possui
            node_t **pass;

            // vetor de pecas de armadura que o personagem veste. 0 - cabeca, 1 - torso e bracos, 2 - maos e antebracos, 3 - pernas, 4 - pes
            item_t *armor[5];
            // vetor de items que o personagem segura. 0 - mao principal, 1 - mao secundaria
            item_t *hand_held[2];
            // lista encadeada de outros equipamentos que o personagem carrega/equipa
            node_t **other_equips;
        // ATRIBUTOS
        
            // forca
            int a_str;
            // destreza
            int a_dex;
            // inteligencia
            int a_int;
            // sabedoria
            int a_wis;
            // constituicao
            int a_con;
            // carisma
            int a_cha;
    } character_t;
    // define a estrutura de um monstro
    typedef struct monster_type {
        // INFORMACOES BASICAS

            // a especie do monstro
            char species[50];
            // a classe do monstro. Cada especie tem suas classes
            char class[50];
            // a decricao do monstro
            char *descr;
            // o sexo do monstro
            char sex;
            // ca do monstro
            int ac;
        
        // ESTADO DO MONSTRO

            // pontos de vida atuais do monstro
            int hp;
            // pontos de vida totais do monstro
            int hp_max;
            /* estado fisico do personagem. 1s indicam que o estado esta ativo, 0 que esta inativo. Indices:
            0 - Presença: 0 foragido; 1 em batalha
            1 - Proteção: -1 vulneravel (atacantes recebem vantagem); 0 normal; 1 protegido (atacantes sofrem desvantagem)
            */
            int state[2];
            // vetor de acoes que este monstro pode executar
            node_t **act;
            // vetor de indices das passivas que o monstro possui
            node_t **pass;

            // vetor de pecas de armadura que o monstro veste. 0 - cabeca, 1 - torso e bracos, 2 - maos e antebracos, 3 - pernas, 4 - pes
            item_t *armor[5];
            // vetor de items que o monstro segura. 0 - mao principal, 1 - mao secundaria
            item_t *hand_held[2];
            // lista encadeada de outros equipamentos que o monstro carrega/equipa
            node_t **other_equips;
            // inventario do monstro
            node_t **inventory;

            
        // ATRIBUTOS

            // forca
            int a_str;
            // destreza
            int a_dex;
            // inteligencia
            int a_int;
            // sabedoria
            int a_wis;
            // constituicao
            int a_con;
            // carisma
            int a_cha;

    } monster_t;
    // associa uma especie de monstro a uma funcao que e capaz de gerar as configuracoes deste monstro sobre a struct monster_t
    typedef struct mtr_gen_type {
        // a especie associada
        char species[50];
        // a funcao que gera o monstro
        int (*spawn)(int id);
    } mtr_gen_t;

#ifndef PLAYER_BIB_H
#include "player_character/player_bib.h"
#endif

#ifndef MONSTER_BIB_H
#include "monster_character/monster_bib.h"
#endif

#ifndef CLASSES_BIB_H
#include "player_character/classes_bib.h"
#endif

#ifndef ADVENTURER_H
#include "game_modes/adventurer.h"
#endif

////////////////////////////// VARIAVEIS GLOBAIS //////////////////////////////
    //////////////// HANDLERS
        
        // mutex para usar funcoes de entrada e saida de tela
        pthread_mutex_t m_io;
        // mutex para atualizar o contador ready
        pthread_mutex_t m_ready;
        // mutex para atualizar as threads recem-nascidas
        pthread_mutex_t m_new_ready;
        // mutex para os dados de cada monstro
        pthread_mutex_t m_mtr[MAX_MTRS];
        // mutex para os dados de cada personagem
        pthread_mutex_t m_ctr[MAX_CTRS];
        // variavel de condicao de fim de evento
        pthread_cond_t c_event;
        // condicional de sincronizacao thread recem nascida
        pthread_cond_t c_new;
        // vetor de threads do programa
        pthread_t ts[100];
        // habilita mutexes recursivas
        pthread_mutexattr_t attr;

        // inicializa todos os mutexes e var de condicao
        // ret: 0 se obtiver sucesso, -1 do contrario
        int init_handlers();

        // destroi todos os mutexes e var de condicao
        // ret: 0 se obtiver sucesso, -1 do contrario
        int destroy_handlers();
    
    //////////////// VARIAVEIS GLOBAIS
    
        // vetor de personagens
        character_t *ctr[MAX_CTRS];
        // vetor de monstros ativos
        monster_t *mtr[MAX_MTRS];
        // armazena quantos modelos de monstros ja foram registrados
        int mtr_model_i;
        // vetor de geradores de monstros
        mtr_gen_t *mtr_factory;
        // conta quantos geradores de monstro foram registrados
        int mtr_gen_i;
        // armazena o indice do proximo espaco ainda nao ocupado por uma thread ativa
        // coincide com a quantidade de threads criadas
        int ti;
        // vetor de racas
        race_t race[20];
        // total de racas ativas (deve ser inicializado com 0 no inicio do programa)
        int race_i;
        // vetor de classes
        class_t class[20];
        // total de classes ativas (deve ser inicializado com 0 no inicio do programa)
        int class_i;
        // lista de passivas
        node_t **pass_l;
        // total de passivas ativas (deve ser inicializado com 0 no inicio do programa)
        int pass_i;
        // função que acessa a passiva de índice providenciado
        pass_t *pass(int id);
        // lista de acoes
        node_t **act_l;
        // total de acoes ativas (deve ser inicializado com 0 no inicio do programa)
        int act_i;
        // função que acessa a ação de índice providenciado
        act_t *act(int id);
        // vetor de modelos de itens
        item_t item[50];
        // total de itens adicionados ao jogo (deve ser inicializado com 0 no inicio do programa)
        int item_i;
        // armazena o atual evento em rigor
        /*
        Legenda dos valores de evento:
        -3: Race - evento em que todas as threads dormem por um tempo e em seguida imprimem algo na tela. O tempo que dormem é afetado negativamente pelas suas destrezas
        -2: Chat - evento em que cada thread imprime uma mensagem, ao mesmo tempo. As threads de jogador pedem a mensagem a ser impressa ao jogador
        -1: Greet - evento de teste em que cada thread ativa imprime algo na tela, se identificando
        0: Exit - a thread finaliza sua execucao
        1: Status - todas as threads de jogadores imprimem na tela seus status
        2: Combat - enquanto houver jogadores e monstros vivos, eles estarao revezando turnos num combate
        */
        int event;
        // contadores de monstros e jogadores
        int monsters, player_characters;
        // indica se algum personagem morreu na execucao da rodada atual de combate
        int flag_fallen_characters;
        // contador de quantas threads ja estao prontas
        int ready;
        // contador das threads recem nascidas
        int new_ready;
        // define se o programa esta executando em modo de teste ou nao
        int testing;
        // string que armazena os erros que acontecerem no programa
        char *errors_msg;
        // string que armazena mensagens de debug em modo 'testing'
        char *debug_msg;

////////////////////////////// FUNCOES DE THREADS //////////////////////////////

    // funcao responsavel por executar todas as interacoes dos personagens do jogador com o mundo
    void *player_character(void *empty);
    // funcao responsavel por executar as interacoes dos monstros
    void *monster(void *empty);

////////////////////////////// FUNCOES DA MAIN //////////////////////////////
    //////////////// EVENTOS

        // as threads ativas imprimem algo na tela, se identificando
        int event_greet();
        // as threads jogadores pedem uma mensagem para imprimir, e os monstros imprimem um rosnado, mas todos ao mesmo tempo
        int event_chat();
        // cada thread de jogador imprime na tela seus status
        int event_status();
        // na largada, todas as threads dormem por um tempo e em seguida imprimem algo na tela. O tempo que dormem é afetado negativamente pelas suas destrezas
        int event_race();
        // finaliza a execucao da thread
        int event_end();
        // enquanto ainda houver monstros (e jogadores) vivos, executa um combate em turnos dos jogadores contra os monstros
        int event_combat();

    // inicia as listas globais
    void init_lists();
    // lê todas as passivas definidas no arquivo passives.txt e as adiciona à struct pass
    void load_pass();
    // lê todas as ações definidas no arquivo actions.txt e as adiciona à struct pass
    void load_act();
    // lê todas as raças definidas no arquivo races.txt e as adiciona à struct pass
    void load_race();
    // lê todas as classes definidas no arquivo classes.txt e as adiciona à struct pass
    void load_class();
    // lê todas os itens definidos no arquivo items.txt e as adiciona à struct pass
    void load_item();

    // função que popula a fábrica de mosntros. Deve ser extendida para abarcar todos os monstros novos que forem implementados
    void load_factory();

    // faz as checagens e decisoes de um inicio de nova rodada de combate
    void round_start(int round);
    // faz as checagens de fim de rodada de combate e determina se havera uma nova rodada
    int round_end(int round);
    // libera todos os recursos alocados pelo programa
    int free_all_resources();

    // cria n personagems de jogador. Se manual for verdadeiro, o personagem e consturido pelo jogador
    int create_pc(int manual, int n);
    // cria n monstros de especie especificada em 'name' ate a virgula e classe especificada apos
    int create_mc(char *name, int n);

    // adiciona uma funcao e sua espeice ao vetor de geradores de monstros
    int factory_insert(int (*gen)(int), char *species);

    // encontra o indice da passiva pelo seu nome
    int get_pass_n(char *name);

    // encontra o indice de uma acao pelo seu nome
    int get_act_n(char *name);

    // cria n threads, com a funcao f
    // a funcao passa para a funcao f um espaco de inteiro, alocado dinamicamente, com o indice desta thread no vetor ts
    // ret: 0 se obtiver sucesso, -1 do contrario
    int create_threads(int n, void *f);

    // espera pelas ultimas n threads do vetor de threads
    // ret: 0 se obtiver sucesso, -1 do contrario
    int join_threads(int n);

    // rola 'total' dados de 'sides' lados e pega os 'effective' maiores valores
    int roll_dice_advantage(int total, int sides, int effective);
    // gera um numero aleatorio de 1 ate o numero especificado (sides)
    int roll_die(int sides);
    // gera o resultado da soma de n execucoes de roll_dice(sides)
    int roll_dice(int n, int sides);
    
    // retorna o valor do modificador do atributo especificado
    int mod(int a) {return a/2-5;}

    // pede uma entrada de um caracter ao jogador quantas vezes forem necessarias para obter um valor correto (um valor que esteja dentro do array de strings val). Se capital for 1, nao ignora maiusculas
    char getv(char *val, int capital);
    // pede uma entrada de inteiro maior ou igual a que min e menor ou igual a max ate o usuario dar isso
    int getint(int min, int max);

    // armazena um erro na msg que mostra os erros
    void raise_error(char *msg);

    // limpa a tela e persiste as msgs de erro no topo
    void clr_screen();

////////////////////////////// FUNCOES AUXILIARES //////////////////////////////

    // armazena em 'buffer' uma string descrevendo o estado aparente do personagem, com base da fracao de vida que ainda lhe resta. "Aparentemente bem" 2/3 acima, "Ferido" 1/3 ate 2/3, "Gravemente ferido" 0/3 ate 1/3
    void apparent_health(int hp, int total, char *buffer);
    // utilizado pelas threads no meio de suas acoes. Se for um jogador, libera o mutex de io. Incrementa o contador ready e espera a largada para continuar o processamento
    void barrier(int id,char c);
    // espera todos os personagens ativos levantarem suas bandeiras de ready (ocorre uma linha antes de entrarem em seus postos de espera por broadcast)
    void wait_characters() {while(ready != player_characters + monsters) sched_yield();}

////////////////////////////// FUNCOES EASE OF LIFE //////////////////////////////

    void wait_for_return_key() {scanf("%*[^\n]"); getchar();}
#endif