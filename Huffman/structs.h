/**
 * @file structs.h
 * @brief Definições de estruturas e constantes utilizadas no algoritmo de compressão Huffman.
 */

#include <stdio.h>      
#include <stdlib.h>    
#include <windows.h>    /**< Para suporte a acentuação no Windows (SetConsoleOutputCP) */
#include <string.h> 

/** 
 * @def MAX_LEITURA
 * @brief Tamanho máximo para leitura de dados via entrada padrão.
 */
#define MAX_LEITURA 100

/** 
 * @def TAM_ASCII
 * @brief Tamanho total da tabela ASCII padrão.
 */
#define TAM_ASCII 256

/**
 * @struct NOHUFF
 * @brief Estrutura que representa um nó da árvore de Huffman.
 *
 * Cada nó armazena:
 * - Um ponteiro genérico para o caractere ou valor.
 * - A frequência de ocorrência desse caractere.
 * - Ponteiros para o próximo nó (caso em lista).
 * - Ponteiros para os nós esquerdo e direito na árvore de Huffman.
 */
typedef struct nohuff {
    void *caracter;                       
    int frequencia;                       
    struct nohuff *prox;               
    struct nohuff *esquerda, *direita;
} NOHUFF;

/**
 * @struct HEAP
 * @brief Estrutura que representa uma heap máxima para montagem da árvore de Huffman.
 *
 * A heap é implementada como um vetor de ponteiros para nós de Huffman, controlado pelos campos:
 * - tamanho: quantidade atual de elementos.
 * - capacidade: capacidade máxima permitida.
 */
typedef struct{
    NOHUFF *inicio; 
    int tamanho;
}LISTA;
