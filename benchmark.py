import csv
import glob
import os
import re
import subprocess
import sys

try:
    import matplotlib.pyplot as plt
    TEM_MATPLOTLIB = True
except ImportError:
    TEM_MATPLOTLIB = False

if os.path.exists("./simulador"):
    EXECUTAVEL = "./simulador"
else:
    sys.exit("Erro: executável <simulador> não encontrado")

PASTA_SAIDA = "resultados"
os.makedirs(PASTA_SAIDA, exist_ok=True)


def rodar_simulacao(algoritmo, arquivo_log, tam_pagina, tam_memoria, tabela):
    cmd = [EXECUTAVEL, algoritmo, arquivo_log, str(tam_pagina), str(tam_memoria), tabela]
    res = subprocess.run(cmd, capture_output=True, text=True)

    if res.returncode != 0:
        raise RuntimeError(f"Falha ao executar {' '.join(cmd)}:\n{res.stdout}\n{res.stderr}")

    saida = res.stdout
    try:
        acessos = int(re.search(r"Total de acessos à memória:\s*(\d+)", saida).group(1))
        faults = int(re.search(r"Páginas lidas:\s*(\d+)", saida).group(1))
        escritas = int(re.search(r"Páginas escritas:\s*(\d+)", saida).group(1))
        memoria_bytes = int(re.search(r"Mem[oó]ria da tabela:\s*(\d+)", saida).group(1))
        tempo_consulta = float(re.search(r"Tempo de consulta:\s*([\d.]+)", saida).group(1))
        tempo_total = float(re.search(r"Tempo total:\s*([\d.]+)", saida).group(1))
    except AttributeError:
        raise ValueError(f"Não foi possível extrair as métricas da saída:\n{saida}")

    return {
        "algoritmo": algoritmo,
        "arquivo": os.path.basename(arquivo_log),
        "tam_pagina_kb": tam_pagina,
        "tam_memoria_kb": tam_memoria,
        "qtd_quadros": tam_memoria // tam_pagina,
        "tabela": tabela,
        "acessos": acessos,
        "escritas": escritas,
        "faults": faults,
        "taxa_faults_pct": round((faults / acessos) * 100, 4),
        "memoria_bytes": memoria_bytes,
        "memoria_kb": round(memoria_bytes / 1024.0, 4),
        "tempo_consulta_ms": tempo_consulta,
        "tempo_total_ms": tempo_total,
    }


def salvar_csv(caminho, registros):
    if not registros:
        return
    with open(caminho, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=registros[0].keys())
        writer.writeheader()
        writer.writerows(registros)
    print(f"Salvo: {caminho}")


