#!/bin/bash

# --- Configuração ---
EXECUTAVEL="./simulador"
ARQUIVO_CSV="resultados_simulacao.csv"
ARQUIVOS_LOG=("compilador.log" "matriz.log" "compressor.log" "simulador.log")
ALGORITMOS=("lru" "random" "clock" "fifo")

# --- Teste 1: Memória Variável (Página Fixa em 4KB) ---
PAGINA_FIXA=4
MEMORIAS=(128 256 512 1024 2048) # Em KB

# --- Teste 2: Página Variável (Memória Fixa em 1MB) ---
MEMORIA_FIXA=1024 # 1MB
PAGINAS=(2 4 8 16 32 64) # Em KB

# --- Início do Script ---
echo "Gerando dados para análise... Isso pode demorar alguns minutos."

# 1. Limpa o arquivo antigo e escreve o cabeçalho do CSV
echo "TipoTeste,Algoritmo,ArquivoLog,TamPagina,TamMemoria,PaginasLidas,PaginasEscritas" > $ARQUIVO_CSV

# --- Execução do Teste 1 ---
echo "Executando Teste 1 (Variando Memória)..."
for alg in "${ALGORITMOS[@]}"; do
    for log in "${ARQUIVOS_LOG[@]}"; do
        for mem in "${MEMORIAS[@]}"; do
            echo "  -> Rodando: $alg, $log, ${PAGINA_FIXA}KB Pag, ${mem}KB Mem"

            # Executa o simulador e captura a saída
            # CUIDADO: Este grep/awk deve bater EXATAMENTE com seu printf
            output=$( $EXECUTAVEL $alg $log $PAGINA_FIXA $mem )

            # Parseia a saída para extrair os números
            lidas=$(echo "$output" | grep "Numero paginas lidas:" | awk '{print $4}')
            escritas=$(echo "$output" | grep "Paginas escritas:" | awk '{print $3}')

            # Escreve a linha no CSV
            echo "Teste1,$alg,$log,$PAGINA_FIXA,$mem,$lidas,$escritas" >> $ARQUIVO_CSV
        done
    done
done

# --- Execução do Teste 2 ---
echo "Executando Teste 2 (Variando Página)..."
for alg in "${ALGORITMOS[@]}"; do
    for log in "${ARQUIVOS_LOG[@]}"; do
        for pag in "${PAGINAS[@]}"; do
            echo "  -> Rodando: $alg, $log, ${pag}KB Pag, ${MEMORIA_FIXA}KB Mem"

            # Executa o simulador e captura a saída
            output=$( $EXECUTAVEL $alg $log $pag $MEMORIA_FIXA )

            # Parseia a saída
            lidas=$(echo "$output" | grep "Numero paginas lidas:" | awk '{print $4}')
            escritas=$(echo "$output" | grep "Paginas escritas:" | awk '{print $3}')

            # Escreve a linha no CSV
            echo "Teste2,$alg,$log,$pag,$MEMORIA_FIXA,$lidas,$escritas" >> $ARQUIVO_CSV
        done
    done
done

echo "--- Concluído! ---"
echo "Dados salvos em: $ARQUIVO_CSV"
