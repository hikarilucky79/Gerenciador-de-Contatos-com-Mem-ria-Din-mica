#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME 100
#define MAX_TELEFONE 20
#define MAX_EMAIL 100
#define ARQUIVO_CONTATOS "contatos.dat"

typedef struct {
    char nome[MAX_NOME];
    char telefone[MAX_TELEFONE];
    char email[MAX_EMAIL];
} Contato;

static Contato *lista_contatos = NULL;
static int total_contatos = 0;
static int capacidade_lista = 0;

void criar_contato();
void exibir_contato(const Contato *c);
void listar_todos();
Contato* buscar_contato(const char *nome_busca);
void remover_contato();
void editar_contato();
void ordenar_contatos();
int validar_email(const char *email);
void liberar_memoria();
int contar_contatos();
void salvar_em_arquivo();
int carregar_de_arquivo();
void pausar_e_limpar();
void limpar_buffer();
void exibir_menu();
void expandir_lista();
void exibir_contato_detalhado(const Contato *c, int indice);

int main() {
    int opcao;
    char nome_busca[200];

    if (carregar_de_arquivo() == -1) {
        printf("[i] Nenhum arquivo de contatos encontrado. Iniciando com lista vazia.\n");
    } else {
        printf("[+] %d contato(s) carregado(s) do arquivo!\n", total_contatos);
    }

    printf("\n");
    system("pause");

    do {
        system("cls || clear");
        exibir_menu();

        printf("\nEscolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            printf("[-] Entrada invalida! Digite apenas numeros.\n");
            limpar_buffer();
            system("pause");
            continue;
        }
        limpar_buffer();

        printf("\n");

        switch (opcao) {
            case 1:
                criar_contato();
                break;
            case 2:
                listar_todos();
                break;
            case 3:
                printf("=== BUSCAR CONTATO ===\n");
                printf("Digite o nome (ou parte dele) para buscar: ");
                fgets(nome_busca, sizeof(nome_busca), stdin);
                nome_busca[strcspn(nome_busca, "\n")] = '\0';

                {
                    Contato *encontrado = buscar_contato(nome_busca);
                    if (encontrado != NULL) {
                        printf("\n[+] Contato encontrado:\n");
                        exibir_contato_detalhado(encontrado, -1);
                    } else {
                        printf("[-] Contato nao encontrado!\n");
                    }
                }
                break;
            case 4:
                remover_contato();
                break;
            case 5:
                editar_contato();
                break;
            case 6:
                printf("[i] Total de contatos cadastrados: %d\n", contar_contatos());
                break;
            case 7:
                ordenar_contatos();
                break;
            case 8:
                salvar_em_arquivo();
                break;
            case 9:
                liberar_memoria();
                printf("[i] Memoria liberada. Programa encerrado com sucesso!\n");
                printf("\n");
                return 0;
            default:
                printf("[-] Opcao invalida! Tente novamente.\n");
        }

        pausar_e_limpar();

    } while (opcao != 9);

    return 0;
}

void expandir_lista() {
    if (capacidade_lista == 0) {
        capacidade_lista = 4;
    } else {
        capacidade_lista *= 2;
    }

    Contato *temp = realloc(lista_contatos, capacidade_lista * sizeof(Contato));
    if (temp == NULL) {
        printf("[-] ERRO CRITICO: Falha ao alocar memoria!\n");
        liberar_memoria();
        exit(EXIT_FAILURE);
    }
    lista_contatos = temp;
}

void criar_contato() {
    printf("[+] === ADICIONAR NOVO CONTATO ===\n");

    if (total_contatos >= capacidade_lista) {
        expandir_lista();
    }

    Contato *novo = &lista_contatos[total_contatos];

    printf("Nome: ");
    fgets(novo->nome, MAX_NOME, stdin);
    novo->nome[strcspn(novo->nome, "\n")] = '\0';

    printf("Telefone: ");
    fgets(novo->telefone, MAX_TELEFONE, stdin);
    novo->telefone[strcspn(novo->telefone, "\n")] = '\0';

    do {
        printf("E-mail: ");
        fgets(novo->email, MAX_EMAIL, stdin);
        novo->email[strcspn(novo->email, "\n")] = '\0';

        if (!validar_email(novo->email)) {
            printf("[-] E-mail invalido! Deve conter o caractere '@'.\n");
            printf("Tente novamente.\n\n");
        }
    } while (!validar_email(novo->email));

    total_contatos++;
    printf("\n[+] Contato '%s' adicionado com sucesso!\n", novo->nome);
}

void exibir_contato(const Contato *c) {
    if (c == NULL) return;

    printf("Nome    : %s\n", c->nome);
    printf("Telefone: %s\n", c->telefone);
    printf("E-mail  : %s\n", c->email);
}

void exibir_contato_detalhado(const Contato *c, int indice) {
    if (c == NULL) return;

    if (indice >= 0) {
        printf("\n--- Contato #%d ---\n", indice + 1);
    }
    exibir_contato(c);
}

