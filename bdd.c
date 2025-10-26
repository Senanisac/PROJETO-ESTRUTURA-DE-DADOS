#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constantes para limites do sistema
#define MAX_VARS 100     // Número máximo de variáveis booleanas
#define MAX_NOS 10000    // Número máximo de nós no BDD

// Estrutura de um nó do BDD (Binary Decision Diagram)
typedef struct NoBDD {
    int var_idx;        // Índice da variável booleana (0, 1, 2, ...)
    struct NoBDD *sim;  // Ponteiro para o nó quando variável = 1 (ramo THEN)
    struct NoBDD *nao;  // Ponteiro para o nó quando variável = 0 (ramo ELSE)
    int id;             // ID único para identificar o nó
} NoBDD;


// Estrutura principal do gerenciador BDD
typedef struct {
    NoBDD *nos[MAX_NOS];        // Array de todos os nós criados
    int cont_nos;               // Contador de nós criados
    char nomes_vars[MAX_VARS][20]; // Nomes das variáveis booleanas
    int cont_vars;              // Contador de variáveis criadas
    NoBDD *zero;                // Nó constante FALSO (0)
    NoBDD *um;                  // Nó constante VERDADEIRO (1)
} GerenciadorBDD;


// ==================== FUNÇÕES BÁSICAS DO BDD ====================

/**
 * Inicializa o gerenciador BDD
 * Retorna: Ponteiro para o gerenciador alocado
 */
GerenciadorBDD* bdd_iniciar() 
{
    // Aloca memória para o gerenciador
    GerenciadorBDD *ger = (GerenciadorBDD*)malloc(sizeof(GerenciadorBDD));
    ger->cont_nos = 0;
    ger->cont_vars = 0;
    
    // Cria os nós constantes (terminal nodes)
    ger->zero = (NoBDD*)malloc(sizeof(NoBDD));
    ger->um = (NoBDD*)malloc(sizeof(NoBDD));
    
    // Configura nó FALSO (0)
    ger->zero->var_idx = -1;    // -1 indica nó constante
    ger->zero->sim = ger->zero->nao = NULL;
    ger->zero->id = 0;
    
    // Configura nó VERDADEIRO (1)
    ger->um->var_idx = -1;
    ger->um->sim = ger->um->nao = NULL;
    ger->um->id = 1;
    
    // Adiciona os nós constantes ao array de nós
    ger->nos[ger->cont_nos++] = ger->zero;
    ger->nos[ger->cont_nos++] = ger->um;
    
    return ger;
}

/**
 * Cria uma nova variável booleana no BDD
 * ger: Gerenciador BDD
 * nome: Nome da variável (ex: "A", "B", "C")
 * Retorna: Índice da variável criada
 */
int bdd_nova_var(GerenciadorBDD *ger, const char *nome) 
{
    if (ger->cont_vars >= MAX_VARS) return -1;
    strcpy(ger->nomes_vars[ger->cont_vars], nome);
    return ger->cont_vars++;
}


/**
 * Encontra um nó existente ou cria um novo nó
 * Este é o coração do BDD - garante canonicidade
 */
NoBDD* bdd_encontrar_ou_criar_no(GerenciadorBDD *ger, int var_idx, 
                                NoBDD *no_sim, NoBDD *no_nao) {
    // Regra de redução: se ramos são iguais, retorna um deles
    if (no_sim == no_nao) return no_sim;
    
    // Procura por nó existente com mesma estrutura (canonicidade)
    for (int i = 0; i < ger->cont_nos; i++) 
    {
        NoBDD *no = ger->nos[i];
        if (no->var_idx == var_idx && 
            no->sim == no_sim && 
            no->nao == no_nao) {
            return no;  // Retorna nó existente
        }
    }
    
    // Verifica limite de nós
    if (ger->cont_nos >= MAX_NOS) return NULL;
    
    // Cria novo nó
    NoBDD *novo_no = (NoBDD*)malloc(sizeof(NoBDD));
    novo_no->var_idx = var_idx;
    novo_no->sim = no_sim;
    novo_no->nao = no_nao;
    novo_no->id = ger->cont_nos;
    
    // Adiciona ao array de nós
    ger->nos[ger->cont_nos++] = novo_no;
    return novo_no;
}

/**
 * Cria um nó que representa uma variável booleana
 * Para uma variável A: se A=1 retorna 1, se A=0 retorna 0
 */
NoBDD* bdd_variavel(GerenciadorBDD *ger, int var_idx) 
{
    return bdd_encontrar_ou_criar_no(ger, var_idx, ger->um, ger->zero);
}


// ==================== OPERAÇÕES BOOLEANAS ====================

/**
 * Operação AND (E) entre dois BDDs
 * Implementa: f AND g
 */
NoBDD* bdd_e(GerenciadorBDD *ger, NoBDD *f, NoBDD *g) 
{
    // Casos base para otimização
    if (f == ger->um && g == ger->um) return ger->um;   // 1 AND 1 = 1
    if (f == ger->zero || g == ger->zero) return ger->zero; // 0 AND x = 0
    if (f == g) return f;                               // f AND f = f
    if (f == ger->um) return g;                         // 1 AND g = g
    if (g == ger->um) return f;                         // f AND 1 = f
    
    // Escolhe a variável com menor índice (ordenação fixa)
    int var_idx;
    if (f->var_idx < g->var_idx) 
    {
        var_idx = f->var_idx;
    } 
    else 
    {
        var_idx = g->var_idx;
    }
    
    // Calcula os cofatores (Shannon expansion)
    NoBDD *f_sim = (f->var_idx == var_idx) ? f->sim : f;
    NoBDD *f_nao = (f->var_idx == var_idx) ? f->nao : f;
    NoBDD *g_sim = (g->var_idx == var_idx) ? g->sim : g;
    NoBDD *g_nao = (g->var_idx == var_idx) ? g->nao : g;
    
    // Chamada recursiva para os cofatores
    NoBDD *no_sim = bdd_e(ger, f_sim, g_sim);  // Quando var = 1
    NoBDD *no_nao = bdd_e(ger, f_nao, g_nao);  // Quando var = 0
    
    // Cria/recupera o nó resultante
    return bdd_encontrar_ou_criar_no(ger, var_idx, no_sim, no_nao);
}

