**UC Sistemas Operacionais
ICT/UNIFESP**

**Prof. Bruno Kimura
bruno.kimura@unifesp.br
19/04/2018**

**LAB 2: Map Reduce**

**Metodologia:** Trabalho individual ou em grupo de no máximo 3 (três) alunos a ser desenvolvido
em laboratório de informática através de codificação na linguagem C.

**Data de entrega: 26/04/2018**

**Forma de entrega:** Código .c deve ser enviado no SEAD. Insira como comentário no código o
nome e matrícula de cada integrante do grupo.

**Observação:** Somente serão aceitos trabalhos **autênticos**. Cópias (entre grupos e/ou de fontes da
Internet) serão anuladas.

**Descrição:**
Utilizando as ferramentas de sincronização e comunicação entre processos discutidas em aula,
implemente uma aplicação de contagem de palavras em documentos baseada no modelo de
programação **MapReduce**.
Dezenas de milhares códigos foram implementados utilizando o modelo MapReduce na Google,
incluindo algoritmos para processamento de grafos de larga escala, processamento de texto,
mineração de dados, aprendizado de máquina, traduções de máquina, etc [1].

**Referência:**
[1]Jeffrey Dean and Sanjay Ghemawat. 2008. **_MapReduce: simplified data processing on large clusters_****.** Commun.
ACM 51, 1 (January 2008), 107-113. DOI: https://doi.org/10.1145/1327452.

**Sobre o Modelo MapReduce:**
O modelo prevê conceitualmente duas funções:

- **Mapeamento** : produz um lista de pares<chave, valor> a partir de entradas
    estruturadas de pares <chave, valor> de diferentes tipos:
       ```map(k1, v1) → list(k2, v2)```
- **Redução** : produz um lista de valores a partir de uma entrada que consiste em uma chave e
    uma lista de valores associados a essa chave.
       ```reduce(k2, list(v2)) → list(v2)```

Neste exercício de laboratório, as funções de _map_ e _reduce_ podem ser representadas pelos pseudo-
códigos abaixo. A cada ocorrência da palavra _w_ , a função map emite a palavra a contagem de
ocorrência associada “1”. A função _reduce_ então soma todas as contagens emitidas de uma palavra
específica _w_.
```c
map (String key, String value):
// key: document name
// value: document contents
for each word w in value:
EmitIntermediate(w, "1");
```
```c
reduce (String key, Iterator values):
// key: a word
// values: a list of counts
int result = 0;
for each v in values:
result += ParseInt(v);
Emit(AsString(result));
```

A Figura 1 ilustra a execução de uma implementação baseada no modelo MapReduce [1]. Nesta
implementação, a aplicação ( _user program_ ) divide ( _split_ ) as entradas em pedaços contendo, cada
um, diferentes partes do(s) arquivo(s) de entrada. Várias entidades são executadas de forma
concorrente. Há trabalhares ( _workers_ ) que são responsáveis pelas tarefas de _map_ e _reduce_. A
entidade especial é a mestre ( _master_ ), que é responsável por alocar aos trabalhadores ociosos tarefas
de map e _reduce_. Pares intermediários <chave, valor> gerados pelos trabalhadores na tarefa de map
são buferizados em memória. Periodicamente, o buffer pode ser descarregado em disco,
particionado-o em R arquivos por uma função de partição. O trabalhador alocado na tarefa de
_reduce_ , lê as partições e ordena os pares pelo valore de chaves. Ao ordenar, todas as ocorrências
(valor) de uma mesma chave são agrupadas. Finalmente, os trabalhadores em _reduce_ , iteram sobre
os pares ordenados e soma as ocorrências. A saída de uma função _reduce_ (contagem das ocorrências
de uma chave) é anexada ao arquivo de saída.

Figura 1: Visão de execução de uma implementação baseada em MapReduce. Fonte: [1]


