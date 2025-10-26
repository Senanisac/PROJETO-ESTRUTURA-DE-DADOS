/**
 * @file huffman.h
 * @brief Funções para a compactação e descompactação Huffman.
 */

#include "structs.h"

/**
 * @brief Retorna o tamanho em bytes de um arquivo.
 * 
 * @param arquivo_entrada Ponteiro para o arquivo aberto.
 * @return unsigned long Tamanho do arquivo em bytes ou -1 se inválido.
 */
unsigned long tamanho_arquivo(FILE *arquivo_entrada){
    if(!arquivo_entrada) return -1; 

    fseek(arquivo_entrada, 0, SEEK_END);   
    unsigned long tam_arq = ftell(arquivo_entrada);
    rewind(arquivo_entrada);
    
    return tam_arq;
}

/**
 * @brief Conta a frequência de cada byte no arquivo.
 * 
 * @param arquivo_entrada Ponteiro para o arquivo.
 * @param tam_arq Tamanho do arquivo.
 * @return unsigned long* Vetor de frequência com 256 posições.
 */
unsigned long *atribuir_frequencia(FILE *arquivo_entrada, unsigned long tam_arq){
    unsigned long *frequencia = calloc(TAM_ASCII, sizeof(unsigned long));
    unsigned char byte;

    for(int i = 0; i < tam_arq; i++){
        fread(&byte, sizeof(unsigned char), 1, arquivo_entrada);
        frequencia[byte]++; 
    }

    rewind(arquivo_entrada);
    return frequencia;
}

/**
 * @brief Adiciona o nó de forma ordenada.
 * 
 * @param fila Ponteiro para a fila.
 * @param novo Ponteiro para o no a ser ordenado.
 */
void inserir_ordenado(LISTA *fila, NOHUFF *novo){
    if(fila->inicio == NULL || novo->frequencia <= fila->inicio->frequencia){
        novo->prox = fila->inicio;
        fila->inicio = novo;
    }else{
        NOHUFF *aux = fila->inicio;
        while(aux->prox && aux->prox->frequencia < novo->frequencia)
            aux = aux->prox;
        novo->prox = aux->prox;
        aux->prox = novo;
    }
    fila->tamanho++;
}

/**
 * @brief Cria um novo no para a lista encadeada ordenada.
 * 
 * @param frequencia Array com as frequencias.
 * @param lista Ponteiro para a lista encadeada ordenada.
 */
void preencher_fila(unsigned long *frequencia, LISTA *lista){
    NOHUFF *novo;
    for(int i = 0; i < TAM_ASCII; i++){
        if(frequencia[i] > 0){
            novo = malloc(sizeof(NOHUFF));

            novo->caracter = malloc(sizeof(unsigned char));
            *(unsigned char*) novo->caracter = i;
            novo->frequencia = frequencia[i];
            novo->direita = NULL;
            novo->esquerda = NULL;
            novo->prox = NULL;

            inserir_ordenado(lista, novo);
        }
    }
}

/**
 * @brief Remove o caracter com menor frequencia, da lista, que se encontra no inicio, pois ela está ordenada do menor para o maior
 * 
 * @param fila Ponteiro para o início da fila.
 * @return O no removido.
 */
NOHUFF *remove_no_inicio(LISTA *fila){
    NOHUFF *aux = NULL;

    if (fila->inicio){
        aux = fila->inicio;
        fila->inicio = aux->prox;
        aux->prox = NULL;
        fila->tamanho--;
    }

    return aux;
}

/**
 * @brief Monta a arvore de huffman, juntando os dois ultimos nos.
 * 
 * @param fila Ponteiro para a fila.
 * @return retorna a raiz da arvore.
 */
NOHUFF *montar_arvore(LISTA *fila){
    NOHUFF *primeiro, *segundo, *novo;
    while(fila->tamanho > 1){
        primeiro = remove_no_inicio(fila);
        segundo = remove_no_inicio(fila);

        novo = malloc(sizeof(NOHUFF));
        novo->caracter = malloc(sizeof(unsigned char));
        *(unsigned char*)novo->caracter = '*';
        novo->frequencia = primeiro->frequencia + segundo->frequencia;
        novo->esquerda = primeiro;
        novo->direita = segundo;
        novo->prox = NULL;

        inserir_ordenado(fila, novo);
    }

    return fila->inicio;
}
/**
 * @brief Calcula a altura da árvore de Huffman.
 * 
 * @param raiz Ponteiro para a raiz da árvore.
 * @return unsigned int Altura da árvore.
 */
unsigned int altura_arvore(NOHUFF *raiz){
    if(!raiz) return -1;

    int esq = altura_arvore(raiz->esquerda) + 1;
    int dir = altura_arvore(raiz->direita) + 1;

    return (esq > dir) ? esq : dir;
}