def gerar_graficos(dados_tabelas, dados_algoritmos):
    if not TEM_MATPLOTLIB:
        print("\nAviso: 'matplotlib' não instalado. Rode 'pip install matplotlib' para gerar os gráficos.")
        return

    nomes_tabelas = {
        "densa": "Densa",
        "th2": "Hierárquica (2 níveis)",
        "th3": "Hierárquica (3 níveis)",
        "invertida": "Invertida",
    }

    # Gráfico: Memória ocupada vs. Tamanho da Página (Memória fixa em 1024 KB)
    plt.figure(figsize=(9, 5))
    for tab in ["densa", "th2", "th3", "invertida"]:
        subset = sorted(
            [r for r in dados_tabelas if r["tabela"] == tab and r["tam_memoria_kb"] == 1024],
            key=lambda x: x["tam_pagina_kb"],
        )
        plt.plot(
            [r["tam_pagina_kb"] for r in subset],
            [r["memoria_kb"] for r in subset],
            marker="o",
            label=nomes_tabelas[tab],
        )

    plt.yscale("log")  # Escala logarítmica para visualizar de ~0.5 KB até 4096 KB
    plt.xlabel("Tamanho da Página (KB)")
    plt.ylabel("Memória da Tabela (KB - escala log)")
    plt.title("Custo de Memória por Estrutura de Tabela (RAM = 1024 KB)")
    plt.xticks([2, 4, 8, 16, 32, 64])
    plt.grid(True, which="both", linestyle="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{PASTA_SAIDA}/grafico_memoria_tabelas.png", dpi=150)
    plt.close()

    # Gráfico: Tempo de Consulta vs. Quantidade de Quadros (Página fixa em 2 KB)
    plt.figure(figsize=(9, 5))
    for tab in ["densa", "th2", "th3", "invertida"]:
        subset = sorted(
            [r for r in dados_tabelas if r["tabela"] == tab and r["tam_pagina_kb"] == 2],
            key=lambda x: x["qtd_quadros"],
        )
        plt.plot(
            [r["qtd_quadros"] for r in subset],
            [r["tempo_consulta_ms"] for r in subset],
            marker="s",
            label=nomes_tabelas[tab],
        )

    plt.xlabel("Quantidade de Quadros Físicos (RAM / Página)")
    plt.ylabel("Tempo Acumulado nas Consultas (ms)")
    plt.title("Desempenho das Consultas vs. Quantidade de Quadros (Página = 2 KB)")
    plt.grid(True, linestyle="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{PASTA_SAIDA}/grafico_tempo_consultas.png", dpi=150)
    plt.close()

    # Gráfico 3: Page Faults vs. Tamanho da Memória por Algoritmo
    logs_disponiveis = sorted(set(r["arquivo"] for r in dados_algoritmos))
    log_principal = logs_disponiveis[0]

    plt.figure(figsize=(9, 5))
    for algo in ["lru", "clock", "random"]:
        subset = sorted(
            [r for r in dados_algoritmos if r["algoritmo"] == algo and r["arquivo"] == log_principal],
            key=lambda x: x["tam_memoria_kb"],
        )
        plt.plot(
            [r["tam_memoria_kb"] for r in subset],
            [r["faults"] for r in subset],
            marker="^",
            label=algo.upper(),
        )

    plt.xlabel("Tamanho da Memória Física (KB)")
    plt.ylabel("Total de Page Faults")
    plt.title(f"Page Faults por Algoritmo de Substituição ({log_principal}, Página = 4 KB)")
    plt.grid(True, linestyle="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{PASTA_SAIDA}/grafico_faults_algoritmos.png", dpi=150)
    plt.close()

    print(f"Gráficos salvos em '{PASTA_SAIDA}/'.")


def main():
    logs = sorted(glob.glob("data/*.log"))
    if not logs:
        sys.exit("Erro: nenhum arquivo .log encontrado na pasta 'data/'.")

    log_base = "data/compilador.log" if "data/compilador.log" in logs else logs[0]
    tabelas = ["densa", "th2", "th3", "invertida"]
    algoritmos = ["lru", "clock", "random"]

    # Benchmark das 4 estruturas de tabela
    print(f"Experimento 1: Comparando Tabelas ({log_base})")
    dados_tabelas = []
    paginas = [2, 4, 8, 16, 32, 64]
    memorias_tab = [128, 1024, 4096, 16384]

    for tab in tabelas:
        for tam_pag in paginas:
            for tam_mem in memorias_tab:
                print(f"Rodando: tabela={tab:9s} | pag={tam_pag:2d}KB | mem={tam_mem:5d}KB")
                res = rodar_simulacao("lru", log_base, tam_pag, tam_mem, tab)
                dados_tabelas.append(res)

    salvar_csv(f"{PASTA_SAIDA}/resultados_tabelas.csv", dados_tabelas)

    # Benchmark dos algoritmos de substituição (usando th2)
    print("\nExperimento 2: Comparando Algoritmos")
    dados_algoritmos = []
    memorias_algo = [128, 256, 512, 1024, 2048, 4096]

    for arq in logs:
        for algo in algoritmos:
            for tam_mem in memorias_algo:
                print(f"Rodando: log={os.path.basename(arq):15s} | algo={algo:6s} | mem={tam_mem:4d}KB")
                res = rodar_simulacao(algo, arq, 4, tam_mem, "th2")
                dados_algoritmos.append(res)

    salvar_csv(f"{PASTA_SAIDA}/resultados_algoritmos.csv", dados_algoritmos)

    gerar_graficos(dados_tabelas, dados_algoritmos)


if __name__ == "__main__":
    main()