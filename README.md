# Immediate-Priority-Ceiling-Protocol-IPCP-
Neste trabalho, é implementado o protocolo de prioridade teto imediato (Immediate Priority Ceiling Protocol - IPCP) no EPOS.

# Conteudo do Projeto

- epos: Pasta relativa ao sistema operacional de tempos real com modificacoes nos arquivos rt_rm.cc, semaphore.cc, semaphore.h e types.h afim de adaptar ao escalonador IPCP.
- rts-dataplot: Pasta que contem um programa python para visualizar a saida do programa executado na pasta epos.
- Enunciado IPCP.pdf: PDF contendo a descricao do trabalho.

# Execucao do EPOS

Para executar o programa deve se atentar aos requisitos estabelecidos no Enunciado do problema.

Com esses requisitos minimos implentados dentro da pasta do epos eh so executar o comando para executa-lo:
> make all
> make APPLICATION=rt_rm clean run

# Plotando IPCP implementado no EPOS

Primeiramente deve-se estar dentro da pasta do epos e digitar o comando:
> make APPLICATION=rt_rm run > rt_rm_output

Com isso feito sera gerado um arquivo: rt_rm_output na mesma pasta.
Apos isso, deve-se mover o arquivo para a pasta rts-dataplot e executar o comando no terminal:
> python3 rt_plot.py