/**
 * Operação OR (OU) entre dois BDDs
 * Implementa: f OR g
 */
NoBDD* bdd_ou(GerenciadorBDD *ger, NoBDD *f, NoBDD *g) 
{
    // Casos base para otimização
    if (f == ger->um || g == ger->um) return ger->um;   // 1 OR x = 1
    if (f == ger->zero && g == ger->zero) return ger->zero; // 0 OR 0 = 0
    if (f == g) return f;                               // f OR f = f
    if (f == ger->zero) return g;                       // 0 OR g = g
    if (g == ger->zero) return f;                       // f OR 0 = f
    
    // Escolhe a variável com menor índice
    int var_idx;
    if (f->var_idx < g->var_idx) 
    {
        var_idx = f->var_idx;
    } 
    else 
    {
        var_idx = g->var_idx;
    }
    
    // Calcula os cofatores
    NoBDD *f_sim = (f->var_idx == var_idx) ? f->sim : f;
    NoBDD *f_nao = (f->var_idx == var_idx) ? f->nao : f;
    NoBDD *g_sim = (g->var_idx == var_idx) ? g->sim : g;
    NoBDD *g_nao = (g->var_idx == var_idx) ? g->nao : g;
    
    // Chamada recursiva
    NoBDD *no_sim = bdd_ou(ger, f_sim, g_sim);  // Quando var = 1
    NoBDD *no_nao = bdd_ou(ger, f_nao, g_nao);  // Quando var = 0
    
    return bdd_encontrar_ou_criar_no(ger, var_idx, no_sim, no_nao);
}

// ==================== VERIFICAÇÃO DE EQUIVALÊNCIA ====================

/**
 * Verifica se dois BDDs representam a mesma função booleana
 * A canonicidade do BDD garante que funções equivalentes
 * terão exatamente a mesma estrutura
 */
int bdd_sao_equivalentes(NoBDD *f, NoBDD *g) 
{
    // Se são o mesmo nó (mesmo ponteiro), são equivalentes
    if (f == g) return 1;
    
    // Se um é constante e outro não, não são equivalentes
    if ((f->var_idx == -1) != (g->var_idx == -1)) return 0;
    
    // Se ambos são constantes, compara seus valores
    if (f->var_idx == -1 && g->var_idx == -1) {
        return f->id == g->id;  // Compara 0/1
    }
    
    // Se usam variáveis diferentes, não são equivalentes
    if (f->var_idx != g->var_idx) return 0;
    
    // Verifica recursivamente os dois ramos
    return bdd_sao_equivalentes(f->sim, g->sim) && 
           bdd_sao_equivalentes(f->nao, g->nao);
}

// ==================== PROGRAMA PRINCIPAL ====================

/**
 * Função principal que testa a equivalência dos dois circuitos
 * Circuito 1: F1 = (A AND B) OR C
 * Circuito 2: F2 = (A OR C) AND (B OR C)
 * Estes circuitos são logicamente equivalentes
 */
void testar_circuitos() 
{
    printf("VERIFICACAO DE CIRCUITOS COM BDD\n");
    printf("=================================\n\n");
    
    // Inicializa o sistema BDD
    GerenciadorBDD *ger = bdd_iniciar();
    
    // Cria as variáveis booleanas
    int A = bdd_nova_var(ger, "A");
    int B = bdd_nova_var(ger, "B");
    int C = bdd_nova_var(ger, "C");
    
    // Cria os nós para cada variável
    NoBDD *varA = bdd_variavel(ger, A);
    NoBDD *varB = bdd_variavel(ger, B);
    NoBDD *varC = bdd_variavel(ger, C);
    
    printf("Circuitos para ComparaCAO:\n");
    printf("1: F1 = (A E B) OU C\n");
    printf("2: F2 = (A OU C) E (B OU C)\n\n");
    
    // Constrói o Circuito 1: F1 = (A AND B) OR C
    NoBDD *F1_e = bdd_e(ger, varA, varB);      // (A AND B)
    NoBDD *F1 = bdd_ou(ger, F1_e, varC);       // (A AND B) OR C
    
    // Constrói o Circuito 2: F2 = (A OR C) AND (B OR C)
    NoBDD *F2_ou1 = bdd_ou(ger, varA, varC);   // (A OR C)
    NoBDD *F2_ou2 = bdd_ou(ger, varB, varC);   // (B OR C)
    NoBDD *F2 = bdd_e(ger, F2_ou1, F2_ou2);    // (A OR C) AND (B OR C)
    
    printf("Resultado da Verificacao:\n");
    if (bdd_sao_equivalentes(F1, F2)) 
    {
        printf("✓ F1 e F2 sao EQUIVALENTES!\n");
        printf("  Os dois circuitos implementam a mesma funcao logica.\n");
    } 
    else {

        printf("✗ F1 e F2 nao sao equivalentes!\n");
    }
    
    
}

/**
 * Função principal do programa
 */
int main() {
    testar_circuitos();
    return 0;
}