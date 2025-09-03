#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -------------------- Estruturas --------------------

// Estrutura que representa uma sala da mansão
typedef struct Sala {
    char nome[50];              // Nome da sala
    char pista[100];            // Pista associada à sala (pode ser "")
    struct Sala *esquerda;      // Caminho para a esquerda
    struct Sala *direita;       // Caminho para a direita
} Sala;

// Estrutura para a árvore BST de pistas
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;


// -------------------- Funções da Mansão --------------------

/*
 * Função: criarSala
 * -----------------
 * Cria dinamicamente uma sala com ou sem pista associada.
 */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *novaSala = (Sala*) malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro de alocacao de memoria para a sala!\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    if (pista != NULL) strcpy(novaSala->pista, pista);
    else strcpy(novaSala->pista, "");
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}


// -------------------- Funções da Árvore de Pistas --------------------

/*
 * Função: criarPistaNode
 * ----------------------
 * Cria dinamicamente um nó de pista.
 */
PistaNode* criarPistaNode(const char *pista) {
    PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
    if (novo == NULL) {
        printf("Erro de alocacao de memoria para a pista!\n");
        exit(1);
    }
    strcpy(novo->pista, pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

/*
 * Função: inserirPista
 * --------------------
 * Insere uma pista na árvore BST em ordem alfabética.
 */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) return criarPistaNode(pista);

    if (strcmp(pista, raiz->pista) < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (strcmp(pista, raiz->pista) > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    return raiz;
}

/*
 * Função: exibirPistas
 * --------------------
 * Exibe todas as pistas da BST em ordem alfabética (in-order traversal).
 */
void exibirPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}


// -------------------- Exploração --------------------

/*
 * Função: explorarSalasComPistas
 * ------------------------------
 * Permite ao jogador explorar a mansão, coletando pistas.
 */
void explorarSalasComPistas(Sala *salaAtual, PistaNode **pistas) {
    char escolha;

    while (salaAtual != NULL) {
        printf("\nVocê está em: %s\n", salaAtual->nome);

        // Se a sala tem pista, coleta
        if (strlen(salaAtual->pista) > 0) {
            printf("Você encontrou uma pista: \"%s\"\n", salaAtual->pista);
            *pistas = inserirPista(*pistas, salaAtual->pista);
        } else {
            printf("Nenhuma pista aqui.\n");
        }

        // Mostrar opções
        printf("Escolha um caminho:\n");
        if (salaAtual->esquerda != NULL) {
            printf("  (e) Ir para %s\n", salaAtual->esquerda->nome);
        }
        if (salaAtual->direita != NULL) {
            printf("  (d) Ir para %s\n", salaAtual->direita->nome);
        }
        printf("  (s) Sair da exploração\n");
        printf("Opção: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' && salaAtual->esquerda != NULL) {
            salaAtual = salaAtual->esquerda;
        } else if (escolha == 'd' && salaAtual->direita != NULL) {
            salaAtual = salaAtual->direita;
        } else if (escolha == 's') {
            printf("\nVocê decidiu encerrar a exploração.\n");
            return;
        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}


// -------------------- Função Principal --------------------

int main() {
    // Criação manual do mapa da mansão
    Sala *hall = criarSala("Hall de Entrada", "Pegada misteriosa");
    Sala *salaEstar = criarSala("Sala de Estar", "Um retrato quebrado");
    Sala *cozinha = criarSala("Cozinha", "Uma faca ensanguentada");
    Sala *jardim = criarSala("Jardim", "");
    Sala *biblioteca = criarSala("Biblioteca", "Um diário rasgado");
    Sala *quarto = criarSala("Quarto", "Um bilhete suspeito");
    Sala *porao = criarSala("Porão", "Uma chave enferrujada");

    // Montagem da árvore da mansão
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = jardim;
    salaEstar->direita = biblioteca;

    cozinha->esquerda = quarto;
    cozinha->direita = porao;

    // Árvore de pistas inicial (vazia)
    PistaNode *pistas = NULL;

    // Início do jogo
    printf("Bem-vindo ao Detective Quest!\n");
    explorarSalasComPistas(hall, &pistas);

    // Exibir pistas coletadas
    printf("\n===== Pistas Coletadas (em ordem alfabética) =====\n");
    if (pistas == NULL) {
        printf("Você não coletou nenhuma pista.\n");
    } else {
        exibirPistas(pistas);
    }

    return 0;
}
