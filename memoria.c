#include "memoria.h"

// --- CORREÇÃO: Função 'carregaPagina' simplificada ---
// Ela agora só carrega a página. Não processa mais a vítima.
void carregaPagina(EntradaPagina* entrada_nova, QuadroFisico* quadro_alvo,
                   unsigned int page_number, char rw, long total_acessos) {

    quadro_alvo->ocupado = true;
    quadro_alvo->numero_pagina = page_number;
    quadro_alvo->ultimo_acesso = total_acessos;
    quadro_alvo->usado = true;
    quadro_alvo->sujo = (rw == 'W');

    // Atualiza a *entrada específica* da nova página
    entrada_nova->valido = true;
    // Precisamos do índice do quadro. É mais fácil passar o quadro_alvo (índice)
    // Mas para manter sua estrutura, vamos assumir que o ponteiro é o bastante.
    // A forma correta seria passar o 'quadro_alvo' como 'unsigned int'
    // Mas vamos manter sua lógica por enquanto.
}


// --- CORREÇÃO: Assinatura e Lógica ---
void substituirPaginaRandom(EntradaPagina* tabela_de_paginas_completa, EntradaPagina* entrada_nova,
                 QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas) {

    unsigned int quadro_alvo = rand() % num_quadros;
    fprintf(stderr, "Substituindo página no quadro %d para a página %u\n", quadro_alvo, page_number);

    // 1. Processar Vítima
    QuadroFisico* vitima = &quadros_memoria_fisica[quadro_alvo];
    if (vitima->sujo == true) {
        (*paginas_escritas)++; // CORREÇÃO: Usar (*)
    }

    // CORREÇÃO (BUG FATAL): Usar o array completo para invalidar
    unsigned int pagina_antiga = vitima->numero_pagina;
    tabela_de_paginas_completa[pagina_antiga].valido = false;

    // 2. Carregar nova página
    carregaPagina(entrada_nova, vitima, page_number, rw, total_acessos);
}

// --- CORREÇÃO: Assinatura e Lógica ---
void substituirPaginaLRU(EntradaPagina* tabela_de_paginas_completa, EntradaPagina* entrada_nova,
                 QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas) {

    // ... (Seu código para achar 'indice_vitima_lru' está CORRETO) ...
    long menor_timestamp = quadros_memoria_fisica[0].ultimo_acesso;
    int indice_vitima_lru = 0;
    for (int i = 1; i < num_quadros; i++) {
        if (quadros_memoria_fisica[i].ultimo_acesso < menor_timestamp) {
            menor_timestamp = quadros_memoria_fisica[i].ultimo_acesso;
            indice_vitima_lru = i;
        }
    }
    unsigned int quadro_alvo = indice_vitima_lru;

    // --- CORREÇÃO: Adicionar lógica de processar vítima ---
    QuadroFisico* vitima = &quadros_memoria_fisica[quadro_alvo];
    if (vitima->sujo == true) {
        (*paginas_escritas)++; // CORREÇÃO: Usar (*)
    }

    // CORREÇÃO (BUG FATAL): Usar o array completo para invalidar
    unsigned int pagina_antiga = vitima->numero_pagina;
    tabela_de_paginas_completa[pagina_antiga].valido = false;

    // 2. Carregar nova página
    carregaPagina(entrada_nova, vitima, page_number, rw, total_acessos);
    fprintf(stderr, "Algoritmo LRU escolheu o quadro %d.\n", quadro_alvo);
}

// --- CORREÇÃO: Assinatura e Lógica ---
void substituirPaginaCLK(EntradaPagina* tabela_de_paginas_completa, EntradaPagina* entrada_nova,
                 QuadroFisico* quadros_memoria_fisica, unsigned int num_quadros,
                 unsigned int page_number, char rw, long total_acessos, long int* paginas_escritas){

    // ... (Seu código para achar 'quadro_alvo' com 'static ponteiro_clock' está CORRETO) ...
    static int ponteiro_clock = 0;
    unsigned int quadro_alvo;
    while (true) {
        if (quadros_memoria_fisica[ponteiro_clock].usado == true) {
            quadros_memoria_fisica[ponteiro_clock].usado = false;
            ponteiro_clock = (ponteiro_clock + 1) % num_quadros;
        } else {
            quadro_alvo = ponteiro_clock;
            ponteiro_clock = (ponteiro_clock + 1) % num_quadros;
            break;
        }
    }

    // --- CORREÇÃO: Adicionar lógica de processar vítima ---
    QuadroFisico* vitima = &quadros_memoria_fisica[quadro_alvo];
    if (vitima->sujo == true) {
        (*paginas_escritas)++; // CORREÇÃO: Usar (*)
    }

    // CORREÇÃO (BUG FATAL): Usar o array completo para invalidar
    unsigned int pagina_antiga = vitima->numero_pagina;
    tabela_de_paginas_completa[pagina_antiga].valido = false;

    // 2. Carregar nova página
    carregaPagina(entrada_nova, vitima, page_number, rw, total_acessos);
    fprintf(stderr, "Algoritmo CLOCK escolheu o quadro %d.\n", quadro_alvo);
}

// --- CORREÇÃO: Lógica de 'pesquisaQuadros' ---
void pesquisaQuadros(EntradaPagina* entrada_nova, QuadroFisico* quadros_memoria_fisica,
    unsigned int num_quadros, unsigned int page_number, char rw, long total_acessos,
    long int* paginas_lidas, bool* aux_ver, unsigned int* itera_quadros){

    (*paginas_lidas)++;
    fprintf(stderr, "FALTA DE PAGINA!\n");

    // CORREÇÃO: 'itera_quadros' é o loop var, 'aux_ver' é o flag de sucesso
    // '*aux_ver = false;' é feito no 'main'

    //Procurando um quadro livre
    for ((*itera_quadros) = 0; (*itera_quadros) < num_quadros; (*itera_quadros)++)
    {
        QuadroFisico* quadro_atual = &quadros_memoria_fisica[*itera_quadros];

        if (quadro_atual->ocupado == false) {
            fprintf(stderr, "Usando quadro livre %d para a página %u\n", *itera_quadros, page_number);

            // Carrega a página no quadro livre
            quadro_atual->ocupado = true;
            quadro_atual->numero_pagina = page_number;
            quadro_atual->ultimo_acesso = total_acessos;
            quadro_atual->usado = true;
            quadro_atual->sujo = (rw == 'W');

            // Atualiza a entrada da tabela
            entrada_nova->valido = true;
            entrada_nova->numero_quadro = *itera_quadros;

            *aux_ver = true; // <-- CORREÇÃO: Sinaliza que encontrou
            break; // Para o loop
        }
    }
    // Se o loop terminar sem 'break', 'aux_ver' continua 'false'
    // e 'itera_quadros' será 'num_quadros', ativando a lógica em 'main'.
}