void listar_todos() {
    printf("[i] === LISTA DE CONTATOS (%d) ===\n", total_contatos);

    if (total_contatos == 0) {
        printf("[i] Nenhum contato cadastrado.\n");
        return;
    }

    for (int i = 0; i < total_contatos; i++) {
        printf("\n[Contato %d/%d]\n", i + 1, total_contatos);
        exibir_contato(&lista_contatos[i]);
    }
}

Contato* buscar_contato(const char *nome_busca) {
    if (total_contatos == 0 || nome_busca == NULL || strlen(nome_busca) == 0) {
        return NULL;
    }

    char busca_lower[MAX_NOME];
    char nome_lower[MAX_NOME];

    strncpy(busca_lower, nome_busca, MAX_NOME - 1);
    busca_lower[MAX_NOME - 1] = '\0';
    for (int i = 0; busca_lower[i]; i++) {
        busca_lower[i] = tolower((unsigned char)busca_lower[i]);
    }

    for (int i = 0; i < total_contatos; i++) {
        strncpy(nome_lower, lista_contatos[i].nome, MAX_NOME - 1);
        nome_lower[MAX_NOME - 1] = '\0';
        for (int j = 0; nome_lower[j]; j++) {
            nome_lower[j] = tolower((unsigned char)nome_lower[j]);
        }

        if (strstr(nome_lower, busca_lower) != NULL) {
            return &lista_contatos[i];
        }
    }

    return NULL;
}

int validar_email(const char *email) {
    if (email == NULL || strlen(email) == 0) return 0;

    const char *arroba = strchr(email, '@');
    if (arroba == NULL) return 0;

    if (arroba == email) return 0;
    if (*(arroba + 1) == '\0') return 0;

    if (strchr(arroba + 1, '.') == NULL) return 0;

    return 1;
}

void remover_contato() {
    if (total_contatos == 0) {
        printf("[i] Nenhum contato para remover.\n");
        return;
    }

    char nome_remover[MAX_NOME];
    printf("[i] === REMOVER CONTATO ===\n");
    printf("Digite o nome do contato a remover: ");
    fgets(nome_remover, MAX_NOME, stdin);
    nome_remover[strcspn(nome_remover, "\n")] = '\0';

    Contato *alvo = buscar_contato(nome_remover);

    if (alvo == NULL) {
        printf("[-] Contato '%s' nao encontrado!\n", nome_remover);
        return;
    }

    int indice = (int)(alvo - lista_contatos);

    printf("\nContato encontrado:\n");
    exibir_contato_detalhado(alvo, indice);

    {
        char confirmacao;
        printf("\nTem certeza que deseja remover este contato? (s/n): ");
        scanf("%c", &confirmacao);
        limpar_buffer();

        if (tolower((unsigned char)confirmacao) != 's') {
            printf("[-] Remocao cancelada.\n");
            return;
        }
    }

    for (int i = indice; i < total_contatos - 1; i++) {
        lista_contatos[i] = lista_contatos[i + 1];
    }

    total_contatos--;
    printf("\n[+] Contato removido com sucesso!\n");

    if (capacidade_lista > 8 && total_contatos < capacidade_lista / 4) {
        capacidade_lista /= 2;
        Contato *temp = realloc(lista_contatos, capacidade_lista * sizeof(Contato));
        if (temp != NULL) {
            lista_contatos = temp;
        }
    }
}

void editar_contato() {
    if (total_contatos == 0) {
        printf("[i] Nenhum contato para editar.\n");
        return;
    }

    char nome_editar[MAX_NOME];
    printf("[+] === EDITAR CONTATO ===\n");
    printf("Digite o nome do contato a editar: ");
    fgets(nome_editar, MAX_NOME, stdin);
    nome_editar[strcspn(nome_editar, "\n")] = '\0';

    Contato *alvo = buscar_contato(nome_editar);

    if (alvo == NULL) {
        printf("[-] Contato '%s' nao encontrado!\n", nome_editar);
        return;
    }

    printf("\nContato atual:\n");
    exibir_contato(alvo);

    printf("\n--- O que deseja editar? ---\n");
    printf("1. Nome\n");
    printf("2. Telefone\n");
    printf("3. E-mail\n");
    printf("4. Cancelar\n");
    printf("Opcao: ");

    {
        int opcao_edicao;
        if (scanf("%d", &opcao_edicao) != 1) {
            printf("[-] Opcao invalida!\n");
            limpar_buffer();
            return;
        }
        limpar_buffer();

        printf("\n");
        switch (opcao_edicao) {
            case 1:
                printf("Novo nome (%d chars max): ", MAX_NOME - 1);
                fgets(alvo->nome, MAX_NOME, stdin);
                alvo->nome[strcspn(alvo->nome, "\n")] = '\0';
                printf("[+] Nome atualizado!\n");
                break;
            case 2:
                printf("Novo telefone (%d chars max): ", MAX_TELEFONE - 1);
                fgets(alvo->telefone, MAX_TELEFONE, stdin);
                alvo->telefone[strcspn(alvo->telefone, "\n")] = '\0';
                printf("[+] Telefone atualizado!\n");
                break;
            case 3: {
                char novo_email[MAX_EMAIL];
                do {
                    printf("Novo e-mail: ");
                    fgets(novo_email, MAX_EMAIL, stdin);
                    novo_email[strcspn(novo_email, "\n")] = '\0';

                    if (!validar_email(novo_email)) {
                        printf("[-] E-mail invalido! Deve conter '@'.\n");
                    }
                } while (!validar_email(novo_email));

                strncpy(alvo->email, novo_email, MAX_EMAIL - 1);
                alvo->email[MAX_EMAIL - 1] = '\0';
                printf("[+] E-mail atualizado!\n");
                break;
            }
            case 4:
                printf("[-] Edicao cancelada.\n");
                break;
            default:
                printf("[-] Opcao invalida!\n");
        }
    }
}

