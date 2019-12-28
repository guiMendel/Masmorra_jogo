#include <stdlib.h>
#include <stdio.h>
#include "../masmorra_bib.c"

// Pausam a execucao em caso de resultado inesperado
void e(int v) {
    if(v != 0) {
        printf("Erro na execucao!\n");
        getchar();
    }
}
void eq(int a, int b) {
    if(a != b) {
        printf("Erro na execucao!\n");
        getchar();
    }
}

// realiza a preparacao pre-teste
void setup() {
    ti = 0; event = 0; monsters = 0; ready = 0; new_ready = 0; player_characters = 0;
    e(init_handlers());
}
// realiza a limpa pos-teste (n e o numero de thread criadas)
void finalize(int n) {
    e(event_end());
    e(join_threads(n));
    e(destroy_handlers());
}

// Funcoes utilizadas somente aqui
void *say_hello(void *arg) {
    int id = *(int*) arg;
    free(arg);

    pthread_mutex_lock(&m_io);
        printf("%d - Hello there\n", id);
    pthread_mutex_unlock(&m_io);

    // avisa a main que esta preparada e dorme
    pthread_mutex_lock(&m_ready);
        ready++;
        pthread_cond_wait(&c_event, &m_ready);
    pthread_mutex_unlock(&m_ready);
    
    return NULL;
}

int spawn_testmonster(int id) {
    return 0;
}

