/* 
 * REPRESENTACAO DE GRAFOS - Versao 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>

#define COMPONENTE_NAO_DEFINIDA 0
/*#include <stdbool.h>*/

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
} Vertice;

/*
 * Declaracao das funcoes para manipulacao de grafos 
 */
void imprimeGrafo(Vertice G[], int ordem);
void criaGrafo(Vertice **G, int ordem);
int acrescentaAresta(Vertice G[], int ordem, int v1, int v2);
int calculaTamanho(Vertice G[], int ordem);
void marcarComponente(Vertice *v, int componente, Vertice G[], int ordem);
void definirComponentesGrafos(Vertice G[], int ordem);
int numComponentes(Vertice G[], int ordem);
bool conexo(Vertice G[], int ordem);

/*
 * Implementacao das funcoes para manipulacao de grafos 
 */

/* Criacao de um grafo com ordem predefinida e, inicilamente, sem nenhuma aresta */
void criaGrafo(Vertice **G, int ordem)
{
    int i;
    *G = (Vertice *)malloc(sizeof(Vertice) * ordem);

    for (i = 0; i < ordem; i++)
    {
        (*G)[i].nome = i;
        (*G)[i].componente = COMPONENTE_NAO_DEFINIDA; /* 0: sem componente atribuida */
        (*G)[i].prim = NULL;                          /* Cada vertice sem nenua aresta incidente */
    }
}

/*  Acrescenta uma aresta em um grafo previamente criado.  */
/*  Devem ser passados os extremos v1 e v2 da aresta a ser acrescentada  */
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
        Aresta *aux;
        aux = G[i].prim;
        for (j = 0; aux != NULL; aux = aux->prox, j++)
            ; /*contando as iterações*/
        totalArestas += j;
    }
    return totalArestas / 2 + ordem;
}

/*  Imprime um grafo exibindo seus vertices e as arestas incidentes neles */
void imprimeGrafo(Vertice G[], int ordem)
{
    int i;
    Aresta *aux;

    printf("Ordem:       %d\n", ordem);
    printf("Tamanho:     %d\n", calculaTamanho(G, ordem));
    printf("Componentes: %d\n", numComponentes(G, ordem));
    printf("Conexo:      %s\n", conexo(G, ordem) ? "sim" : "nao");
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

/*
Marca o vertice com o número de seu componente, 
bem como todos os vertices a ele associados
*/
void marcarComponente(Vertice *v, int componente, Vertice G[], int ordem)
{
    Aresta *aux;
    Vertice *outroLado;

    if (v->componente != COMPONENTE_NAO_DEFINIDA)
    {
        /*Componente já marcado, nada a ser feito*/
        return;
    }

    v->componente = componente;
    aux = v->prim;
    for (; aux != NULL; aux = aux->prox)
    {
        outroLado = &G[aux->nome];
        marcarComponente(outroLado, componente, G, ordem);
    }
}

void definirComponentesGrafos(Vertice G[], int ordem)
{
    int componenteAtual;
    int i;

    componenteAtual = 1;

    for (i = 0; i < ordem; i++)
    {
        if (G[i].componente == COMPONENTE_NAO_DEFINIDA)
        {
            marcarComponente(&G[i], componenteAtual, G, ordem);
            componenteAtual++;
        }
    }
}

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
        if (componenteAtual == COMPONENTE_NAO_DEFINIDA)
        {
            /*Executar definirComponentesGrafos(G[], int)*/
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

bool conexo(Vertice G[], int ordem)
{
    /*
    Definição de grafo conexo: se para todo o par de 
    vertices de um grafo, existe um caminho entre eles, 
    e temos pelo menos um vértice.

    Logo, grafos vazios não são conexos.
    */

    return numComponentes(G, ordem) == 1;
}

/*
Dois vértices, não relacionados por uma aresta
+-+   +-+
|0|   |1|
+-+   +-+
*/
void testeVerticesIsolados ()
{
    Vertice *G;
    int ordem;

    ordem = 2;
    criaGrafo(&G, ordem);
    definirComponentesGrafos(G, ordem);

    imprimeGrafo(G, ordem);
}

/*
+-+  +-+
|0+->+1|
+-+  +++
      |
+-+   |
|2+<--+
+-+
*/
void testeVerticesConexos ()
{
    Vertice *G;
    int ordem;

    ordem = 3;

    criaGrafo(&G, ordem);
    acrescentaAresta(G, ordem, 0, 1);
    acrescentaAresta(G, ordem, 1, 2);

    definirComponentesGrafos(G, ordem);

    imprimeGrafo(G, ordem);
    
}

/*
Nenhum vértice, nenhuma aresta
*/
void testeGrafoVazio ()
{
    Vertice *G;
    int ordem;

    ordem = 0;

    criaGrafo(&G, ordem);
    definirComponentesGrafos(G, ordem);

    imprimeGrafo(G, ordem);
}

int main(int argc, char *argv[])
{
    testeVerticesIsolados();
    testeVerticesConexos();
    testeGrafoVazio();

    return EXIT_SUCCESS;
}
