#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* =======================================================================
   ESTRUTURAS DE DADOS
   ======================================================================= */

// Nó da árvore da mansão (mapa)
typedef struct Sala {
    char nome[64];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

/* BST de pistas (mantém em ordem e conta repetições) */
typedef struct PistaBST {
    char pista[128];
    int freq;                  // quantas vezes essa pista foi coletada
    struct PistaBST *esq;
    struct PistaBST *dir;
} PistaBST;

/* Tabela hash (encadeamento separado) para pista -> suspeito */
typedef struct HashNode {
    char pista[128];
    char suspeito[64];
    struct HashNode *prox;
} HashNode;

typedef struct {
    HashNode **buckets;
    size_t capacidade;
} HashTable;

/* =======================================================================
   ASSINATURAS
   ======================================================================= */

/* Mansão */
Sala* criarSala(const char *nome);                              // cria dinamicamente um cômodo
void explorarSalas(Sala *inicio, PistaBST **pistas, HashTable *ht); // navega pela árvore e ativa o sistema de pistas

/* Pistas (BST) */
PistaBST* inserirPista(PistaBST *raiz, const char *pista);      // insere (ou incrementa) pista na BST
void exibirPistasInOrder(PistaBST *raiz);                       // imprime pistas em ordem alfabética
int contarPistasDoSuspeito(PistaBST *raiz, HashTable *ht, const char *acusado);

/* Hash (pista -> suspeito) */
HashTable* criarHash(size_t capacidade);
void liberarHash(HashTable *ht);
unsigned long hash_djb2(const char *str);
void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito);  // associa pista/suspeito
const char* encontrarSuspeito(HashTable *ht, const char *pista);             // consulta o suspeito correspondente

/* Julgamento final */
void verificarSuspeitoFinal(PistaBST *pistas, HashTable *ht);   // conduz à fase de julgamento final

/* Utilidades */
const char* obterPistaParaSala(const char *nomeSala);           // regra fixa: sala -> pista (ou "")
void limparNewline(char *s);

/* (Opcional) liberar memória das árvores */
void liberarSalas(Sala *raiz);
void liberarBST(PistaBST *raiz);

/* =======================================================================
   IMPLEMENTAÇÃO
   ======================================================================= */

/* ------------------- Mansão ------------------- */
/*
 * criarSala()
 * Cria dinamicamente um cômodo com nome.
 */
Sala* criarSala(const char *nome) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro de alocacao para Sala\n");
        exit(1);
    }
    strncpy(s->nome, nome, sizeof(s->nome));
    s->nome[sizeof(s->nome)-1] = '\0';
    s->esquerda = s->direita = NULL;
    return s;
}

/*
 * explorarSalas()
 * Navega pela árvore binária a partir do "inicio", oferecendo escolhas:
 * (e) esquerda, (d) direita, (s) sair.
 * Ao entrar numa sala, detecta a pista associada por regra fixa e
 * adiciona/atualiza na BST de pistas; também informa o suspeito relacionado
 * via tabela hash (se houver).
 */
void explorarSalas(Sala *inicio, PistaBST **pistas, HashTable *ht) {
    Sala *atual = inicio;
    char op;

    puts("=== Exploracao da Mansao ===");
    while (atual) {
        printf("\nVoce esta em: %s\n", atual->nome);

        // Pega a pista associada a esta sala (pode ser "")
        const char *p = obterPistaParaSala(atual->nome);
        if (p && p[0] != '\0') {
            printf("Pista encontrada: \"%s\"\n", p);
            *pistas = inserirPista(*pistas, p);

            const char *sus = encontrarSuspeito(ht, p);
            if (sus) {
                printf("Essa pista esta associada ao suspeito: %s\n", sus);
            } else {
                printf("Nao ha suspeito associado a esta pista.\n");
            }
        } else {
            printf("Nenhuma pista aqui.\n");
        }

        // Opções de navegação
        printf("\nEscolha um caminho:\n");
        if (atual->esquerda) printf("  (e) Ir para %s\n", atual->esquerda->nome);
        if (atual->direita)  printf("  (d) Ir para %s\n", atual->direita->nome);
        printf("  (s) Sair da exploracao\n");
        printf("Opcao: ");
        if (scanf(" %c", &op) != 1) op = 's';

        if (op == 'e' && atual->esquerda) {
            atual = atual->esquerda;
        } else if (op == 'd' && atual->direita) {
            atual = atual->direita;
        } else if (op == 's') {
            puts("\nExploracao encerrada.");
            break;
        } else {
            puts("Opcao invalida. Tente novamente.");
        }
    }
}

