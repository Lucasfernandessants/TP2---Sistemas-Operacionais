import csv
import glob
import os
import re
import statistics
import subprocess
import sys
from collections import defaultdict

# ============================================================
# CONFIGURACOES
# ============================================================

EXECUTAVEL = "./simulador"
PASTA_DADOS = "data"
PASTA_SAIDA = "resultados"
LOGS_OFICIAIS = [
    "compilador.log",
    "compressor.log",
    "matriz.log",
    "simulador.log",
]

# As quatro estruturas de tabela de paginas implementadas.
TABELAS = ["densa", "th2", "th3", "invertida"]

# o tempo será medido sob diferentes politicas de substituicao.
ALGORITMOS = ["lru", "clock", "random", "opt"]

MEMORIAS = [128, 256, 512, 1024, 2048, 4096, 16384]
PAGINA_FIXA = 4

TIMEOUT = 600

ARQUIVO_BRUTO = os.path.join(PASTA_SAIDA, "tempos_consulta_brutos.csv")
ARQUIVO_MEDIAS = os.path.join(PASTA_SAIDA, "tempos_consulta_medias.csv")

os.makedirs(PASTA_SAIDA, exist_ok=True)


def extrair_tempo_consulta(saida):
    """Extrai o tempo de consulta informado pelo proprio simulador."""
    resultado = re.search(r"Tempo de consulta:\s*([\d.,]+)", saida)
    if resultado is None:
        raise ValueError(
            "Metrica 'Tempo de consulta' nao encontrada na saida:\n" + saida
        )
    return float(resultado.group(1).replace(",", "."))


def rodar_simulacao(algoritmo, arquivo_log, memoria, tabela):
    comando = [
        EXECUTAVEL,
        algoritmo,
        arquivo_log,
        str(PAGINA_FIXA),
        str(memoria),
        tabela,
    ]

    processo = subprocess.run(
        comando,
        capture_output=True,
        text=True,
        timeout=TIMEOUT,
    )

    if processo.returncode != 0:
        raise RuntimeError(
            f"Codigo de retorno: {processo.returncode}\n"
            f"STDOUT:\n{processo.stdout}\n"
            f"STDERR:\n{processo.stderr}"
        )

    tempo = extrair_tempo_consulta(processo.stdout)

    return {
        "arquivo": os.path.basename(arquivo_log),
        "algoritmo": algoritmo,
        "tabela": tabela,
        "tam_pagina_kb": PAGINA_FIXA,
        "tam_memoria_kb": memoria,
        "tempo_consulta_ms": tempo,
    }


def salvar_csv(caminho, registros):
    if not registros:
        print(f"Nenhum dado para salvar em {caminho}.")
        return

    with open(caminho, "w", newline="", encoding="utf-8") as arquivo:
        escritor = csv.DictWriter(arquivo, fieldnames=list(registros[0].keys()))
        escritor.writeheader()
        escritor.writerows(registros)

    print(f"Arquivo salvo: {caminho}")


def calcular_medias(registros):
    grupos = defaultdict(list)
    for registro in registros:
        chave = (registro["tam_memoria_kb"], registro["tabela"])
        grupos[chave].append(registro["tempo_consulta_ms"])

    medias = []
    for memoria in MEMORIAS:
        linha = {"tam_memoria_kb": memoria}
        for tabela in TABELAS:
            valores = grupos.get((memoria, tabela), [])
            linha[tabela] = round(statistics.mean(valores), 6) if valores else ""
            linha[f"{tabela}_amostras"] = len(valores)
        medias.append(linha)

    return medias


def imprimir_tabela_latex(medias):
    print("\nTabela LaTeX (tempos em ms; media dos logs e algoritmos):\n")
    print(r"\begin{tabular}{rcccc}")
    print(r"\toprule")
    print(r"\textbf{Memoria (KB)} & \textbf{Densa} & \textbf{TH2} & \textbf{TH3} & \textbf{Invertida} \\")
    print(r"\midrule")

    for linha in medias:
        valores = [linha[t] for t in TABELAS]
        formatados = [f"{v:.3f}" if isinstance(v, (int, float)) else "--" for v in valores]
        print(
            f"{linha['tam_memoria_kb']} KB & "
            + " & ".join(formatados)
            + r" \\"
        )

    print(r"\bottomrule")
    print(r"\end{tabular}")


def main():
    if not os.path.isfile(EXECUTAVEL):
        sys.exit(
            f"Executavel '{EXECUTAVEL}' nao encontrado. "
            "Compile o projeto antes de executar o benchmark."
        )

    if not os.access(EXECUTAVEL, os.X_OK):
        sys.exit(
            f"'{EXECUTAVEL}' nao possui permissao de execucao. "
            "Execute: chmod +x simulador"
        )

    logs = [
        os.path.join(PASTA_DADOS, nome)
        for nome in LOGS_OFICIAIS
        if os.path.isfile(os.path.join(PASTA_DADOS, nome))
    ]
    ausentes = [nome for nome in LOGS_OFICIAIS if not os.path.isfile(os.path.join(PASTA_DADOS, nome))]
    if ausentes:
        sys.exit(
            "Estao faltando arquivos de log obrigatorios na pasta data/: "
            + ", ".join(ausentes)
        )

    print("Arquivos utilizados:")
    for log in logs:
        print(f"  - {log}")

    total = len(logs) * len(ALGORITMOS) * len(TABELAS) * len(MEMORIAS)
    print(f"\nTotal de execucoes: {total}")
    print(
        f"Cada execucao usa pagina de {PAGINA_FIXA} KB, "
        "um algoritmo, um log e uma estrutura de tabela."
    )
    print("Os tempos serao agregados pela media dos logs e algoritmos.\n")

    registros = []
    falhas = []
    atual = 0

    for memoria in MEMORIAS:
        for log in logs:
            for algoritmo in ALGORITMOS:
                for tabela in TABELAS:
                    atual += 1
                    print(
                        f"[{atual}/{total}] memoria={memoria:5d} KB | "
                        f"log={os.path.basename(log):15s} | "
                        f"algoritmo={algoritmo:6s} | tabela={tabela}",
                        flush=True,
                    )

                    try:
                        registros.append(
                            rodar_simulacao(algoritmo, log, memoria, tabela)
                        )
                    except Exception as erro:
                        print(f"  ERRO: {erro}")
                        falhas.append({
                            "arquivo": os.path.basename(log),
                            "algoritmo": algoritmo,
                            "tabela": tabela,
                            "tam_memoria_kb": memoria,
                            "erro": str(erro),
                        })

    salvar_csv(ARQUIVO_BRUTO, registros)

    medias = calcular_medias(registros)
    salvar_csv(ARQUIVO_MEDIAS, medias)
    imprimir_tabela_latex(medias)

    if falhas:
        caminho_falhas = os.path.join(PASTA_SAIDA, "falhas_tempo_consulta.csv")
        salvar_csv(caminho_falhas, falhas)
        print(f"\nAtencao: {len(falhas)} execucoes falharam. Consulte {caminho_falhas}.")

    print("\nBenchmark de tempo de consulta finalizado.")
    print(f"Resultados detalhados: {ARQUIVO_BRUTO}")
    print(f"Medias para o relatorio: {ARQUIVO_MEDIAS}")


if __name__ == "__main__":
    main()
