#include "bibliotecas/huffman.h"

/**
 * @brief Compacta um arquivo usando o algoritmo de Huffman.
 * 
 * @param caminho Caminho do arquivo a ser compactado.
 * @param nome_arquivo Nome para o arquivo compactado de saída.
 */
void compactar(char *caminho, char *nome_arquivo){
    FILE *arquivo_entrada = fopen(caminho, "rb");
    if(!arquivo_entrada){
        printf("\n\tERRO AO ABRIR ARQUIVO ENTRADA.\n");
        return;
    }

    unsigned long tam_arq = tamanho_arquivo(arquivo_entrada);
    printf("\n\tTAMANHO DO ARQUIVO ORIGINAL: %ld bytes\n", tam_arq);

    unsigned long *frequencia = atribuir_frequencia(arquivo_entrada, tam_arq);

    LISTA fila;
    fila.inicio = NULL;
    fila.tamanho = 0;

    preencher_fila(frequencia, &fila);

    NOHUFF *arvore = montar_arvore(&fila);

    unsigned int altura_max_arvore = altura_arvore(arvore) + 1;
    unsigned char **dicionario = malloc(sizeof(unsigned char*) * TAM_ASCII);

    for(int i = 0; i < TAM_ASCII; i++)
        dicionario[i] = calloc(altura_max_arvore, sizeof(unsigned char));

    gerar_dicionario(dicionario, arvore, "", altura_max_arvore);
    
    FILE *arquivo_saida = fopen(nome_arquivo, "wb");
    if(!arquivo_saida){
        printf("\n\tERRO AO CRIAR ARQUIVO SAIDA");
        return;
    }

    fwrite("AB", sizeof(unsigned char), 2, arquivo_saida);

    short tam_arvore = salvar_arvore(arvore, arquivo_saida);
    printf("\n\tTAMANHO ARVORE: %d", tam_arvore);

    short tam_lixo = salvar_dados(arquivo_entrada, arquivo_saida, dicionario, tam_arq, tam_arvore);
    printf("\n\tTAMANHO LIXO: %d", tam_lixo);

    salvar_cabecalho(arquivo_saida, tam_lixo, tam_arvore);

    fclose(arquivo_saida);
    liberar_arvore(arvore);
    liberar_dicionario(dicionario);
    free(frequencia);
}

/**
 * @brief Descompacta um arquivo compactado usando Huffman.
 * 
 * @param caminho Caminho do arquivo compactado.
 * @param nome_arquivo Nome para o arquivo descompactado.
 */
void descompactar(char *caminho, char *nome_arquivo){
    FILE *arquivo_entrada = fopen(caminho, "rb");
    if(!arquivo_entrada){
        printf("\n\tERRO AO ABRIR ARQUIVO ENTRADA.\n");
        return;
    }

    unsigned long tam_arq = tamanho_arquivo(arquivo_entrada);

    unsigned short tam_lixo;
    unsigned short tam_arvore;

    ler_cabecalho(arquivo_entrada, &tam_lixo, &tam_arvore);

    FILE *arquivo_saida = fopen(nome_arquivo, "wb");
    if(!arquivo_saida){
        printf("\n\tERRO AO CRIAR ARQUIVO SAIDA.\n");
        return;
    }

    decodificar(arquivo_entrada, arquivo_saida, tam_arq, tam_lixo, tam_arvore);
}

/**
 * @brief Função principal do programa, que apresenta o menu e chama os métodos de compactação e descompactação.
 * 
 * @return int 0 para sucesso.
 */
int main(){
    int opcao;

    SetConsoleOutputCP(65001); //previne erros em alguns caracteres

    printf("\n\t=== COMPRESSOR HUFFMAN ===\n");
    printf("\n\tDigite uma opcao:");
    printf("\n\t1 - Compactar\n\t2 - Descompactar\n\t0 - Sair\n\n\tescolha: ");
    scanf("%d", &opcao);
    getchar(); // Remove o '\n' do texto

    switch (opcao){
    case 1:{
        printf("\n\tDIGITE O ENDERECO DO ARQUIVO QUE DESEJA ABRIR: ");

        char endereco[MAX_LEITURA];

        if(!fgets(endereco, MAX_LEITURA, stdin)){ 
            printf("\n\tERRO: FALHA AO LER A ENTRADA.\n");
            break;
        }
        if (strlen(endereco) == 0) {
            printf("\n\tERRO: CAMINHO VAZIO.\n");
            break;
        }

        char nome_arquivo[MAX_LEITURA];

        printf("\n\tDIGITE UM NOME PARA O ARQUIVO COMPACTADO: ");
        if(!fgets(nome_arquivo, MAX_LEITURA, stdin)){
            printf("\n\tERRO: FALHA AO LER A NOME DO ARQUIVO.\n");
            break;
        }
        if (strlen(nome_arquivo) == 0) {
            printf("\n\tERRO: NOME DO ARQUIVO VAZIO.\n");
            break;
        }

        endereco[strcspn(endereco, "\n")] = '\0';
        nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0'; 
        strcat(nome_arquivo, ".huff");

        compactar(endereco, nome_arquivo);

        break;
    }
    case 2:{
        printf("\n\tDIGITE O CAMINHO COMPLETO DO ARQUIVO QUE DESEJA ABRIR: ");

        char caminho[MAX_LEITURA];

        if(!fgets(caminho, MAX_LEITURA, stdin)){
            printf("\n\tERRO: FALHA AO LER A ENTRADA.huff.\n");
            break;
        }
        if (strlen(caminho) == 0) {
            printf("\n\tERRO: CAMINHO VAZIO.\n");
            break;
        }

        char nome_arquivo[MAX_LEITURA];

        printf("\n\tDIGITE O NOME PARA O ARQUIVO DESCOMPACTADO (COM SUA EXTENÇÃO): ");
        if(!fgets(nome_arquivo, MAX_LEITURA, stdin)){
            printf("\n\tERRO: FALHA AO LER A NOME DO ARQUIVO.\n");
            break;
        }
        if (strlen(nome_arquivo) == 0) {
            printf("\n\tERRO: NOME DO ARQUIVO VAZIO.\n");
            break;
        }

        caminho[strcspn(caminho, "\n")] = '\0';
        nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0'; 

        descompactar(caminho, nome_arquivo);

        break;
    }
    
    default:
        printf("\n\tOPÇÃO INVÁLIDA!");
        break;
    }

    printf("\n\tSaindo...");

    return 0;
}