int main() {
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));
    testing = 1;
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
        // passivas e acoes do guerreiro e item espada longa
        {
            char txt[500] = "Voce ganha +2 de bonus nas jogadas de ataque realizadas com uma arma de ataque a distancia.";
            char *descr = (char *) malloc(sizeof(char) * (strlen(txt) + 1));
            strcpy(descr, txt);
            add_pass("Arquearia", descr);

            strcpy(txt, "Quando voce rolar um 1 ou um 2 num dado de dano de um ataque com arma corpo-a-corpo que voce esteja empunhando com duas maos, voce pode rolar o dado novamente e usar a nova rolagem, mesmo que resulte em 1 ou 2. A arma deve ter a propriedade duas maos ou versatil para ganhar esse beneficio.");
            // garante o bom funcionamento da funcao malloc
            descr = NULL;
            descr = (char *) malloc(sizeof(char) * (strlen(txt) + 1));
            strcpy(descr, txt);
            add_pass("Combate com Armas Grandes", descr);
            
            strcpy(txt, "Quando voce estiver engajado em uma luta com duas armas, voce pode adicionar o seu modificador de passiva de dano na jogada de dano de seu segundo ataque.");
            // garante o bom funcionamento da funcao malloc
            descr = NULL;
            descr = (char *) malloc(sizeof(char) * (strlen(txt) + 1));
            strcpy(descr, txt);
            add_pass("Combate com Duas Armas", descr);

            strcpy(txt, "Enquanto estiver usando armadura, voce ganha +1 de bonus em sua CA.");
            // garante o bom funcionamento da funcao malloc
            descr = NULL;
            descr = (char *) malloc(sizeof(char) * (strlen(txt) + 1));
            strcpy(descr, txt);
            add_pass("Defesa", descr);

            strcpy(txt, "Quando voce empunhar uma arma de ataque corpo-a-corpo em uma mao e nenhuma outra arma, voce ganha +2 de bonus nas jogadas de dano com essa arma.");
            // garante o bom funcionamento da funcao malloc
            descr = NULL;
            descr = (char *) malloc(sizeof(char) * (strlen(txt) + 1));
            strcpy(descr, txt);
            add_pass("Duelismo", descr);

            strcpy(txt, "No seu turno, caso possua um escudo, voce pode escolher um personagem nao hostil. Qualquer outro personagem que tentar ataca-lo sofre desvantagem.");
            // garante o bom funcionamento da funcao malloc
            descr = NULL;
            descr = (char *) malloc(sizeof(char) * (strlen(txt) + 1));
            strcpy(descr, txt);
            add_pass("Protecao", descr);

            strcpy(txt, "Voce possui uma reserva de estamina e pode usa-la para proteger a si mesmo contra danos. No seu turno, voce pode usar uma acao bonus para recuperar pontos de vida igual a 1d10 + seu nivel.\nUma vez que usar essa caracteristica, voce precisara terminar um descanso curto ou longo para usa-la novamente.");
            // garante o bom funcionamento da funcao malloc
            descr = NULL;
            descr = (char *) malloc(sizeof(char) * (strlen(txt) + 1));
            strcpy(descr, txt);
            add_pass("Retomar o Folego", descr);

            strcpy(txt, "Voce pode forcar o seu limite para alem do normal por um momento. Durante o seu turno, voce pode realizar uma acao adicional juntamente com sua acao normal e possivel acao bonus.\nUma vez que usar essa caracteristica, voce precisara terminar um descanso curto ou longo para usá-la novamente.");
            // garante o bom funcionamento da funcao malloc
            descr = NULL;
            descr = (char *) malloc(sizeof(char) * (strlen(txt) + 1));
            strcpy(descr, txt);
            add_pass("Surto de Acao", descr);

            strcpy(txt, "Seus golpes adquirem uma margem de acerto critico de 19 a 20 nas jogadas de ataque.");
            // garante o bom funcionamento da funcao malloc
            descr = NULL;
            descr = (char *) malloc(sizeof(char) * (strlen(txt) + 1));
            strcpy(descr, txt);
            add_pass("Critico Aprimorado", descr);

            // cria a espada longa
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
        }

    // adiciona o goblin
        factory_insert(spawn_goblin, "Goblin");
        factory_insert(spawn_testmonster, "test");

    // adiciona a acao Ataque Basico, Passar Turno e Fugir
        {
            char desc[200] = "O personagem realiza um ataque, com a arma equipada em sua mao principal, contra uma determinada criatura.";
            act_t a;
            strcpy(a.name, ACT_ATAQUE_BASICO);
            a.descr = (char*) malloc(sizeof(char) * (strlen(desc) + 1));
            strcpy(a.descr, desc);
            a.perform = act_ataque_basico;
            a.performable = check_ataque_basico;
            a.current_effects = effects_ataque_basico;
            add_act(a);

            strcpy(desc, "O personagem passa esta rodada sem realizar nenhuma acao.");
            strcpy(a.name, ACT_PASSAR_TURNO);
            a.descr = (char*) malloc(sizeof(char) * (strlen(desc) + 1));
            strcpy(a.descr, desc);
            a.perform = act_passar_turno;
            a.performable = check_passar_turno;
            a.current_effects = effects_passar_turno;
            add_act(a);

            strcpy(desc, "O personagem tenta escapar do combate.\nQuanto mais monstros ainda houver em combate, menor a chance de sucesso.\n");
            strcpy(a.name, ACT_FUGIR);
            a.descr = (char*) malloc(sizeof(char) * (strlen(desc) + 1));
            strcpy(a.descr, desc);
            a.perform = act_fugir;
            a.performable = check_fugir;
            a.current_effects = effects_fugir;
            add_act(a);
        }


    // ROTINA 1
        printf("ROTINA 1\n");
        // Criar 5 threads e esperar por elas
        setup();
        e(create_threads(5, say_hello));
        finalize(5);
    
    // // ROTINA 2
        // printf("\nROTINA 2\n");
        // // Criar 2 threads, depois criar mais 3 threads, apagar 2, e criar mais 2.
        // setup();
        // e(create_threads(2, say_hello));
        // e(create_threads(3, say_hello));
        // e(event_end());
        // e(join_threads(2));
        // e(create_threads(2, say_hello));
        // finalize(5);

    // ROTINA 3
        printf("\nROTINA 3\n");
        // cria uma thread de jogador e executa o evento event_greet()
        setup();
        e(create_pc(0, 1));
        eq(player_characters, 1);
        e(event_greet());
        finalize(1);

    // ROTINA 4
        printf("\nROTINA 4\n");
        // cria 3 threads de jogador, 3 threads de monstro e executa o evento greet
        setup();
        e(create_pc(0, 3));
        eq(player_characters, 3);
        e(create_mc("test,", 3));
        // e(create_threads(3, monster));
        // while(new_ready != 3) sched_yield();
        //     pthread_mutex_lock(&m_new_ready);
        //         new_ready = 0;
        //     pthread_mutex_unlock(&m_new_ready);
        //     if(pthread_cond_broadcast(&c_new) != 0) return -1;

        eq(monsters, 3);
        e(event_greet());
        finalize(6);

    // ROTINA 5
        printf("\nROTINA 5\n");
        // cria 2 jogadores e 3 mosntros, executa o evento chat, que solicita de cada jogador uma mensagem, mas em que os monstros so gemem, mas em que todas as threads liberam suas mensagens ao mesmo tempo.
        setup();
        e(create_pc(0, 2));
        eq(player_characters, 2);
        e(create_mc("test,", 3));
        // e(create_threads(3, monster));
        // while(new_ready != 3) sched_yield();
        //     pthread_mutex_lock(&m_new_ready);
        //         new_ready = 0;
        //     pthread_mutex_unlock(&m_new_ready);
        //     if(pthread_cond_broadcast(&c_new) != 0) return -1;

        eq(monsters, 3);
        e(event_chat());
        finalize(5);

    // ROTINA 6
        printf("\nROTINA 6\n");
        // lanca 2 d20 (roll_die(20)) e 2 conjuntos de dados (roll_dice)
        setup();
        int t = roll_die(20);
        eq(t>0 && t<=20, 1);
        t = roll_die(20);
        eq(t>0 && t<=20, 1);
        t = roll_dice(3, 6);
        eq(t>0 && t<=18, 1);
        t = roll_dice(100, 30);
        eq(t>0 && t<=3000, 1);
        finalize(0);

    // ROTINA 7
        printf("\nROTINA 7\n");
        // gera 3 jogadores, gera 3 monstros e executa, executa o evento de imprimir seus status e executa o evento de corrida teste 5 vezes
        setup();
        e(create_pc(0, 3));
        eq(player_characters, 3);
        e(create_mc("test,", 3));
        // e(create_threads(3, monster));
        // while(new_ready != 3) sched_yield();
        //     pthread_mutex_lock(&m_new_ready);
        //         new_ready = 0;
        //     pthread_mutex_unlock(&m_new_ready);
        //     if(pthread_cond_broadcast(&c_new) != 0) return -1;

        eq(monsters, 3);
        e(event_status());
        e(event_race());
        // e(event_race());
        // e(event_race());
        // e(event_race());
        // e(event_race());
        finalize(6);

    // ROTINA 8
        printf("\nROTINA 8\n");
        // testa a funcao getv
        {
            char test = getv(SN_PROMPT, 0);
            printf("%c\n", test);
        }


    // ROTINA 9
        printf("\nROTINA 9\n");
        // cria um pj manualmente e um automaticamente, imprime seus atributos
        setup();
        // e(create_pc(1, 1));
        e(create_pc(0, 1));
        e(create_pc(0, 1));
        e(event_status());
        finalize(2);

    // ROTINA 10
        printf("\nROTINA 10\n");
        // cria um goblin e imprime seus status
        setup();
        e(create_mc("Goblin", 1));
        e(event_status());
        finalize(1);

    // // ROTINA 11
        // suspendi esta rotina pois ela foge do padrao de implementacao do restante do codigo
        //     printf("\nROTINA 11\n");
        //     // cria um jogador e um goblin e forca o jogador a atacar o goblin
        //     setup();
        //     create_pc(0, 1);
        //     create_mc("Goblin", 1);
        //     event_status();
        //     // ESSA FUNCAO ESTA EXECUTANDO NA THREAD MAIN, PRECISO PASSAR PARA A THREAD DO MONSTRO. ALEM DO QUE, QUANDO RESETAR O READY PRA LIBERAR O MONSTRO, VAI LIBERAR O JOGADOR TAMBEM. TEM Q VER ISSO.
        //     force_action(0, ACT_ATAQUE_BASICO);
        //     event_status();
        //     finalize(2);
        

    // ROTINA 12
        printf("\nROTINA 12\n");
        // cria um jogador e um goblin e testa o evento de combate
        setup();
        create_pc(0, 1);
        create_mc("Goblin", 1);
        event_status();
        event_combat();
        finalize(2);

    // ROTINA 13
        printf("\nROTINA 13\n");
        // cria 3 jogadores e 3 goblins e testa o combate
        setup();
        create_pc(0, 3);
        create_mc("Goblin", 3);
        event_status();
        event_combat();
        finalize(6);

    // ROTINA 14
        printf("\nROTINA 14\n");
        // cria 15 goblins e 2 jogadores (a ideia e verificar se a morte de jogador funciona)
        setup();
        create_pc(0, 2);
        create_mc("Goblin", 15);
        event_status();
        event_combat();
        finalize(17);

    free_all_resources();

    return 0;
}