#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10000

// ======================== FILA DE PRIORIDADE SEM HEAP (LISTA ORDENADA) ==========================

typedef struct no{
    int valor;
    struct no *prox;
}NO;

typedef struct{
    NO *inicio;
    int comparacoes;
} FILA;

FILA* criarFilaLista() {
    FILA *fila = malloc(sizeof(FILA));
    fila->inicio = NULL;
    fila->comparacoes = 0;
    return fila;
}

void inserirLista(FILA *fila, int valor){
    NO *novo = malloc(sizeof(NO));
    novo->valor = valor;
    novo->prox = NULL;

    NO **p = &fila->inicio;
    while (*p && (*p)->valor > valor){
        fila->comparacoes++;
        p = &(*p)->prox;
    }

    fila->comparacoes++;
    novo->prox = *p;
    *p = novo;
}

int removerLista(FILA *fila) {
    if (!fila->inicio) return -1;
    int val = fila->inicio->valor;
    NO *aux = fila->inicio;
    fila->inicio = fila->inicio->prox;
    free(aux);
    return val;
}

// ======================== FILA DE PRIORIDADE COM HEAP ==========================
typedef struct{
    int *dados;
    int tamanho;
    int comparacoes;
}HEAP;

HEAP *criarHEAP(int capacidade){
    HEAP *heap = malloc(sizeof(HEAP));
    heap->dados = malloc(sizeof(int) * capacidade);
    heap->tamanho = 0;
    heap->comparacoes = 0;
    return heap;
}

void trocar(int *a, int *b){
    int aux = *a;
    *a = *b;
    *b = aux;
}

void subir(HEAP *heap, int indice){
    while(indice > 0){
        int pai = (indice - 1) / 2;
        heap->comparacoes++;
        if(heap->dados[indice] <= heap->dados[pai]) break;
        trocar(&heap->dados[indice], &heap->dados[pai]);
        indice = pai;
    }
}

void descer(HEAP* heap, int indice) {
    while (2 * indice + 1 < heap->tamanho) {
        int filho = 2 * indice + 1;
        if (filho + 1 < heap->tamanho && heap->dados[filho + 1] > heap->dados[filho]) {
            filho++;
        }
        heap->comparacoes++;
        if (heap->dados[indice] >= heap->dados[filho]) break;
        trocar(&heap->dados[indice], &heap->dados[filho]);
        indice = filho;
    }
}

void inserirHeap(HEAP* heap, int valor) {
    heap->dados[heap->tamanho++] = valor;
    subir(heap, heap->tamanho - 1);
}

int removerHeap(HEAP* h) {
    if (h->tamanho == 0) return -1;
    int raiz = h->dados[0];
    h->dados[0] = h->dados[--h->tamanho];
    descer(h, 0);
    return raiz;
}

// ======================== MAIN ==========================

int main() {
    srand(time(NULL));
    FILA* filaLista = criarFilaLista();
    HEAP* heap = criarHEAP(MAX);

    FILE* f_insercao = fopen("insercao.csv", "w");

    if (!f_insercao) {
        printf("Erro ao abrir os arquivos!\n");
        return 1;
    }

    fprintf(f_insercao, "Tamanho,SemHeap,ComHeap\n");

    // Inserir 500 elementos
    for (int i = 0; i < 50; i++) {
        int val = rand() % 10000;
        inserirLista(filaLista, val);
        inserirHeap(heap, val);
        fprintf(f_insercao, "%d,%d,%d\n", i, filaLista->comparacoes, heap->comparacoes);
    }

    fclose(f_insercao);

    printf("Arquivo insercao.csv gerado!\n");
    return 0;
}
