#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @brief Representa um Quadro Físico na memória (o "Quarto").
 */
typedef struct {
    bool ocupado;
    bool sujo;                     // Dirty-bit (acesso 'W')
    unsigned int numero_pagina;    // ID da página virtual que está aqui
    bool usado;                    // Bit de referência (para CLOCK)
    long ultimo_acesso;            // Timestamp global (para LRU)
} QuadroFisico;

/**
 * @brief Representa uma entrada.
 */
typedef struct {
    bool valido;
    unsigned int numero_quadro;    // Em qual índice do array de QuadroFisico?
} EntradaPagina;


int encontrarQuadroLivre(QuadroFisico* quadros_memoria_fisica, int num_quadros);

/**
 * @brief Executa o algoritmo de substituição escolhido para encontrar uma vítima.
 * @return O índice (quadro_alvo) da página vítima a ser substituída.
 */
int encontrarVitima(QuadroFisico* quadros_memoria_fisica, int num_quadros,
                    const char* algoritmo_nome, int* ponteiro_fila, int* ponteiro_clock);

/**
 * @brief Processa a vítima: verifica bit sujo, incrementa contador, invalida na tabela.
 */
void processarVitima(int quadro_vitima_idx, QuadroFisico* quadros_memoria_fisica,
                     EntradaPagina* tabela_de_paginas_completa, long* paginas_escritas);

/**
 * @brief Carrega os dados da nova página no quadro físico e atualiza a entrada da tabela.
 */
void carregarPagina(int quadro_alvo_idx, unsigned int page_number, char rw, long total_acessos,
                    QuadroFisico* quadros_memoria_fisica, EntradaPagina* entrada_nova);

#endif // MEMORIA_H
