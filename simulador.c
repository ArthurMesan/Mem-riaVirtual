#include "simulador.h"

int main(int argc, char *argv[]) {

    if (argc != 5) {
        fprintf(stderr, "Formato esperado: %s <algoritmo> <arquivo.log> <tam_pagina_kb> <tam_memoria_kb>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* algoritmo_nome = argv[1];
    char* arquivo_log_nome = argv[2];
    int tam_pagina_kb = atoi(argv[3]);
    int tam_memoria_kb = atoi(argv[4]);

    FILE *arquivo_log = fopen(arquivo_log_nome, "r");
    if (arquivo_log == NULL) {
        perror("Erro ao abrir o arquivo de log");
        return EXIT_FAILURE;
    }

    // Inicializa o gerador de números aleatórios, para ranfdom
    srand(time(NULL));

    // --- 2. CÁLCULOS INICIAIS ---
    unsigned int s, tmp;
    unsigned int tam_pagina_bytes = tam_pagina_kb * 1024;
    tmp = tam_pagina_bytes;
    s = 0;
    while(tmp > 1) { //calcula log2(tam_pagina_bytes)
        tmp = tmp >> 1;
        s++;
    }

    int num_quadros = tam_memoria_kb / tam_pagina_kb;
    long num_entradas_tabela = 1L << (32 - s);

    // --- 3. ALOCAÇÃO E INICIALIZAÇÃO ---

    // Contadores
    long total_acessos = 0;
    long paginas_lidas = 0;
    long paginas_escritas = 0;

    // Ponteiros para algoritmos
    int ponteiro_fila = 0;
    int ponteiro_clock = 0;

    // Alocar Quadros Físicos
    QuadroFisico* quadros_memoria_fisica = malloc(num_quadros * sizeof(QuadroFisico));
    if (quadros_memoria_fisica == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para os quadros físicos.\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < num_quadros; i++) {
        quadros_memoria_fisica[i].ocupado = false;
        quadros_memoria_fisica[i].sujo = false;
        quadros_memoria_fisica[i].numero_pagina = 0;
        quadros_memoria_fisica[i].ultimo_acesso = 0;
        quadros_memoria_fisica[i].usado = false;
    }

    // Alocar Tabela de Páginas (com o tamanho correto)
    EntradaPagina* tabela_de_paginas = malloc(num_entradas_tabela * sizeof(EntradaPagina));
    if (tabela_de_paginas == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória para a tabela de páginas.\n");
        free(quadros_memoria_fisica);
        return EXIT_FAILURE;
    }
    for (long i = 0; i < num_entradas_tabela; i++) {
        tabela_de_paginas[i].valido = false;
    }

    printf("Iniciando o simulador...\n");
    printf("Config: %s, %s, %dKB Pag, %dKB Mem. Quadros: %d. Entradas Tabela: %ld. Bits 's': %u\n",
           algoritmo_nome, arquivo_log_nome, tam_pagina_kb, tam_memoria_kb, num_quadros, num_entradas_tabela, s);

    // Simulação
    unsigned addr;
    char rw;
    while(fscanf(arquivo_log, "%x %c", &addr, &rw) == 2) {
        total_acessos++;

        unsigned int page_number = addr >> s;
        EntradaPagina* entrada = &tabela_de_paginas[page_number];

        if (entrada->valido == true) {
            // Page Hit
            unsigned int quadro_idx = entrada->numero_quadro;
            quadros_memoria_fisica[quadro_idx].ultimo_acesso = total_acessos;
            quadros_memoria_fisica[quadro_idx].usado = true;
            if (rw == 'W') {
                quadros_memoria_fisica[quadro_idx].sujo = true;
            }

        } else {
            //fage fault
            paginas_lidas++;
            fprintf(stderr, "Page Fault na página %u (Acesso %ld)\n", page_number, total_acessos);

            // procurar quadro livre
            int quadro_alvo_idx = encontrarQuadroLivre(quadros_memoria_fisica, num_quadros);

            // se memória estiver cheia, encontrar e processar vitima
            if (quadro_alvo_idx == -1) {
                fprintf(stderr, "  Memória cheia. Substituindo... ");

                //Encontrar vítima
                quadro_alvo_idx = encontrarVitima(quadros_memoria_fisica, num_quadros, algoritmo_nome,
                                                  &ponteiro_fila, &ponteiro_clock);

                //Processar vítima, contar 'suja' e invalidar
                processarVitima(quadro_alvo_idx, quadros_memoria_fisica, tabela_de_paginas, &paginas_escritas);
            } else {
                fprintf(stderr, "  Usando quadro livre %d.\n", quadro_alvo_idx);
            }

            // Carregar a nova página no quadro_alvo (seja ele livre ou da vítima)
            carregarPagina(quadro_alvo_idx, page_number, rw, total_acessos, quadros_memoria_fisica, entrada);
        }
    }

    //Relatorio
    fclose(arquivo_log);

    printf("\n--- Relatório Final ---\n");
    printf("Arquivo de entrada: %s\n", arquivo_log_nome);
    printf("Tamanho da memória: %d KB\n", tam_memoria_kb);
    printf("Tamanho das páginas: %d KB\n", tam_pagina_kb);
    printf("Técnica de reposição: %s\n", algoritmo_nome);
    printf("Total de acessos à memória: %ld\n", total_acessos);
    printf("Páginas lidas (Page Faults): %ld\n", paginas_lidas);
    printf("Páginas escritas (Dirty): %ld\n", paginas_escritas);
    printf("-------------------------\n");

    free(tabela_de_paginas);
    free(quadros_memoria_fisica);

    return EXIT_SUCCESS;
}