/* ------------------- BST de Pistas ------------------- */
/*
 * inserirPista()
 * Insere a pista na BST em ordem; se já existir, incrementa a frequencia.
 */
PistaBST* inserirPista(PistaBST *raiz, const char *pista) {
    if (!raiz) {
        PistaBST *n = (PistaBST*) malloc(sizeof(PistaBST));
        if (!n) {
            fprintf(stderr, "Erro de alocacao na BST de pistas\n");
            exit(1);
        }
        strncpy(n->pista, pista, sizeof(n->pista));
        n->pista[sizeof(n->pista)-1] = '\0';
        n->freq = 1;
        n->esq = n->dir = NULL;
        return n;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) raiz->esq = inserirPista(raiz->esq, pista);
    else if (cmp > 0) raiz->dir = inserirPista(raiz->dir, pista);
    else raiz->freq++; // pista repetida: soma frequencia
    return raiz;
}

/*
 * exibirPistasInOrder()
 * Percorre em-ordem para imprimir em ordem alfabetica.
 */
void exibirPistasInOrder(PistaBST *raiz) {
    if (!raiz) return;
    exibirPistasInOrder(raiz->esq);
    if (raiz->freq == 1) printf("- %s\n", raiz->pista);
    else                 printf("- %s (x%d)\n", raiz->pista, raiz->freq);
    exibirPistasInOrder(raiz->dir);
}

/*
 * contarPistasDoSuspeito()
 * Soma as frequencias das pistas na BST que mapeiam para o suspeito "acusado".
 */
int contarPistasDoSuspeito(PistaBST *raiz, HashTable *ht, const char *acusado) {
    if (!raiz) return 0;
    int total = 0;
    total += contarPistasDoSuspeito(raiz->esq, ht, acusado);
    const char *sus = encontrarSuspeito(ht, raiz->pista);
    if (sus && strcmp(sus, acusado) == 0) {
        total += raiz->freq;
    }
    total += contarPistasDoSuspeito(raiz->dir, ht, acusado);
    return total;
}

/* ------------------- Tabela Hash ------------------- */
/*
 * criarHash()
 * Cria uma tabela hash com 'capacidade' buckets.
 */
HashTable* criarHash(size_t capacidade) {
    HashTable *ht = (HashTable*) malloc(sizeof(HashTable));
    if (!ht) { fprintf(stderr, "Erro de alocacao HashTable\n"); exit(1); }
    ht->capacidade = capacidade;
    ht->buckets = (HashNode**) calloc(capacidade, sizeof(HashNode*));
    if (!ht->buckets) { fprintf(stderr, "Erro de alocacao buckets\n"); exit(1); }
    return ht;
}

void liberarHash(HashTable *ht) {
    if (!ht) return;
    for (size_t i = 0; i < ht->capacidade; ++i) {
        HashNode *cur = ht->buckets[i];
        while (cur) {
            HashNode *nx = cur->prox;
            free(cur);
            cur = nx;
        }
    }
    free(ht->buckets);
    free(ht);
}

/* djb2: hash simples e eficiente para strings */
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

/*
 * inserirNaHash()
 * Insere ou substitui a associacao pista -> suspeito (encadeamento separado).
 */
void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito) {
    unsigned long h = hash_djb2(pista) % ht->capacidade;
    HashNode *cur = ht->buckets[h];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) {
            // substitui suspeito
            strncpy(cur->suspeito, suspeito, sizeof(cur->suspeito));
            cur->suspeito[sizeof(cur->suspeito)-1] = '\0';
            return;
        }
        cur = cur->prox;
    }
    // novo nó
    HashNode *n = (HashNode*) malloc(sizeof(HashNode));
    if (!n) { fprintf(stderr, "Erro de alocacao HashNode\n"); exit(1); }
    strncpy(n->pista, pista, sizeof(n->pista));
    n->pista[sizeof(n->pista)-1] = '\0';
    strncpy(n->suspeito, suspeito, sizeof(n->suspeito));
    n->suspeito[sizeof(n->suspeito)-1] = '\0';
    n->prox = ht->buckets[h];
    ht->buckets[h] = n;
}

/*
 * encontrarSuspeito()
 * Retorna o suspeito associado à pista (ou NULL se não houver).
 */
const char* encontrarSuspeito(HashTable *ht, const char *pista) {
    unsigned long h = hash_djb2(pista) % ht->capacidade;
    HashNode *cur = ht->buckets[h];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) return cur->suspeito;
        cur = cur->prox;
    }
    return NULL;
}

/* ------------------- Julgamento Final ------------------- */
/*
 * verificarSuspeitoFinal()
 * Lista as pistas e pergunta quem é o culpado. Aprova acusação se >= 2 pistas
 * apontarem para o acusado.
 */