void ordenar_contatos() {
    if (total_contatos < 2) {
        printf("[i] Necessario pelo menos 2 contatos para ordenar.\n");
        return;
    }

    printf("[i] Ordenando %d contatos alfabeticamente...\n", total_contatos);

    for (int i = 0; i < total_contatos - 1; i++) {
        for (int j = 0; j < total_contatos - i - 1; j++) {
            char nome1[MAX_NOME], nome2[MAX_NOME];
            strncpy(nome1, lista_contatos[j].nome, MAX_NOME - 1);
            strncpy(nome2, lista_contatos[j + 1].nome, MAX_NOME - 1);
            nome1[MAX_NOME - 1] = '\0';
            nome2[MAX_NOME - 1] = '\0';

            for (int k = 0; nome1[k]; k++) nome1[k] = tolower((unsigned char)nome1[k]);
            for (int k = 0; nome2[k]; k++) nome2[k] = tolower((unsigned char)nome2[k]);

            if (strcmp(nome1, nome2) > 0) {
                Contato temp = lista_contatos[j];
                lista_contatos[j] = lista_contatos[j + 1];
                lista_contatos[j + 1] = temp;
            }
        }
    }

    printf("[+] Contatos ordenados alfabeticamente!\n");

    listar_todos();
}

int contar_contatos() {
    return total_contatos;
}

void salvar_em_arquivo() {
    FILE *arquivo = fopen(ARQUIVO_CONTATOS, "wb");
    if (arquivo == NULL) {
        printf("[-] Erro ao abrir arquivo para gravacao!\n");
        return;
    }

    fwrite(&total_contatos, sizeof(int), 1, arquivo);
    fwrite(lista_contatos, sizeof(Contato), total_contatos, arquivo);

    fclose(arquivo);
    printf("[+] %d contato(s) salvos em '%s'!\n", total_contatos, ARQUIVO_CONTATOS);
}

int carregar_de_arquivo() {
    FILE *arquivo = fopen(ARQUIVO_CONTATOS, "rb");
    if (arquivo == NULL) {
        return -1;
    }

    {
        int qtd_lida;
        if (fread(&qtd_lida, sizeof(int), 1, arquivo) != 1) {
            fclose(arquivo);
            return -1;
        }

        if (qtd_lida <= 0) {
            fclose(arquivo);
            return 0;
        }

        lista_contatos = malloc(qtd_lida * sizeof(Contato));
        if (lista_contatos == NULL) {
            printf("[-] Erro ao alocar memoria para carregar contatos!\n");
            fclose(arquivo);
            return -1;
        }

        capacidade_lista = qtd_lida;

        {
            size_t lidos = fread(lista_contatos, sizeof(Contato), qtd_lida, arquivo);
            fclose(arquivo);

            if (lidos != (size_t)qtd_lida) {
                printf("[!] Aviso: Alguns contatos podem ter sido lidos incorretamente.\n");
            }

            total_contatos = (int)lidos;
            return total_contatos;
        }
    }
}

void liberar_memoria() {
    if (lista_contatos != NULL) {
        free(lista_contatos);
        lista_contatos = NULL;
    }
    total_contatos = 0;
    capacidade_lista = 0;
    printf("[i] Memoria liberada com sucesso.\n");
}

void exibir_menu() {
    printf("========================================\n");
    printf("       GERENCIADOR DE CONTATOS          \n");
    printf("========================================\n");
    printf("  1. Adicionar contato\n");
    printf("  2. Listar todos os contatos\n");
    printf("  3. Buscar contato pelo nome\n");
    printf("  4. Remover contato\n");
    printf("  5. Editar contato\n");
    printf("  6. Contar total de contatos\n");
    printf("  7. Ordenar contatos (A-Z)\n");
    printf("  8. Salvar em arquivo\n");
    printf("  9. Sair (liberar memoria)\n");
    printf("========================================\n");
    printf("Total atual: %d contato(s)\n", total_contatos);
}

void pausar_e_limpar() {
    printf("\n");
    system("pause");
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
