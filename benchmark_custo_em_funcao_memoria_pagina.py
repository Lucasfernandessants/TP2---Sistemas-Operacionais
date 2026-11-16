
import csv
import glob
import os
import re
import subprocess
import sys
from pathlib import Path

try:
    import matplotlib.pyplot as plt
    TEM_MATPLOTLIB = True
except ImportError:
    TEM_MATPLOTLIB = False


# ============================================================
# CONFIGURAÇÕES
# ============================================================

EXECUTAVEL = "./simulador"
PASTA_SAIDA = "resultados"

TABELAS = ["densa", "th2", "th3", "invertida"]

ALGORITMOS = ["lru", "clock", "random", "opt"]

# Experimento 1: tamanho da página variável.
PAGINAS = [2, 4, 8, 16, 32, 64]
MEMORIA_FIXA = 1024

# Experimento 2: tamanho da memória variável.
MEMORIAS = [128, 256, 512, 1024, 2048, 4096, 16384]
PAGINA_FIXA = 4

# Tempo máximo de execução de cada simulação (segundos).
TIMEOUT = 600

os.makedirs(PASTA_SAIDA, exist_ok=True)

def carregar_csv(caminho):
    with open(caminho, "r", newline="", encoding="utf-8") as arquivo:
        leitor = csv.DictReader(arquivo)
        dados = []

        for linha in leitor:
            registro = {}

            for chave, valor in linha.items():
                try:
                    registro[chave] = float(valor) if "." in valor else int(valor)
                except (ValueError, TypeError):
                    registro[chave] = valor

            dados.append(registro)

        return dados

# ============================================================
# EXECUÇÃO DA SIMULAÇÃO
# ============================================================

def rodar_simulacao(algoritmo, arquivo_log, tam_pagina, tam_memoria, tabela):

    cmd = [
        EXECUTAVEL,
        algoritmo,
        arquivo_log,
        str(tam_pagina),
        str(tam_memoria),
        tabela
    ]

    res = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        timeout=TIMEOUT
    )

    if res.returncode != 0:
        raise RuntimeError(
            f"Processo retornou código {res.returncode}.\n"
            f"STDOUT:\n{res.stdout}\n"
            f"STDERR:\n{res.stderr}"
        )

    saida = res.stdout

    def extrair(padrao, nome):
        resultado = re.search(padrao, saida)

        if resultado is None:
            raise ValueError(
                f"Métrica '{nome}' não encontrada na saída:\n{saida}"
            )

        return resultado.group(1)

    acessos = int(extrair(
        r"Total de acessos à memória:\s*(\d+)",
        "acessos"
    ))

    faults = int(extrair(
        r"Páginas lidas:\s*(\d+)",
        "page faults"
    ))

    escritas = int(extrair(
        r"Páginas escritas:\s*(\d+)",
        "páginas escritas"
    ))

    memoria_bytes = int(extrair(
        r"Mem[oó]ria da tabela:\s*(\d+)",
        "memória da tabela"
    ))

    tempo_consulta = float(extrair(
        r"Tempo de consulta:\s*([\d.]+)",
        "tempo de consulta"
    ))

    tempo_total = float(extrair(
        r"Tempo total:\s*([\d.]+)",
        "tempo total"
    ))

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
        "taxa_faults_pct": round(
            (faults / acessos) * 100, 4
        ) if acessos > 0 else 0,
        "memoria_bytes": memoria_bytes,
        "memoria_kb": round(
            memoria_bytes / 1024.0, 4
        ),
        "tempo_consulta_ms": tempo_consulta,
        "tempo_total_ms": tempo_total,
    }


# ============================================================
# SALVAMENTO DOS RESULTADOS
# ============================================================

