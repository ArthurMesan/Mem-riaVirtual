import os

import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

# --- 1. Configuração e Carregamento ---

# Nome do arquivo de dados gerado pelo script bash
ARQUIVO_CSV = "resultados_simulacao.csv"
# Pasta para salvar os gráficos
PASTA_GRAFICOS = "graficos"

# Cria a pasta 'graficos' se ela não existir
os.makedirs(PASTA_GRAFICOS, exist_ok=True)

# Tenta carregar o arquivo CSV
try:
    df = pd.read_csv(ARQUIVO_CSV)
except FileNotFoundError:
    print(f"Erro: Arquivo '{ARQUIVO_CSV}' não encontrado.")
    print("Por favor, execute o script './gerar_dados.sh' primeiro.")
    exit()

# Converte colunas para números (caso haja algum problema de parsing)
df["PaginasLidas"] = pd.to_numeric(df["PaginasLidas"], errors="coerce")
df["PaginasEscritas"] = pd.to_numeric(df["PaginasEscritas"], errors="coerce")

print(f"Dados carregados de '{ARQUIVO_CSV}'. Gerando gráficos...")

# Pega a lista de arquivos de log únicos que foram testados
arquivos_log = df["ArquivoLog"].unique()

# Configura o estilo dos gráficos
sns.set_theme(style="whitegrid", palette="deep")

# --- 2. Geração dos Gráficos (Teste 1: Variando Memória) ---

print("Gerando gráficos do Teste 1 (Variando Memória)...")
for log_file in arquivos_log:
    # Filtra o DataFrame para os dados deste gráfico específico
    df_teste1 = df[(df["TipoTeste"] == "Teste1") & (df["ArquivoLog"] == log_file)]

    # Pega os valores fixos para o título
    tam_pagina = df_teste1["TamPagina"].iloc[0]

    # Cria a figura
    plt.figure(figsize=(12, 7))

    # Cria o gráfico de linhas (um para cada algoritmo)
    grafico1 = sns.lineplot(
        data=df_teste1,
        x="TamMemoria",
        y="PaginasLidas",
        hue="Algoritmo",  # Linhas com cores diferentes
        style="Algoritmo",  # Linhas com estilos diferentes
        markers=True,
        lw=2.5,
    )

    # --- Formatação do Gráfico ---
    grafico1.set_title(
        f"Teste 1: Page Faults vs. Tamanho da Memória\n(Arquivo: {log_file} | Página: {tam_pagina}KB)",
        fontsize=16,
        pad=20,
    )
    grafico1.set_xlabel("Tamanho da Memória (KB)", fontsize=12)
    grafico1.set_ylabel("Total de Page Faults (Páginas Lidas)", fontsize=12)

    # O eixo X (memória) é em potências de 2, então a escala logarítmica fica melhor
    grafico1.set_xscale("log", base=2)
    # Garante que os ticks do eixo X sejam os valores que testamos
    mem_ticks = df_teste1["TamMemoria"].unique()
    grafico1.set_xticks(mem_ticks)
    grafico1.set_xticklabels(mem_ticks)

    plt.legend(title="Algoritmo", loc="upper right")

    # Salva o arquivo
    nome_arquivo = f"{PASTA_GRAFICOS}/teste1_vs_memoria_{log_file}.png"
    plt.savefig(nome_arquivo)
    plt.close()  # Fecha a figura para liberar memória

# --- 3. Geração dos Gráficos (Teste 2: Variando Página) ---

print("Gerando gráficos do Teste 2 (Variando Página)...")
for log_file in arquivos_log:
    # Filtra o DataFrame para os dados deste gráfico
    df_teste2 = df[(df["TipoTeste"] == "Teste2") & (df["ArquivoLog"] == log_file)]

    # Pega os valores fixos para o título
    tam_memoria = df_teste2["TamMemoria"].iloc[0]

    # Cria a figura
    plt.figure(figsize=(12, 7))

    # Cria o gráfico de linhas
    grafico2 = sns.lineplot(
        data=df_teste2,
        x="TamPagina",
        y="PaginasLidas",
        hue="Algoritmo",
        style="Algoritmo",
        markers=True,
        lw=2.5,
    )

    # --- Formatação do Gráfico ---
    grafico2.set_title(
        f"Teste 2: Page Faults vs. Tamanho da Página\n(Arquivo: {log_file} | Memória: {tam_memoria}KB)",
        fontsize=16,
        pad=20,
    )
    grafico2.set_xlabel("Tamanho da Página (KB)", fontsize=12)
    grafico2.set_ylabel("Total de Page Faults (Páginas Lidas)", fontsize=12)

    # O eixo X (página) também é em potências de 2
    grafico2.set_xscale("log", base=2)
    pag_ticks = df_teste2["TamPagina"].unique()
    grafico2.set_xticks(pag_ticks)
    grafico2.set_xticklabels(pag_ticks)

    plt.legend(title="Algoritmo", loc="upper right")

    # Salva o arquivo
    nome_arquivo = f"{PASTA_GRAFICOS}/teste2_vs_pagina_{log_file}.png"
    plt.savefig(nome_arquivo)
    plt.close()  # Fecha a figura

# --- 4. Gráfico Bônus (Páginas Escritas) ---

print("Gerando gráfico bônus (Páginas Escritas)...")
df_bonus = df[
    (df["TipoTeste"] == "Teste1")
    & (df["ArquivoLog"] == "compilador.log")  # Apenas para o compilador.log
]

plt.figure(figsize=(12, 7))
grafico_bonus = sns.lineplot(
    data=df_bonus,
    x="TamMemoria",
    y="PaginasEscritas",  # Usando a outra métrica
    hue="Algoritmo",
    style="Algoritmo",
    markers=True,
    lw=2.5,
)
grafico_bonus.set_title(
    "Gráfico Bônus: Páginas Escritas vs. Tamanho da Memória\n(Arquivo: compilador.log | Página: 4KB)",
    fontsize=16,
    pad=20,
)
grafico_bonus.set_xlabel("Tamanho da Memória (KB)", fontsize=12)
grafico_bonus.set_ylabel("Total de Páginas Escritas (Dirty)", fontsize=12)
grafico_bonus.set_xscale("log", base=2)
mem_ticks = df_bonus["TamMemoria"].unique()
grafico_bonus.set_xticks(mem_ticks)
grafico_bonus.set_xticklabels(mem_ticks)
plt.legend(title="Algoritmo", loc="upper right")
nome_arquivo = f"{PASTA_GRAFICOS}/bonus_paginas_escritas_compilador.png"
plt.savefig(nome_arquivo)
plt.close()

print(f"\n--- Concluído! ---")
print(f"Todos os gráficos foram salvos na pasta: '{PASTA_GRAFICOS}'")
