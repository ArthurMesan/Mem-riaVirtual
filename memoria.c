#include "memoria.h"

void carregaPagina(EntradaPagina* tabela_de_paginas, QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas,unsigned int* quadro_alvo){

    // --- Processar a Vítima (Centralizado) ---
    QuadroFisico* vitima = &quadros_memoria_fisica[*quadro_alvo];

    if (vitima->sujo == true) {
        (*paginas_escritas)++; // <-- CORREÇÃO BUG 1
        fprintf(stderr, " -> Página %u (vítima) estava suja. Escrevendo no disco.\n", vitima->numero_pagina);
    }
    // Invalidar a página antiga na Tabela de Páginas
    unsigned int pagina_antiga = vitima->numero_pagina;
    tabela_de_paginas[pagina_antiga].valido = false;

    quadros_memoria_fisica[*quadro_alvo].ocupado = true;
    quadros_memoria_fisica[*quadro_alvo].numero_pagina = page_number;
    quadros_memoria_fisica[*quadro_alvo].ultimo_acesso = total_acessos;
    quadros_memoria_fisica[*quadro_alvo].usado = true;
    quadros_memoria_fisica[*quadro_alvo].sujo = (rw == 'W');

    tabela_de_paginas->valido = true;
    tabela_de_paginas->numero_quadro = *quadro_alvo;

}

void substituirPaginaRandom(EntradaPagina* tabela_de_paginas, QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas) {

    unsigned int quadro_alvo = rand() % num_quadros;
    fprintf(stderr, "Substituindo página no quadro %d para a página %u\n", quadro_alvo, page_number);
    QuadroFisico* vitima = &quadros_memoria_fisica[quadro_alvo];

    //Verificação Paginas Escritas
    if (vitima->sujo == true) { paginas_escritas++; }


    unsigned int pagina_antiga = vitima->numero_pagina;
    tabela_de_paginas[pagina_antiga].valido = false;

    carregaPagina(tabela_de_paginas,quadros_memoria_fisica, num_quadros, page_number, rw, total_acessos, paginas_escritas, &quadro_alvo);

}

void substituirPaginaLRU(EntradaPagina* tabela_de_paginas, QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas) {

    long menor_timestamp = quadros_memoria_fisica[0].ultimo_acesso;
    int indice_vitima_lru = 0;

    // Começa do 1, já que o 0 é o nosso 'menor' inicial
    for (int i = 1; i < num_quadros; i++) {
        if (quadros_memoria_fisica[i].ultimo_acesso < menor_timestamp) {
            menor_timestamp = quadros_memoria_fisica[i].ultimo_acesso;
            indice_vitima_lru = i;
        }
    }
    unsigned int quadro_alvo = indice_vitima_lru;
    carregaPagina(tabela_de_paginas,quadros_memoria_fisica, num_quadros, page_number, rw, total_acessos, paginas_escritas, &quadro_alvo);
    fprintf(stderr, "Algoritmo LRU escolheu o quadro %d.\n", quadro_alvo);

}

void substituirPaginaCLK(EntradaPagina* tabela_de_paginas, QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas){

    static int ponteiro_clock = 0;
    unsigned int quadro_alvo;
    while (true) {
    // Verifica o quadro apontado pelo ponteiro
        if (quadros_memoria_fisica[ponteiro_clock].usado == true) {
            // Dá uma segunda chance: marca como não usado e avança
            quadros_memoria_fisica[ponteiro_clock].usado = false;
            ponteiro_clock = (ponteiro_clock + 1) % num_quadros;
        } else {
            // Encontrou a vítima! (usado == false)
            quadro_alvo = ponteiro_clock;
            // Avança o ponteiro para a próxima vez
            ponteiro_clock = (ponteiro_clock + 1) % num_quadros;
            break; // Sai do loop while(true)
        }
    }
    carregaPagina(tabela_de_paginas,quadros_memoria_fisica, num_quadros, page_number, rw, total_acessos, paginas_escritas, &quadro_alvo);
    fprintf(stderr, "Algoritmo CLOCK escolheu o quadro %d.\n", quadro_alvo);
}




void pesquisaQuadros(EntradaPagina* tabela_de_paginas, QuadroFisico* quadros_memoria_fisica,
    unsigned int num_quadros, unsigned int page_number, char rw, long total_acessos, long int* paginas_lidas, bool* aux_ver, unsigned int* itera_quadros){

    (*paginas_lidas)++;
    fprintf(stderr, "FALTA DE PAGINA!\n");
    tabela_de_paginas->valido = true;
    tabela_de_paginas->numero_quadro = *itera_quadros;

    //Procurando um quadro livre
    for ((*itera_quadros) = 0; (*itera_quadros) < num_quadros; (*itera_quadros)++)
    {
        QuadroFisico* quadro_atual = &quadros_memoria_fisica[*itera_quadros];
        *aux_ver = *aux_ver && quadro_atual->ocupado;

        if (quadro_atual->ocupado == false) {
            fprintf(stderr, "Usando quadro livre %d para a página %u\n", *itera_quadros, page_number);

            quadros_memoria_fisica[*itera_quadros].ocupado = true;
            quadros_memoria_fisica[*itera_quadros].numero_pagina = page_number;
            quadros_memoria_fisica[*itera_quadros].ultimo_acesso = total_acessos;
            quadros_memoria_fisica[*itera_quadros].usado = true;
            quadros_memoria_fisica[*itera_quadros].sujo = (rw == 'W');

            tabela_de_paginas->numero_quadro = *itera_quadros;
            break;
        }

    }
}
