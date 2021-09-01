Esse script foi feito para as atividades de escalonamento em tempo real do OpenEPOS.
A saída da aplicação deve estar de acordo com o padrão dos arquivos de exemplo que deixei.

## Requisitos
Python 3  
Matplotlib

## Uso
Para uma determinada aplicação do EPOS, e.g. rt_rm, execute: 
>$ make APPLICATION=rt_rm run > rt_rm_output

Em seguida, ajuste a main do script em Python para receber o(s) arquivo(s) correto(s). 
(Suporta até dois gráficos de uma vez)
