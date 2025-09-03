#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura que representa uma sala da mansão
typedef struct Sala {
    char nome[50];              // Nome da sala
    struct Sala *esquerda;      // Caminho para a sala à esquerda
    struct Sala *direita;       // Caminho para a sala à direita
} Sala;

/*
 * Função: criarSala
 * -----------------
 * Cria dinamicamente uma nova sala da mansão.
 *
 * nome: string com o nome da sala.
 *
 * retorna: ponteiro para a sala criada.
 */
Sala* criarSala(const char *nome) {
    Sala *novaSala = (Sala*) malloc(sizeof(Sala));
    if (novaSala == NULL) {
        printf("Erro de alocacao de memoria!\n");
        exit(1);
    }
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

/*
 * Função: explorarSalas
 * ----------------------
 * Permite que o jogador explore a mansão a partir de uma sala,
 * escolhendo ir para a esquerda (e), para a direita (d) ou sair (s).
 *
 * salaAtual: ponteiro para a sala de início da exploração.
 */
void explorarSalas(Sala *salaAtual) {
    char escolha;

    while (salaAtual != NULL) {
        printf("\nVocê está em: %s\n", salaAtual->nome);

        // Se a sala não tem caminhos, termina a exploração
        if (salaAtual->esquerda == NULL && salaAtual->direita == NULL) {
            printf("Você chegou ao fim do caminho. Fim da exploração!\n");
            return;
        }

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
            printf("Você decidiu sair da exploração.\n");
            return;
        } else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

/*
 * Função principal: main
 * ----------------------
 * Monta o mapa fixo da mansão (árvore binária) e inicia a exploração.
 */
int main() {
    // Criação manual da árvore (mapa da mansão)
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *jardim = criarSala("Jardim");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *quarto = criarSala("Quarto");
    Sala *porao = criarSala("Porão");

    // Montagem da árvore
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = jardim;
    salaEstar->direita = biblioteca;

    cozinha->esquerda = quarto;
    cozinha->direita = porao;

    // Inicia a exploração
    printf("Bem-vindo ao Detective Quest!\n");
    explorarSalas(hall);

    return 0;
}
