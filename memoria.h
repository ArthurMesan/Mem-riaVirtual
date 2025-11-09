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


void substituirPaginaRandom(EntradaPagina* tabela_de_paginas, QuadroFisico* quadros_memoria_fisica,
     unsigned int num_quadros, unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas);

void substituirPaginaLRU(EntradaPagina* tabela_de_paginas, QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas);

void substituirPaginaCLK(EntradaPagina* tabela_de_paginas, QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas);

void pesquisaQuadros(EntradaPagina* tabela_de_paginas, QuadroFisico* quadros_memoria_fisica,
     unsigned int num_quadros, unsigned int page_number, char rw, long total_acessos, long int* paginas_lidas, bool* aux_ver, unsigned int* itera_quadros);

void carregaPagina(EntradaPagina* tabela_de_paginas, QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas, unsigned int* quadro_alvo);

#endif
