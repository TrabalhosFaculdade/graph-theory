/* 
 * REPRESENTACAO DE GRAFOS
 * 
 * Daniel Dias de Lima      31687679
 * Leandro Alexandre        31616720
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

/**
 * Definição das cores dos algoritmos de busca
*/
#define BRANCO 1
#define CINZA 2
#define PRETO 3

#define ELEMENTO_NAO_DEFINIDO -1

/* 
 * Estrutura de dados para representar grafos
 */
typedef struct aresta
{ /* Celula de uma lista de arestas */
    int nome;
    struct aresta *prox;
} Aresta;

typedef struct vert
{ /* Cada vertice tem um ponteiro para uma lista de arestas incidentes nele */
    int nome;
    int componente;
    Aresta *prim;

    int paiBuscaLargura;
    int corBuscaLargura;
    int distanciaBuscaLargura;

    int paiBuscaProfundida;
    int tempoDescobertaBuscaProf;
    int tempoFinalizacaoBuscaProf;
    int corBuscaProfundida;
} Vertice;

/**
 * Estrutura usada para fazer o controle dos vertices 
 * que precisam ser visitados nos algoritmos de busca. 
 * 
 * Fila não circular, a guardar, no maximo, uma quantidade
 * de valores limitada a ordem de um grafo
*/
typedef struct fila
{
    int *valores;
    int tamanhoMax;
    int indiceRetirada;
    int indiceInsercao;
} Fila;

/*
 * Declaracao das funcoes para manipulacao de grafos 
 */
void imprimeGrafo(Vertice G[], int ordem);
void criaGrafo(Vertice **G, int ordem);
int acrescentaAresta(Vertice G[], int ordem, int v1, int v2);
int calculaTamanho(Vertice G[], int ordem);


void buscaLargura(Vertice G[], int ordem, int verticeInicial);
bool eConexoBLargura(Vertice G[], int ordem);

void buscaProfundida(Vertice G[], int ordem);
void buscaProfundidaVisita(Vertice G[], int ordem, int verticeAtual, int *tempo, int componente);
bool eConexoBProf(Vertice G[], int ordem);

/**
 * Operacoes de gerenciamento da fila, usada para o gerenciamento
 * da ordem de navegacao dos vertices do grafo nos algoritmos de busca
*/
Fila *inicializaFila(int tamanho);
void liberaFila(Fila *fila);
void enfileira(Fila *fila, int elemento);
int desinfileira(Fila *fila);
bool filaEstaVazia(Fila *fila);

/*
 * Implementacao das funcoes para manipulacao de grafos 
 */

/* Criacao de um grafo com ordem predefinida e, inicilamente, sem nenhuma aresta */
void criaGrafo(Vertice **G, int ordem)
{
    int i;
    *G = (Vertice *)malloc(sizeof(Vertice) * ordem); /* Alocacao dinamica de um vetor de vertices */

    for (i = 0; i < ordem; i++)
    {
        (*G)[i].nome = i;
        (*G)[i].componente = ELEMENTO_NAO_DEFINIDO;       /* 0: sem componente atribuida */
        (*G)[i].prim = NULL;                                /* Cada vertice sem nenhuma aresta incidente */
        (*G)[i].paiBuscaLargura = ELEMENTO_NAO_DEFINIDO;    /* Não possui pai antes da busca em largura ser executada */
        (*G)[i].paiBuscaProfundida = ELEMENTO_NAO_DEFINIDO; /* Não possui pai antes da busca em profundida ser executada */
    }
}

/**
 * Acrescenta uma aresta em um grafo previamente criado.
 * Devem ser passados os extremos v1 e v2 da aresta a ser acrescentada  
*/
int acrescentaAresta(Vertice G[], int ordem, int v1, int v2)
{
    Aresta *A1;
    Aresta *A2;

    if (v1 < 0 || v1 >= ordem) /* Testo se vertices sao validos */
        return 0;
    if (v2 < 0 || v2 >= ordem)
        return 0;

    /* Acrescento aresta na lista do vertice v1 */
    A1 = (Aresta *)malloc(sizeof(Aresta));
    A1->nome = v2;
    A1->prox = G[v1].prim;
    G[v1].prim = A1;

    /* Acrescento aresta na lista do vertice v2 */
    A2 = (Aresta *)malloc(sizeof(Aresta));
    A2->nome = v1;
    A2->prox = G[v2].prim;
    G[v2].prim = A2;

    return 1;
}