/**
 * @brief Gera o dicionário de códigos binários para cada caractere.
 * 
 * @param dicionario Vetor de strings com os códigos.
 * @param raiz Ponteiro para a árvore.
 * @param caminho Caminho atual (string de 0s e 1s).
 * @param tam_max Tamanho máximo das strings do caminho.
 */
void gerar_dicionario(unsigned char **dicionario, NOHUFF *raiz, char *caminho, unsigned int altura_max_arvore){
    char esquerda[altura_max_arvore], direita[altura_max_arvore];

    if(!raiz->esquerda && !raiz->direita)
        strcpy(dicionario[*(unsigned char*)raiz->caracter], caminho);
    else{
        strcpy(esquerda, caminho);
        strcpy(direita, caminho);

        strcat(esquerda, "0");
        strcat(direita, "1");

        gerar_dicionario(dicionario, raiz->esquerda, esquerda, altura_max_arvore);
        gerar_dicionario(dicionario, raiz->direita, direita, altura_max_arvore);
    }
}

/**
 * @brief Salva a árvore de Huffman serializada em pré-ordem no arquivo.
 * 
 * @param raiz Ponteiro para a árvore.
 * @param arquivo_saida Arquivo de saída.
 * @return short Quantidade de caracteres salvos.
 */
short salvar_arvore(NOHUFF *raiz, FILE *arquivo_saida){
    if(!raiz) return 0;
    
    int folha_escape = (*(unsigned char*)raiz->caracter == '*' || *(unsigned char*)raiz->caracter == '\\') && !raiz->esquerda && !raiz->direita;

    if(folha_escape)
        fwrite("\\", sizeof(unsigned char), 1, arquivo_saida);
    
    fwrite(&*(unsigned char*)raiz->caracter, sizeof(unsigned char), 1, arquivo_saida);
    int esquerda = salvar_arvore(raiz->esquerda, arquivo_saida);
    int direita = salvar_arvore(raiz->direita, arquivo_saida);
    
    return 1 + esquerda + direita + folha_escape;
}

/**
 * @brief Salva os dados comprimidos no arquivo.
 * 
 * @param arquivo_entrada Ponteiro para o arquivo original.
 * @param arquivo_saida Ponteiro para o arquivo de saída.
 * @param dicionario Vetor de dicionário de Huffman.
 * @param tam_arq Tamanho do arquivo original.
 * @param tam_arvore Tamanho da arvore binaria.
 * @return short tamanho do lixo obtido no ultimo byte.
 */
short salvar_dados(FILE *arquivo_entrada, FILE *arquivo_saida, unsigned char **dicionario, int tam_arquivo, int tam_arvore){
    int bit_atual = 0;
    int tamanho_lixo = 0;
    unsigned char escrever_buffer = 0;
    unsigned char ler_buffer;

    fseek(arquivo_saida, 0, SEEK_END);

    for (int i = 0; i < tam_arquivo; i++){
        fread(&ler_buffer, sizeof(unsigned char), 1, arquivo_entrada);
        unsigned char *codigo = dicionario[ler_buffer];

        for(int j = 0; codigo[j] != '\0'; j++){
            escrever_buffer <<= 1;
            if(codigo[j] == '1')
                escrever_buffer |= 1;
            
            bit_atual++;

            if(bit_atual == 8){
                fwrite(&escrever_buffer, sizeof(unsigned char), 1, arquivo_saida);
                escrever_buffer = 0;
                bit_atual = 0;
            }
        }
    }

    if(bit_atual > 0){
        escrever_buffer <<= 8 - bit_atual;
        fwrite(&escrever_buffer, sizeof(unsigned char), 1, arquivo_saida);
        tamanho_lixo = 8 - bit_atual;
    }

    fclose(arquivo_entrada);
    rewind(arquivo_saida);
    return tamanho_lixo;
}

/**
 * @brief Substitui os dois caracteres iniciais por 3 bits para o lixo e 13 bits para o tamanho da arvore resultando em .
 *  bytes
 * @param arquivo_saida Nome do arquivo de saida.
 * @param tam_lixo Tamanho do lixo no final do arquivo.
 * @param tam_arvore Tamanho da arvore binaria.
 */
void salvar_cabecalho(FILE *arquivo_saida, unsigned short tam_lixo, unsigned short tam_arvore){
    fseek(arquivo_saida, 0, SEEK_SET);
    tam_lixo <<= 13;
    short cabecalho = tam_lixo | tam_arvore;
    unsigned char buffer = cabecalho >> 8;
    fwrite(&buffer, sizeof(unsigned char), 1, arquivo_saida);
    buffer = cabecalho;
    fwrite(&buffer, sizeof(unsigned char), 1, arquivo_saida);
}

/**
 * @brief Le o cabecalho do arquivo compactado para descobrir o tamanho do lixo e arvore.
 * 
 * @param arquivo_entrada Nome do arquivo de entrada.
 * @param tam_lixo Ponteiro para guardar o tamanho do lixo no final do arquivo.
 * @param tam_arvore Ponteiro para guardar o tamanho da arvore binaria.
 */