void verificarSuspeitoFinal(PistaBST *pistas, HashTable *ht) {
    puts("\n===== Pistas coletadas (ordem alfabetica) =====");
    if (!pistas) {
        puts("Nenhuma pista coletada.");
        return;
    }
    exibirPistasInOrder(pistas);

    char acusado[64];
    printf("\nQuem voce acusa como culpado? ");
    getchar(); // consome '\n' que pode sobrar do scanf anterior
    if (!fgets(acusado, sizeof(acusado), stdin)) return;
    limparNewline(acusado);

    int evidencias = contarPistasDoSuspeito(pistas, ht, acusado);
    printf("\nVeredito: %d pista(s) apontam para %s.\n", evidencias, acusado);
    if (evidencias >= 2) {
        printf("Acusacao PROCEDENTE: ha evidencias suficientes.\n");
    } else {
        printf("Acusacao IMPROCEDENTE: evidencias insuficientes.\n");
    }
}

/* ------------------- Utilidades ------------------- */
/*
 * obterPistaParaSala()
 * Regras fixas: cada sala tem (ou nao) uma pista pre-definida.
 * Ajuste os pares conforme sua narrativa.
 */
const char* obterPistaParaSala(const char *nomeSala) {
    if (strcmp(nomeSala, "Hall de Entrada") == 0) return "Pegadas de lama recentes";
    if (strcmp(nomeSala, "Sala de Estar") == 0)   return "Retrato trincado";
    if (strcmp(nomeSala, "Cozinha") == 0)         return "Faca com resquicios";
    if (strcmp(nomeSala, "Biblioteca") == 0)      return "Diario com paginas rasgadas";
    if (strcmp(nomeSala, "Jardim") == 0)          return "Luvas enterradas";
    if (strcmp(nomeSala, "Quarto") == 0)          return "Bilhete anonimo";
    if (strcmp(nomeSala, "Porão") == 0 || strcmp(nomeSala, "Porao") == 0) return "Chave enferrujada";
    return ""; // sem pista
}

void limparNewline(char *s) {
    size_t n = strlen(s);
    if (n && s[n-1] == '\n') s[n-1] = '\0';
}

/* Liberacao opcional de memoria */
void liberarSalas(Sala *r) {
    if (!r) return;
    liberarSalas(r->esquerda);
    liberarSalas(r->direita);
    free(r);
}
void liberarBST(PistaBST *r) {
    if (!r) return;
    liberarBST(r->esq);
    liberarBST(r->dir);
    free(r);
}

/* =======================================================================
   MAIN: monta mapa fixo, popula hash pista->suspeito, roda exploracao e julgamento
   ======================================================================= */

int main(void) {
    /* --- Monta o mapa fixo da mansao (arvore binaria) --- */
    Sala *hall        = criarSala("Hall de Entrada");
    Sala *salaEstar   = criarSala("Sala de Estar");
    Sala *cozinha     = criarSala("Cozinha");
    Sala *jardim      = criarSala("Jardim");
    Sala *biblioteca  = criarSala("Biblioteca");
    Sala *quarto      = criarSala("Quarto");
    Sala *porao       = criarSala("Porao");

    // Ligações (exemplo de mapa)
    hall->esquerda = salaEstar;   hall->direita = cozinha;
    salaEstar->esquerda = jardim; salaEstar->direita = biblioteca;
    cozinha->esquerda = quarto;   cozinha->direita  = porao;

    /* --- Cria tabela hash e cadastra associacoes pista -> suspeito --- */
    HashTable *ht = criarHash(101); // capacidade primo razoavel

    // Você pode ajustar a autoria das pistas como preferir:
    inserirNaHash(ht, "Pegadas de lama recentes",   "Jardineiro");
    inserirNaHash(ht, "Retrato trincado",           "Herdeira");
    inserirNaHash(ht, "Faca com resquicios",        "Chef");
    inserirNaHash(ht, "Diario com paginas rasgadas","Bibliotecario");
    inserirNaHash(ht, "Luvas enterradas",           "Jardineiro");
    inserirNaHash(ht, "Bilhete anonimo",            "Herdeira");
    inserirNaHash(ht, "Chave enferrujada",          "Zelador");

    /* --- BST de pistas (vazia inicialmente) --- */
    PistaBST *pistas = NULL;

    /* --- Jogo --- */
    puts("Bem-vindo a Detective Quest - Julgamento Final!");
    explorarSalas(hall, &pistas, ht);
    verificarSuspeitoFinal(pistas, ht);

    /* --- (Opcional) liberar memoria --- */
    liberarBST(pistas);
    liberarSalas(hall);
    liberarHash(ht);

    return 0;
}