/*  Funcao que retorna o tamanho de um grafo */
int calculaTamanho(Vertice G[], int ordem)
{
    int i;
    int totalArestas = 0;

    for (i = 0; i < ordem; i++)
    {
        int j;
        Aresta *aux = G[i].prim;
        for (j = 0; aux != NULL; aux = aux->prox, j++)
            ;
        totalArestas += j;
    }
    return totalArestas / 2 + ordem;
}
/**
 * Imprime um grafo exibindo seus vertices e as arestas incidentes neles
*/
void imprimeGrafo(Vertice G[], int ordem)
{
    int i;
    Aresta *aux;

    printf("Ordem:       %d\n", ordem);
    printf("Tamanho:     %d\n", calculaTamanho(G, ordem));
    printf("===Lista de Adjacencia===:\n");

    for (i = 0; i < ordem; i++)
    {
        printf("V%d (Comp:%2d): ", i, G[i].componente);
        aux = G[i].prim;
        for (; aux != NULL; aux = aux->prox)
            printf("%3d", aux->nome);

        printf("\n");
    }
    printf("=========================:\n\n");
}

/**
 * Inicializacao dos valores da fila, 
 * retorno do ponteiro para a fila inicializada
*/
Fila *inicializaFila(int tamanho)
{
    Fila *fila = (Fila *)malloc(sizeof(Fila));

    fila->indiceRetirada = 0; /*indice a ser usado quando retirando primeiro elemento*/
    fila->indiceInsercao = 0; /*indice a ser usado quando inserindo primeiro elemento*/
    fila->tamanhoMax = tamanho;
    fila->valores = (int *)calloc(tamanho, sizeof(int)); /*todos os elementos da fila igual a zero*/

    return fila;
}

/**
 * Adicao de elemento na fila. Considerando a estrutura de dados,
 * FIFO, vai ser o último a sair considerando todos os elementos 
 * que foram enfileirados anteriormente
*/
void enfileira(Fila *fila, int elemento)
{
    /*validando se fila atingiu seu limite*/
    if (fila->indiceInsercao >= fila->tamanhoMax)
    {
        printf("Fila atingiu sua capacidade máxima: %d", fila->tamanhoMax);
        return;
    }

    fila->valores[fila->indiceInsercao] = elemento;
    fila->indiceInsercao++;
}

int desinfileira(Fila *fila)
{
    int elementoRetirado;

    /*validando se estamos desinfileirando antes do elemento ser inserido*/
    if (fila->indiceInsercao < fila->indiceRetirada)
    {
        printf("Fazendo retirada da fila antes de adicionar elemento. %d - %d", fila->indiceInsercao, fila->indiceRetirada);
        return ELEMENTO_NAO_DEFINIDO;
    }

    elementoRetirado = fila->valores[fila->indiceRetirada];
    fila->indiceRetirada++;

    return elementoRetirado;
}

/**
 * Funcao de utilidade para checar se a fila 
 * passada como parametro esta vazia ou nao.
 * No algoritmo de busca em profundida, 
 * usada como condicao de parada
*/
bool filaEstaVazia(Fila *fila)
{
    return fila->indiceRetirada >= fila->indiceInsercao;
}

/**
 * Funcao de utilidade para a liberação dos elementos da fila da memoria
*/
void liberaFila(Fila *fila)
{
    free(fila->valores);
    free(fila);
}