void ler_cabecalho(FILE *arquivo_entrada, unsigned short *tam_lixo, unsigned short *tam_arvore){
    unsigned char buffer;
    fread(&buffer, sizeof(char), 1, arquivo_entrada);

    unsigned short cabecalho = buffer << 8;

    fread(&buffer, sizeof(char), 1, arquivo_entrada);

    cabecalho |= buffer;

    *tam_lixo = cabecalho >> 13;
    *tam_arvore = cabecalho & 0x1FFF;

    printf("\n\tTamanho Lixo: %d\n", *tam_lixo);
    printf("\n\tTamanho Arvore: %d\n", *tam_arvore);
}

/**
 * @brief Funcao usada para criar um novo no para arvore.
 * 
 * @param caractere Caracter a ser adicionado na arvore.
 * @param esquerda Ponteiro para o no esquerdo.
 * @param direita Ponteiro para o no direito.
 * @return ponteiro para NOHUFF do novo no.
 */
NOHUFF* criar_arvore(unsigned char caractere, NOHUFF *esquerda, NOHUFF *direita){
    NOHUFF *novo = malloc(sizeof(NOHUFF));
    novo->caracter = malloc(sizeof(unsigned char));
    *(unsigned char*) novo->caracter = caractere;
    novo->esquerda = esquerda;
    novo->direita = direita;

    return novo;
}

/**
 * @brief Funcao usada para remontar arvore.
 * 
 * @param arquivo_entrada Nome do arquivo entrada.
 * @param tam_arvore Tamanho da arvore.
 * @return ponteiro NOHUFF de forma recursiva retorna a raiz da arvore.
 */
NOHUFF *remontar_arvore(FILE *arquivo_entrada, unsigned short *tam_arvore){
    unsigned char buffer;
    fread(&buffer, sizeof(unsigned char), 1, arquivo_entrada);

    int e_folha = 0;
    if(*tam_arvore == 0)
        return NULL;
    (*tam_arvore)--;
    if(buffer == '\\'){
        (*tam_arvore)--;
        fread(&buffer, sizeof(unsigned char), 1, arquivo_entrada);
        e_folha = 1;
    }
    if(buffer != '*'){
        e_folha = 1;
    }

    if(e_folha){
        return criar_arvore(buffer, NULL, NULL);
    }
    NOHUFF *esquerda = remontar_arvore(arquivo_entrada, tam_arvore);
    NOHUFF *direita = remontar_arvore(arquivo_entrada, tam_arvore);
    return criar_arvore('*', esquerda, direita);
}

/**
 * @brief Funcao principal para decodificar o arquivo.
 * 
 * @param arquivo_entrada Nome do arquivo entrada.
 * @param arquivo_saida Nome do arquivo saida.
 * @param tam_arquivo Tamanho do arquivo compactado.
 * @param tam_lixo Tamanho do lixo no ultimo byte do arquivo.
 * @param tam_arvore Tamanho da arvore.
 */
void decodificar(FILE *arquivo_entrada, FILE *arquivo_saida, unsigned long tam_arquivo, unsigned short tam_lixo, unsigned short tam_arvore){
    tam_arquivo -= tam_arvore + 2;
    tam_arquivo <<= 3;
    tam_arquivo -= tam_lixo;

    NOHUFF *raiz = remontar_arvore(arquivo_entrada, &tam_arvore);
    NOHUFF *aux = raiz;
    unsigned char buffer;
    int bit_atual = 0;

    while(tam_arquivo--){
        if(bit_atual == 0){
            fread(&buffer, sizeof(unsigned char), 1, arquivo_entrada);
            bit_atual = 8;
        }

        if(buffer & (1 << --bit_atual)){
            aux = aux->direita;
        }else{
            aux = aux->esquerda;
        }

        if(aux->esquerda == NULL && aux->direita == NULL){
            fwrite(&*(unsigned char*)aux->caracter, sizeof(unsigned char), 1, arquivo_saida);
            aux = raiz;
        }
    }
}

/**
 * @brief Libera toda a memória alocada para a árvore de Huffman
 * 
 * @param raiz Ponteiro para a raiz da árvore
 */
void liberar_arvore(NOHUFF* raiz) {
    if (raiz == NULL) return;
    liberar_arvore(raiz->esquerda);
    liberar_arvore(raiz->direita);
    free(raiz);
}

/**
 * @brief Libera toda a memória alocada para o dicionário
 * 
 * @param dicionario Ponteiro para o dicionário
 */
void liberar_dicionario(unsigned char **dicionario) {
    if (!dicionario) return;
    
    for (int i = 0; i < TAM_ASCII; i++) {
        if (dicionario[i]) free(dicionario[i]);
    }
    
    free(dicionario);
}
