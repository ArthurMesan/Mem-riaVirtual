#ifndef MEMORIA_H
#define MEMORIA_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
    bool ocupado;
    unsigned int numero_pagina;
    bool usado;
    long ultimo_acesso;
    bool sujo;
}QuadroFisico;


typedef struct
{
    bool valido;
    unsigned int numero_quadro;
}EntradaPagina;


void carregaPagina(EntradaPagina* entrada_nova, QuadroFisico* quadro_alvo,
                   unsigned int page_number, char rw, long total_acessos);

void substituirPaginaRandom(EntradaPagina* tabela_de_paginas_completa, EntradaPagina* entrada_nova,
                 QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas);

void substituirPaginaLRU(EntradaPagina* tabela_de_paginas_completa, EntradaPagina* entrada_nova,
                 QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas);

void substituirPaginaCLK(EntradaPagina* tabela_de_paginas_completa, EntradaPagina* entrada_nova,
                 QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas);

// Assinatura de pesquisaQuadros estava correta (já recebia 'entrada' que era o 'tabela_de_paginas')
// Mas vamos renomear para clareza:
void pesquisaQuadros(EntradaPagina* entrada_nova, QuadroFisico* quadros_memoria_fisica,
    unsigned int num_quadros, unsigned int page_number, char rw, long total_acessos,
    long int* paginas_lidas, bool* aux_ver, unsigned int* itera_quadros);

#endif