void buscaLargura(Vertice G[], int ordem, int verticeInicial)
{
    Fila *Q;
    Aresta *aux;

    /*inicializacao do algoritmo:
    todos os vertices brancos, distancia inicial inicializada e pai como nulo*/

    int i;
    for (i = 0; i < ordem; i++)
    {
        G[i].corBuscaLargura = BRANCO;
        G[i].distanciaBuscaLargura = INT_MAX;
        G[i].paiBuscaLargura = ELEMENTO_NAO_DEFINIDO;
    }

    /*
    verticie inicial:
    cor cinza, distancia zero (por ser ele mesmo) 
    e sem pais na arvore de busca*/
    G[verticeInicial].corBuscaLargura = CINZA;
    G[verticeInicial].distanciaBuscaLargura = 0;
    G[verticeInicial].paiBuscaLargura = ELEMENTO_NAO_DEFINIDO;

    Q = inicializaFila(ordem); /*Fila vazia, para ser usar, no maximo, ordem vezes*/
    enfileira(Q, verticeInicial);

    while (!filaEstaVazia(Q))
    {
        int indiceVerticeAtual = desinfileira(Q);
        Vertice *u = &G[indiceVerticeAtual];

        /*Iterando sobre as arestas do vertice atual, e fazendo a busca por eles*/
        for (aux = u->prim; aux != NULL; aux = aux->prox)
        {
            Vertice *v = &G[aux->nome];
            if (v->corBuscaLargura == BRANCO)
            {
                v->corBuscaLargura = CINZA;
                v->distanciaBuscaLargura = u->distanciaBuscaLargura + 1;
                v->paiBuscaLargura = u->nome;

                enfileira(Q, v->nome);
            }
        }

        /*busca completamente realizada a partir do vertice atual
        pintando ele de preto para indicar isso*/
        u->corBuscaLargura = PRETO;
    }

    liberaFila(Q);
}

/**
 * A gente vai dizer que um grafo e conexo a partir da
 * realizacao da busca em profundida se todos os vertices
 * do grafo estiverem pintados de preto
 * 
 * O metodo atual conta a quantidade de vertices de cada cor,
 * imprime o resultado e retorna verdadeiro se, e apenas se, 
 * todos os vertices forem pretos
*/
bool eConexoBLargura(Vertice G[], int ordem)
{
    int brancos, pretos, cinzas, i;
    brancos = pretos = cinzas = 0;

    for (i = 0; i < ordem; i++)
    {
        if (G[i].corBuscaLargura == BRANCO)
            brancos++;
        else if (G[i].corBuscaLargura == PRETO)
            pretos++;
        else if (G[i].corBuscaLargura == CINZA)
            cinzas++;
    }

    /*Impressao de resultados*/
    printf("Pretos = %d, brancos = %d, cinzas = %d\n", pretos, brancos, cinzas);

    return pretos == ordem;
}

/**
 * Aqui, para conseguirmos definir a partir do presente
 * algoritmo se um grafo e conexo ou nao, precisamos 
 * passar para a funcao de visita o componente
*/
void buscaProfundida(Vertice G[], int ordem)
{
    int tempo;
    int i, j;

    /*Inicializacao de valores dos vertices para a realizacao do algoritmo*/
    for (i = 0; i < ordem; i++)
    {
        G[i].corBuscaProfundida = BRANCO;
        G[i].paiBuscaProfundida = ELEMENTO_NAO_DEFINIDO;
    }

    /*tempo: variavel informativa, para determinar em que momentos os vertices foram explorados*/
    tempo = 0;
    for (j = 0; j < ordem; j++)
    {
        if (G[j].corBuscaProfundida == BRANCO)
            /*Somente o indice 0 vai ser usado se o grafo for conexo*/
            buscaProfundidaVisita(G, ordem, j, &tempo, j);
    }
}

