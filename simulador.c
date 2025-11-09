#include "simulador.h"
#include "memoria.h"

// Função principal
int main(int argc, char *argv[]) {
    /* Caso o numero de argumentos seje maior ou menor que 5 ,incluindo
    o nome do programa, retorne erro*/

    if (argc != 5) {
        // Imprime uma mensagem de erro no stderr
        fprintf(stderr, "Uso incorreto!\n");
        fprintf(stderr, "Formato esperado: %s <algoritmo> <arquivo.log> <tam_pagina_kb> <tam_memoria_kb>\n", argv[0]);
        return EXIT_FAILURE;
    }


    char* algoritmo_nome = argv[1];
    char* arquivo_log_nome = argv[2];
    // atoi (ASCII to Integer) converte a string do argumento para um número
    int tam_pagina_kb = atoi(argv[3]);
    int tam_memoria_kb = atoi(argv[4]);


    FILE *arquivo_log = fopen(arquivo_log_nome, "r");
    if (arquivo_log == NULL) {
        // perror imprime uma mensagem de erro do sistema
        perror("Erro ao abrir o arquivo de log");
        return EXIT_FAILURE;
    }

    //Processamento do Arquivo de Log

    unsigned addr;
    char rw;
    long total_acessos = 0;
    long paginas_lidas = 0;
    long int paginas_escritas = 0;
    bool aux_ver = false;
    int ponteiro_fila = 0;
    unsigned int itera_quadros = 0;

    unsigned int s, tmp;
    unsigned int tam_pagina_bytes = tam_pagina_kb * 1024;
    tmp = tam_pagina_bytes;
    s = 0;

    while(tmp > 1) { //calcula log2(tam_pagina_bytes)
        tmp = tmp >> 1;
        s++;
    }

    printf("Iniciando o simulador...\n");

    // Loop de leitura do arquivo
    // Logica da Simulação -------------------------------------------------------------------

    int num_quadros = tam_memoria_kb / tam_pagina_kb;
    long num_entradas_tabela = 1L << (32 - s);


    //Quadros Físicos
    QuadroFisico* quadros_memoria_fisica = malloc(num_quadros * sizeof(QuadroFisico));
    if (quadros_memoria_fisica == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para os quadros físicos.\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < num_quadros; i++) {
        quadros_memoria_fisica[i].ocupado = false;
        quadros_memoria_fisica[i].sujo = false;
        //quadros_memoria_fisica[i].pagina_virtual = 0;
        quadros_memoria_fisica[i].ultimo_acesso = 0;
        quadros_memoria_fisica[i].usado = false;
    }


    // Tabela de paginas
    EntradaPagina* tabela_de_paginas = malloc(1048576 * sizeof(EntradaPagina));

    // Inicializar todas as entradas como "inválidas"
    for (long i = 0; i < num_entradas_tabela; i++) {
        tabela_de_paginas[i].valido = false;
    }

    while(fscanf(arquivo_log, "%x %c", &addr, &rw) == 2) {
        total_acessos++;

        unsigned int page_number = addr >> s; //Extrai o número da pagina

        //Pesquisa a pagina
        EntradaPagina* entrada = &tabela_de_paginas[page_number];

        if (entrada->valido == true) {

            //Obter o quadro físico (o "Quarto")
            unsigned int quadro = entrada->numero_quadro;

            //TODO: Atualizar os bits de estado para os algoritmos
            quadros_memoria_fisica[quadro].ultimo_acesso = total_acessos;
            quadros_memoria_fisica[quadro].usado = true;

            //Marcar como "sujo" se for uma escrita
            if (rw == 'W') {
                quadros_memoria_fisica[quadro].sujo = true;
            }

        } else {
            //Pesquisa Quadros
            // Resetando variaveis de controle
            aux_ver = false;
            itera_quadros = 0;

            fprintf(stderr, "Pesquisando quadros\n");
            pesquisaQuadros(entrada,quadros_memoria_fisica,num_quadros,page_number,rw,total_acessos,&paginas_lidas, &aux_ver, &itera_quadros);

            if (aux_ver == false && itera_quadros >= num_quadros) {
                fprintf(stderr, "Substituição de página\n");
                    if (strcmp(algoritmo_nome, "random") == 0) {
                        substituirPaginaRandom(entrada,quadros_memoria_fisica, num_quadros, page_number, rw, total_acessos, &paginas_escritas);
                    }
                    else if (strcmp(algoritmo_nome, "lru") == 0) {
                        substituirPaginaLRU(entrada,quadros_memoria_fisica, num_quadros, page_number, rw, total_acessos, &paginas_escritas);
                    }
                    else if (strcmp(algoritmo_nome, "clock") == 0) {
                        substituirPaginaCLK(entrada,quadros_memoria_fisica, num_quadros, page_number, rw, total_acessos, &paginas_escritas);
                    }
                    else if (strcmp(algoritmo_nome, "fifo") == 0) {

                        unsigned int quadro_alvo = ponteiro_fila;

                        // Avança o ponteiro para a próxima vez (circularmente)
                        ponteiro_fila = (ponteiro_fila + 1) % num_quadros;

                        fprintf(stderr, "Algoritmo FIFO escolheu o quadro %d.\n", quadro_alvo);
                                        }
                    else {
                        printf("Comando desconhecido.\n");
                    }
            }

        }
    }


    fclose(arquivo_log);
    free(tabela_de_paginas);
    free(quadros_memoria_fisica);


    printf("\n--- Relatório Final ---\n");
    printf("Arquivo de entrada: %s\n", arquivo_log_nome);
    printf("Tamanho da memória: %d KB\n", tam_memoria_kb);
    printf("Tamanho das páginas: %d KB\n", tam_pagina_kb);
    printf("Técnica de reposição: %s\n", algoritmo_nome);
    printf("Numero paginas lidas: %ld\n", paginas_lidas);
    printf("Paginas escritas: %ld\n", paginas_escritas);
    printf("-------------------------\n");
    printf("Total de acessos à memória: %ld\n", total_acessos); // [cite: 26]


    // Retorna sucesso
    return EXIT_SUCCESS;
}
