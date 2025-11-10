#include "memoria.h"

int encontrarQuadroLivre(QuadroFisico* quadros_memoria_fisica, int num_quadros) {
    for (int i = 0; i < num_quadros; i++) {
        if (quadros_memoria_fisica[i].ocupado == false) {
            return i; // Encontrou um quadro livre
        }
    }
    return -1; // Memória está cheia
}

int encontrarVitima(QuadroFisico* quadros_memoria_fisica, int num_quadros,
                    const char* algoritmo_nome, int* ponteiro_fila, int* ponteiro_clock) {

    int quadro_vitima_idx = 0;

    if (strcmp(algoritmo_nome, "random") == 0) {
        quadro_vitima_idx = rand() % num_quadros;
        fprintf(stderr, "Random escolheu quadro %d. ", quadro_vitima_idx);

    } else if (strcmp(algoritmo_nome, "lru") == 0) {
        long menor_timestamp = quadros_memoria_fisica[0].ultimo_acesso;
        quadro_vitima_idx = 0;
        for (int i = 1; i < num_quadros; i++) {
            if (quadros_memoria_fisica[i].ultimo_acesso < menor_timestamp) {
                menor_timestamp = quadros_memoria_fisica[i].ultimo_acesso;
                quadro_vitima_idx = i;
            }
        }
        fprintf(stderr, "LRU escolheu quadro %d. ", quadro_vitima_idx);

    } else if (strcmp(algoritmo_nome, "clock") == 0) {
        while (true) {
            if (quadros_memoria_fisica[*ponteiro_clock].usado == true) {
                quadros_memoria_fisica[*ponteiro_clock].usado = false; // Segunda chance
                *ponteiro_clock = (*ponteiro_clock + 1) % num_quadros; // Avança
            } else {
                quadro_vitima_idx = *ponteiro_clock;
                *ponteiro_clock = (*ponteiro_clock + 1) % num_quadros; // Avança para a próxima
                break; // Encontrou vítima
            }
        }
        fprintf(stderr, "CLOCK escolheu quadro %d. ", quadro_vitima_idx);

    } else if (strcmp(algoritmo_nome, "fifo") == 0) {
        quadro_vitima_idx = *ponteiro_fila;
        *ponteiro_fila = (*ponteiro_fila + 1) % num_quadros; // Avança o ponteiro
        fprintf(stderr, "FIFO escolheu quadro %d. ", quadro_vitima_idx);

    } else {
        // Padrão para algoritmo desconhecido é 'random'
        quadro_vitima_idx = rand() % num_quadros;
        fprintf(stderr, "Algoritmo '%s' desconhecido, usando Random. Escolhido quadro %d. ", algoritmo_nome, quadro_vitima_idx);
    }

    return quadro_vitima_idx;
}

void processarVitima(int quadro_vitima_idx, QuadroFisico* quadros_memoria_fisica,
                     EntradaPagina* tabela_de_paginas_completa, long* paginas_escritas) {

    QuadroFisico* vitima = &quadros_memoria_fisica[quadro_vitima_idx];

    // Verificar bit sujo e contar
    if (vitima->sujo == true) {
        (*paginas_escritas)++;
        fprintf(stderr, "Página %u (vítima) estava suja. Escrevendo no disco.\n", vitima->numero_pagina);
    } else {
        fprintf(stderr, "Página %u (vítima) estava limpa.\n", vitima->numero_pagina);
    }

    // invalidar a página antiga
    unsigned int pagina_antiga = vitima->numero_pagina;
    tabela_de_paginas_completa[pagina_antiga].valido = false;
}

void carregarPagina(int quadro_alvo_idx, unsigned int page_number, char rw, long total_acessos,
                    QuadroFisico* quadros_memoria_fisica, EntradaPagina* entrada_nova) {

    QuadroFisico* quadro = &quadros_memoria_fisica[quadro_alvo_idx];

    // Carrega dados no quadro físico
    quadro->ocupado = true;
    quadro->numero_pagina = page_number;
    quadro->ultimo_acesso = total_acessos;
    quadro->usado = true;
    quadro->sujo = (rw == 'W'); //pagina já nasce suja se o primeiro acesso for W

    entrada_nova->valido = true;
    entrada_nova->numero_quadro = quadro_alvo_idx; // Apontador para o quadro
}