/**
 * Função recursiva, usada para navegar entre os vértices do grafo
 * Recebe como parametro um ponteiro de tempo, que vai ser usado para
 * documentar o momento em que o vertice foi 'descoberto' e 'finalizado'.
 * Recebe tambem a componente do vertice sendo visitado. Isso nos permite, 
 * a partir da funcao base de buscaProfundida, decidir se o grafo e conexo 
 * ou nao
*/
void buscaProfundidaVisita(Vertice G[], int ordem, int verticeAtual, int *tempo, int componente)
{
    Aresta *aux;
    Vertice *u;

    (*tempo)++;

    /*Comeco da visita de um vertice: documentar o tempo de inicio e pintar ele de cinza*/
    u = &G[verticeAtual];
    u->tempoDescobertaBuscaProf = *tempo;
    u->corBuscaProfundida = CINZA;
    u->componente = componente; /*usado na verificacao de conexidade*/

    /*Prestar uma visita a todos os vertices adjances do vertice atual.
    Considerar que o metodo atual e recursivo, e sendo esse o caso, a finalizacao
    se da uma vez que todos os vertices chamados abaixo ja tiverem passado pelo mesmo processo*/
    for (aux = u->prim; aux != NULL; aux = aux->prox)
    {

        Vertice *v = &G[aux->nome]; 
        if (v->corBuscaProfundida == BRANCO) /*Apenas visitando vertices brancos, evitando repeticoes*/
        {
            v->paiBuscaProfundida = u->nome; /*Criacao de arvore de busca*/
            buscaProfundidaVisita(G, ordem, v->nome, tempo, componente);
        }
    }

    /*Depois de iterar sobre todos os vertices adjancetes do 
    vertice atual, temos o tempo de finalizacao: pintamos o 
    vertice de preto, ele esta finalizado*/
    (*tempo)++;
    u->corBuscaProfundida = PRETO;
    u->tempoFinalizacaoBuscaProf = *tempo;
}

/**
 * Funcao de utilidade usada para definir se um grafo e conexo
 * ou nao a partir da buscaProfundida
 * 
 * A presente funcao conta quantos componentes estao definidos no 
 * grafo a partir do atributo 'componente' marcados nos vertices.
*/
int numComponentes(Vertice G[], int ordem)
{
    int *componentesDefinidos;
    int encontrados;
    int i, j;
    bool jaDefinido;

    componentesDefinidos = (int *)malloc(ordem * sizeof(int));
    encontrados = 0;

    for (i = 0; i < ordem; i++)
    {

        int componenteAtual = G[i].componente;
        if (componenteAtual == ELEMENTO_NAO_DEFINIDO)
        {
            /*Executar definirComponentesGrafo(G[], int)*/
            /*Componente não definido encontrado, nao incluso na contagem*/
            continue;
        }

        jaDefinido = false;

        for (j = 0; j < encontrados; j++)
        {
            if (componenteAtual == componentesDefinidos[j])
                jaDefinido = true; /*Componente ja contabilizado, nao incluindo*/
        }

        if (!jaDefinido)
            componentesDefinidos[encontrados++] = componenteAtual;
    }

    free(componentesDefinidos);
    return encontrados;
}

bool eConexoBProf(Vertice G[], int ordem)
{
    /*
    Definição de grafo conexo: se para todo o par de 
    vertices, existe um caminho entre eles, e temos 
    pelo menos um vértice. Ou seja, se temos uma, e somente,
    uma componente definida nele. 

    Importante: grafos vazios não são conexos.
    */

    return numComponentes(G, ordem) == 1;
}

/*
 * Programinha simples para testar a representacao de grafo
 */
int main(int argc, char *argv[])
{
    Vertice *G;
    int ordemG = 5;

    criaGrafo(&G, ordemG);
    acrescentaAresta(G, ordemG, 0, 1);
    acrescentaAresta(G, ordemG, 1, 2);
    acrescentaAresta(G, ordemG, 2, 3);

    buscaLargura(G, ordemG, 0);
    buscaProfundida(G, ordemG);
    
    bool eConexoLarg = eConexoBLargura(G, ordemG);
    bool eConexoProf = eConexoBProf(G, ordemG);
    
    imprimeGrafo(G, ordemG);

    if (eConexoLarg && eConexoProf) {
        printf("Grafo conexo\n");
        exit(EXIT_SUCCESS);
    }

    if (!eConexoLarg && !eConexoProf) {
        printf("Grafo não conexo\n");
        exit(EXIT_SUCCESS);
    }

    printf("Resultado de conexidade divergentes, algoritmos inadequados\n");
    exit(EXIT_FAILURE);
}