def salvar_csv(caminho, registros):

    if not registros:
        print(f"Nenhum resultado para salvar em {caminho}.")
        return

    with open(caminho, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(
            f,
            fieldnames=registros[0].keys()
        )

        writer.writeheader()
        writer.writerows(registros)

    print(f"Salvo: {caminho} ({len(registros)} registros)")


# ============================================================
# EXECUÇÃO DE UM CONJUNTO DE EXPERIMENTOS
# ============================================================

def executar_experimentos(nome, logs, paginas, memorias):

    resultados = []
    falhas = []

    total = (
        len(logs)
        * len(ALGORITMOS)
        * len(TABELAS)
        * len(paginas)
        * len(memorias)
    )

    executados = 0

    print(f"\n{'=' * 65}")
    print(f"INICIANDO {nome}")
    print(f"Total de simulações previstas: {total}")
    print(f"{'=' * 65}")

    for arq in logs:

        for algoritmo in ALGORITMOS:

            for tabela in TABELAS:

                for tam_pag in paginas:

                    for tam_mem in memorias:

                        executados += 1

                        print(
                            f"[{executados}/{total}] "
                            f"log={os.path.basename(arq):15s} | "
                            f"algo={algoritmo:7s} | "
                            f"tabela={tabela:9s} | "
                            f"pag={tam_pag:2d}KB | "
                            f"mem={tam_mem:5d}KB",
                            flush=True
                        )

                        try:

                            res = rodar_simulacao(
                                algoritmo,
                                arq,
                                tam_pag,
                                tam_mem,
                                tabela
                            )

                            res["experimento"] = nome
                            resultados.append(res)

                        except Exception as e:

                            print(f"ERRO: {e}")

                            falhas.append({
                                "experimento": nome,
                                "arquivo": os.path.basename(arq),
                                "algoritmo": algoritmo,
                                "tabela": tabela,
                                "tam_pagina_kb": tam_pag,
                                "tam_memoria_kb": tam_mem,
                                "erro": str(e)
                            })

    print(f"\n{nome} concluído.")
    print(f"Simulações bem-sucedidas: {len(resultados)}")
    print(f"Simulações com falha: {len(falhas)}")

    return resultados, falhas


# ============================================================
# GRÁFICOS
# ============================================================

def gerar_graficos(dados_pagina, dados_memoria):

    if not TEM_MATPLOTLIB:
        print(
            "\nAviso: matplotlib não instalado. "
            "Ative o ambiente virtual e execute: python -m pip install matplotlib"
        )
        return

    nomes_tabelas = {
        "densa": "Densa",
        "th2": "Hierárquica (2 níveis)",
        "th3": "Hierárquica (3 níveis)",
        "invertida": "Invertida"
    }

    # Remove apenas os gráficos antigos gerados por esta versão do benchmark,
    # evitando que os 64 gráficos anteriores continuem misturados aos novos.
    prefixos_antigos = (
        "pagina_faults_", "pagina_memoria_",
        "memoria_faults_", "memoria_tempo_"
    )
    for nome in os.listdir(PASTA_SAIDA):
        if nome.endswith(".png") and nome.startswith(prefixos_antigos):
            os.remove(os.path.join(PASTA_SAIDA, nome))

    # ------------------------------------------------------------
    # 8 GRÁFICOS: PAGE FAULTS POR ARQUIVO E EXPERIMENTO
    #
    # O número de page faults deve ser independente da estrutura
    # usada para representar a tabela de páginas. Por isso, usamos
    # a tabela densa como referência, evitando quatro linhas
    # redundantes para cada algoritmo.
    # ------------------------------------------------------------

    logs = sorted(set(
        r["arquivo"] for r in dados_pagina + dados_memoria
    ))

    for log in logs:
        nome_log = Path(log).stem

        # Experimento 1: página variável, memória fixa.
        plt.figure(figsize=(9, 5))

        for algoritmo in ALGORITMOS:
            subset = sorted(
                [
                    r for r in dados_pagina
                    if r["arquivo"] == log
                    and r["tabela"] == "densa"
                    and r["algoritmo"] == algoritmo
                ],
                key=lambda x: x["tam_pagina_kb"]
            )

            if subset:
                plt.plot(
                    [r["tam_pagina_kb"] for r in subset],
                    [r["faults"] for r in subset],
                    marker="o",
                    label=algoritmo.upper()
                )

        plt.xlabel("Tamanho da Página (KB)")
        plt.ylabel("Total de Page Faults")
        plt.title(
            f"Page Faults x Tamanho da Página\n"
            f"{log} | RAM = {MEMORIA_FIXA} KB"
        )
        plt.xticks(PAGINAS)
        plt.grid(True, linestyle="--", alpha=0.5)
        plt.legend(title="Algoritmo")
        plt.tight_layout()
        plt.savefig(
            f"{PASTA_SAIDA}/page_faults_pagina_{nome_log}.png",
            dpi=150
        )
        plt.close()

        # Experimento 2: memória variável, página fixa.
        plt.figure(figsize=(9, 5))

        for algoritmo in ALGORITMOS:
            subset = sorted(
                [
                    r for r in dados_memoria
                    if r["arquivo"] == log
                    and r["tabela"] == "densa"
                    and r["algoritmo"] == algoritmo
                ],
                key=lambda x: x["tam_memoria_kb"]
            )

            if subset:
                plt.plot(
                    [r["tam_memoria_kb"] for r in subset],
                    [r["faults"] for r in subset],
                    marker="s",
                    label=algoritmo.upper()
                )

        plt.xlabel("Tamanho da Memória Física (KB)")
        plt.ylabel("Total de Page Faults")
        plt.title(
            f"Page Faults x Tamanho da Memória\n"
            f"{log} | Página = {PAGINA_FIXA} KB"
        )
        plt.xticks(MEMORIAS, rotation=30)
        plt.grid(True, linestyle="--", alpha=0.5)
        plt.legend(title="Algoritmo")
        plt.tight_layout()
        plt.savefig(
            f"{PASTA_SAIDA}/page_faults_memoria_{nome_log}.png",
            dpi=150
        )
        plt.close()

    # ------------------------------------------------------------
    # 2 GRÁFICOS: CUSTO DE MEMÓRIA DAS QUATRO TABELAS
    #
    # Cada ponto é a média do consumo registrado para a mesma
    # tabela/configuração, considerando os quatro logs e algoritmos.
    # Isso evita linhas repetidas para medições equivalentes.
    # ------------------------------------------------------------

    def media_memoria_por_tabela(dados, eixo):
        agrupados = {}

        for registro in dados:
            chave = (registro["tabela"], registro[eixo])
            agrupados.setdefault(chave, []).append(registro["memoria_kb"])

        return {
            chave: sum(valores) / len(valores)
            for chave, valores in agrupados.items()
        }

    # Memória ocupada pela tabela conforme o tamanho da página.
    medias_pagina = media_memoria_por_tabela(
        dados_pagina, "tam_pagina_kb"
    )

    plt.figure(figsize=(9, 5))
    for tabela in TABELAS:
        pontos = sorted(
            [
                (pagina, valor)
                for (nome_tabela, pagina), valor in medias_pagina.items()
                if nome_tabela == tabela
            ],
            key=lambda item: item[0]
        )

        if pontos:
            plt.plot(
                [p[0] for p in pontos],
                [p[1] for p in pontos],
                marker="o",
                label=nomes_tabelas[tabela]
            )

    plt.xlabel("Tamanho da Página (KB)")
    plt.ylabel("Memória ocupada pela tabela (KB)")
    plt.title(
        f"Custo de Memória das Tabelas x Tamanho da Página\n"
        f"RAM = {MEMORIA_FIXA} KB (média dos arquivos e algoritmos)"
    )
    plt.xticks(PAGINAS)
    plt.grid(True, linestyle="--", alpha=0.5)
    plt.legend(title="Estrutura")
    plt.tight_layout()
    plt.savefig(
        f"{PASTA_SAIDA}/uso_tabela_tamanho_pagina.png",
        dpi=150
    )
    plt.close()

    # Memória ocupada pela tabela conforme o tamanho da memória física.
    medias_memoria = media_memoria_por_tabela(
        dados_memoria, "tam_memoria_kb"
    )

    plt.figure(figsize=(9, 5))
    for tabela in TABELAS:
        pontos = sorted(
            [
                (memoria, valor)
                for (nome_tabela, memoria), valor in medias_memoria.items()
                if nome_tabela == tabela
            ],
            key=lambda item: item[0]
        )

        if pontos:
            plt.plot(
                [p[0] for p in pontos],
                [p[1] for p in pontos],
                marker="s",
                label=nomes_tabelas[tabela]
            )

    plt.xlabel("Tamanho da Memória Física (KB)")
    plt.ylabel("Memória ocupada pela tabela (KB)")
    plt.title(
        f"Custo de Memória das Tabelas x Tamanho da Memória\n"
        f"Página = {PAGINA_FIXA} KB (média dos arquivos e algoritmos)"
    )
    plt.xticks(MEMORIAS, rotation=30)
    plt.grid(True, linestyle="--", alpha=0.5)
    plt.legend(title="Estrutura")
    plt.tight_layout()
    plt.savefig(
        f"{PASTA_SAIDA}/uso_tabela_tamanho_memoria.png",
        dpi=150
    )
    plt.close()

    print(
        f"\nForam gerados {2 * len(logs) + 2} gráficos "
        f"em '{PASTA_SAIDA}/'."
    )


# ============================================================
# MAIN
# ============================================================

def main():

    if not os.path.isfile(EXECUTAVEL):
        sys.exit(
            f"Erro: executável '{EXECUTAVEL}' não encontrado. "
            "Compile o simulador antes de executar o benchmark."
        )

    if not os.access(EXECUTAVEL, os.X_OK):
        sys.exit(
            f"Erro: '{EXECUTAVEL}' não possui permissão de execução. "
            "Execute: chmod +x simulador"
        )

    logs = sorted(glob.glob("data/*.log"))

    if not logs:
        sys.exit("Erro: nenhum arquivo .log encontrado na pasta 'data/'.")

    print(f"Arquivos de log encontrados: {len(logs)}")

    for log in logs:
        print(f"  - {log}")

    arquivo_pagina = f"{PASTA_SAIDA}/resultados_tamanho_pagina.csv"
    arquivo_memoria = f"{PASTA_SAIDA}/resultados_tamanho_memoria.csv"
    arquivo_falhas = f"{PASTA_SAIDA}/falhas.csv"

    # ========================================================
    # VERIFICA SE OS RESULTADOS JÁ EXISTEM
    # ========================================================

    if os.path.exists(arquivo_pagina) and os.path.exists(arquivo_memoria):

        print("\nResultados encontrados! Pulando simulações...")

        dados_pagina = carregar_csv(arquivo_pagina)
        dados_memoria = carregar_csv(arquivo_memoria)

        if os.path.exists(arquivo_falhas):
            todas_falhas = carregar_csv(arquivo_falhas)
        else:
            todas_falhas = []

    else:

        print("\nResultados não encontrados. Executando simulações...")

        # ========================================================
        # EXPERIMENTO 1: TAMANHO DA PÁGINA
        # ========================================================

        dados_pagina, falhas_pagina = executar_experimentos(
            nome="Impacto do Tamanho da Página",
            logs=logs,
            paginas=PAGINAS,
            memorias=[MEMORIA_FIXA]
        )

        salvar_csv(arquivo_pagina, dados_pagina)

        # ========================================================
        # EXPERIMENTO 2: TAMANHO DA MEMÓRIA
        # ========================================================

        dados_memoria, falhas_memoria = executar_experimentos(
            nome="Impacto do Tamanho da Memória",
            logs=logs,
            paginas=[PAGINA_FIXA],
            memorias=MEMORIAS
        )

        salvar_csv(arquivo_memoria, dados_memoria)

        # ========================================================
        # REGISTRO DE FALHAS
        # ========================================================

        todas_falhas = falhas_pagina + falhas_memoria

        salvar_csv(arquivo_falhas, todas_falhas)


    # ========================================================
    # GRÁFICOS
    # ========================================================

    gerar_graficos(dados_pagina, dados_memoria)


    # ========================================================
    # RESUMO FINAL
    # ========================================================

    total_sucesso = len(dados_pagina) + len(dados_memoria)

    print("\n" + "=" * 65)
    print("BENCHMARK FINALIZADO")
    print("=" * 65)
    print(f"Total de simulações bem-sucedidas: {total_sucesso}")
    print(f"Total de simulações com falha: {len(todas_falhas)}")
    print(f"Resultados disponíveis na pasta: {PASTA_SAIDA}/")
    print("=" * 65)


if __name__ == "__main__":
    main()
