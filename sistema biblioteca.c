#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* ============================================================================
 * CONSTANTES E DEFINIÇÕES GLOBAIS
 * ============================================================================ */
#define MAX_LIVROS 500
#define MAX_USUARIOS 500
#define MAX_EMPRESTIMOS 1000

#define ARQ_LIVROS "livros.txt"
#define ARQ_USUARIOS "usuarios.txt"
#define ARQ_EMPRESTIMOS "emprestimos.txt"

#define BKP_LIVROS "livros_backup.txt"
#define BKP_USUARIOS "usuarios_backup.txt"
#define BKP_EMPRESTIMOS "emprestimos_backup.txt"

/* ============================================================================
 * ESTRUTURAS DE DADOS (STRUCTS)
 * ============================================================================ */

/* Representa uma data no calendário civil (dia, mês e ano). */
typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

/* Representa um livro do acervo da biblioteca, incluindo seu controle de estoque
 * e contador histórico de empréstimos para emissão de relatórios. */
typedef struct {
    int codigo;
    char titulo[101];       /* Até 100 caracteres + terminador nulo '\0' */
    char autor[81];         /* Até 80 caracteres + terminador nulo '\0' */
    char editora[61];       /* Até 60 caracteres + terminador nulo '\0' */
    int anoPublicacao;
    int exemplaresDisponiveis;
    char status[15];        /* "disponivel" ou "emprestado" */
    int totalEmprestimos;   /* Acumulador de empréstimos já realizados */
} Livro;

/* Representa um leitor/usuário cadastrado na biblioteca. */
typedef struct {
    int matricula;
    char nome[101];         /* Até 100 caracteres + terminador nulo '\0' */
    char curso[51];         /* Até 50 caracteres + terminador nulo '\0' */
    char telefone[16];      /* Até 15 caracteres + terminador nulo '\0' */
    Data dataCadastro;
} Usuario;

/* Representa uma transação de empréstimo vinculando um usuário a um livro,
 * com controle de data inicial, prazo de 7 dias e status atual. */
typedef struct {
    int codigo;
    int matriculaUsuario;
    int codigoLivro;
    Data dataEmprestimo;
    Data dataPrevistaDevolucao;
    char status[15];        /* "ativo" ou "devolvido" */
} Emprestimo;

/* Estrutura central que agrupa todos os vetores e contadores do sistema.
 * Facilita a passagem de estado por ponteiro entre todas as funções modulares. */
typedef struct {
    Livro livros[MAX_LIVROS];
    int qtdLivros;

    Usuario usuarios[MAX_USUARIOS];
    int qtdUsuarios;

    Emprestimo emprestimos[MAX_EMPRESTIMOS];
    int qtdEmprestimos;
} SistemaBiblioteca;

/* ============================================================================
 * PROTÓTIPOS DAS FUNÇÕES
 * ============================================================================ */
void inicializarSistema(SistemaBiblioteca *sys);
void limparBuffer(void);
void lerString(char *destino, int tamanho);
void paraMinusculo(char *str);
int bissexto(int ano);
int diasNoMes(int mes, int ano);
int dataValida(Data d);
Data somarDias(Data d, int dias);
int compararDatas(Data d1, Data d2);
Data obterDataAtual(void);

void salvarDados(const SistemaBiblioteca *sys);
void carregarDados(SistemaBiblioteca *sys);
void copiarArquivo(const char *origem, const char *destino);
void realizarBackup(void);

int buscarIndiceLivroPorCodigo(const SistemaBiblioteca *sys, int codigo);
int buscarIndiceUsuarioPorMatricula(const SistemaBiblioteca *sys, int matricula);
int buscarIndiceEmprestimoPorCodigo(const SistemaBiblioteca *sys, int codigo);

void cadastrarLivro(SistemaBiblioteca *sys);
void cadastrarUsuario(SistemaBiblioteca *sys);
void realizarEmprestimo(SistemaBiblioteca *sys);
void realizarDevolucao(SistemaBiblioteca *sys);
void pesquisarLivros(const SistemaBiblioteca *sys);
void pesquisarUsuarios(const SistemaBiblioteca *sys);
void listarEmprestimosAtivos(const SistemaBiblioteca *sys);

void relatorioLivrosMaisEmprestados(const SistemaBiblioteca *sys);
void relatorioUsuariosComAtraso(const SistemaBiblioteca *sys);
void buscaAvancadaLivros(const SistemaBiblioteca *sys);
void renovarEmprestimo(SistemaBiblioteca *sys);

void menuLivros(SistemaBiblioteca *sys);
void menuUsuarios(SistemaBiblioteca *sys);
void menuEmprestimos(SistemaBiblioteca *sys);
void menuRelatorios(SistemaBiblioteca *sys);

/* ============================================================================
 * FUNÇÃO PRINCIPAL (MAIN)
 * ============================================================================
 * Objetivo: Ponto de entrada do programa. Inicializa a estrutura principal,
 * carrega os dados salvos em disco e exibe o loop do Menu Principal até que
 * o usuário escolha sair (opção 0), garantindo salvamento e backup ao encerrar.
 * ============================================================================ */
int main(void) {
    SistemaBiblioteca sys;
    int opcao;

    inicializarSistema(&sys);
    carregarDados(&sys);

    do {
        printf("\n==================================================\n");
        printf("       SISTEMA DE GERENCIAMENTO DE BIBLIOTECA     \n");
        printf("==================================================\n");
        printf("1. Gerenciar Livros\n");
        printf("2. Gerenciar Usuarios\n");
        printf("3. Gerenciar Emprestimos e Devolucoes\n");
        printf("4. Relatorios e Busca Avancada\n");
        printf("5. Salvar Dados e Gerar Backup Manual\n");
        printf("0. Sair do Sistema\n");
        printf("--------------------------------------------------\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            printf("\n[Erro] Entrada invalida! Digite apenas numeros.\n");
            continue;
        }
        limparBuffer();

        switch (opcao) {
            case 1:
                menuLivros(&sys);
                break;
            case 2:
                menuUsuarios(&sys);
                break;
            case 3:
                menuEmprestimos(&sys);
                break;
            case 4:
                menuRelatorios(&sys);
                break;
            case 5:
                salvarDados(&sys);
                realizarBackup();
                printf("\n[Sucesso] Dados salvos e backup atualizado!\n");
                break;
            case 0:
                salvarDados(&sys);
                realizarBackup();
                printf("\nDados salvos automaticamente. Encerrando o sistema...\n");
                break;
            default:
                printf("\n[Aviso] Opcao inexistente. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}

/* ============================================================================
 * FUNÇÕES DE INICIALIZAÇÃO E UTILITÁRIOS GERAIS
 * ============================================================================ */

/*
 * Função: inicializarSistema
 * Objetivo: Zerar os contadores de livros, usuários e empréstimos na memória.
 * Parâmetro: sys - Ponteiro para a estrutura principal do sistema.
 */
void inicializarSistema(SistemaBiblioteca *sys) {
    sys->qtdLivros = 0;
    sys->qtdUsuarios = 0;
    sys->qtdEmprestimos = 0;
}

/*
 * Função: limparBuffer
 * Objetivo: Descartar caracteres residuais (como o '\n') no buffer de entrada
 * padrão (stdin) após leituras com scanf, evitando que leituras futuras falhem.
 */
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/*
 * Função: lerString
 * Objetivo: Ler uma linha de texto de forma segura usando fgets, removendo a
 * quebra de linha '\n' ao final ou limpando o excesso caso ultrapasse o limite.
 * Parâmetros:
 *   - destino: Vetor de caracteres onde o texto será armazenado.
 *   - tamanho: Capacidade máxima do vetor (incluindo o '\0').
 */
void lerString(char *destino, int tamanho) {
    if (fgets(destino, tamanho, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        } else {
            limparBuffer();
        }
    }
}

/*
 * Função: paraMinusculo
 * Objetivo: Converter todos os caracteres de uma string para letras minúsculas,
 * permitindo buscas textuais insensíveis a maiúsculas/minúsculas (case-insensitive).
 * Parâmetro: str - String a ser convertida no próprio local.
 */
void paraMinusculo(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}

/*
 * Função: bissexto
 * Objetivo: Verificar se um ano é bissexto segundo o calendário gregoriano.
 * Parâmetro: ano - Ano com 4 dígitos.
 * Retorno: 1 se for bissexto, 0 caso contrário.
 */
int bissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

/*
 * Função: diasNoMes
 * Objetivo: Retornar a quantidade exata de dias de um determinado mês e ano,
 * considerando fevereiro com 29 dias em anos bissextos.
 * Parâmetros: mes (1 a 12) e ano.
 * Retorno: Número de dias do mês (28 a 31), ou 0 se o mês for inválido.
 */
int diasNoMes(int mes, int ano) {
    int dias[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (mes == 2 && bissexto(ano)) {
        return 29;
    }
    if (mes >= 1 && mes <= 12) {
        return dias[mes];
    }
    return 0;
}

/*
 * Função: dataValida
 * Objetivo: Validar se uma data informada pelo usuário existe no calendário.
 * Parâmetro: d - Estrutura Data contendo dia, mês e ano.
 * Retorno: 1 se a data for válida, 0 se contiver valores absurdos ou inexistentes.
 */
int dataValida(Data d) {
    if (d.ano < 1900 || d.ano > 2100) return 0;
    if (d.mes < 1 || d.mes > 12) return 0;
    if (d.dia < 1 || d.dia > diasNoMes(d.mes, d.ano)) return 0;
    return 1;
}

/*
 * Função: somarDias
 * Objetivo: Adicionar uma quantidade de dias a uma data base, ajustando
 * automaticamente as viradas de mês e de ano (usado para prazos de 7 dias).
 * Parâmetros: d - Data inicial; dias - Quantidade de dias a somar.
 * Retorno: Nova estrutura Data resultante.
 */
Data somarDias(Data d, int dias) {
    Data resultado = d;
    resultado.dia += dias;

    while (resultado.dia > diasNoMes(resultado.mes, resultado.ano)) {
        resultado.dia -= diasNoMes(resultado.mes, resultado.ano);
        resultado.mes++;
        if (resultado.mes > 12) {
            resultado.mes = 1;
            resultado.ano++;
        }
    }
    return resultado;
}

/*
 * Função: compararDatas
 * Objetivo: Comparar cronologicamente duas datas (d1 e d2).
 * Parâmetros: d1 e d2 - Estruturas Data a serem comparadas.
 * Retorno: -1 se d1 for anterior a d2; 0 se forem iguais; 1 se d1 for posterior a d2.
 */
int compararDatas(Data d1, Data d2) {
    if (d1.ano != d2.ano) return (d1.ano < d2.ano) ? -1 : 1;
    if (d1.mes != d2.mes) return (d1.mes < d2.mes) ? -1 : 1;
    if (d1.dia != d2.dia) return (d1.dia < d2.dia) ? -1 : 1;
    return 0;
}

/*
 * Função: obterDataAtual
 * Objetivo: Consultar o relógio do sistema operacional via <time.h> e retornar
 * a data atual do computador (usado para detectar empréstimos em atraso).
 * Retorno: Estrutura Data preenchida com dia, mês e ano atuais.
 */
Data obterDataAtual(void) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    Data hoje;
    hoje.dia = tm_info->tm_mday;
    hoje.mes = tm_info->tm_mon + 1;
    hoje.ano = tm_info->tm_year + 1900;
    return hoje;
}

/* ============================================================================
 * FUNÇÕES DE MANIPULAÇÃO DE ARQUIVOS E BACKUP
 * ============================================================================ */

/*
 * Função: salvarDados
 * Objetivo: Gravar todos os registros de livros, usuários e empréstimos da
 * memória para os arquivos texto principais ("livros.txt", "usuarios.txt" e
 * "emprestimos.txt"), separando os campos por ponto e vírgula (';').
 * Parâmetro: sys - Ponteiro constante para o sistema (somente leitura).
 */
void salvarDados(const SistemaBiblioteca *sys) {
    FILE *fLivros = fopen(ARQ_LIVROS, "w");
    if (fLivros != NULL) {
        fprintf(fLivros, "%d\n", sys->qtdLivros);
        for (int i = 0; i < sys->qtdLivros; i++) {
            fprintf(fLivros, "%d;%s;%s;%s;%d;%d;%s;%d\n",
                    sys->livros[i].codigo,
                    sys->livros[i].titulo,
                    sys->livros[i].autor,
                    sys->livros[i].editora,
                    sys->livros[i].anoPublicacao,
                    sys->livros[i].exemplaresDisponiveis,
                    sys->livros[i].status,
                    sys->livros[i].totalEmprestimos);
        }
        fclose(fLivros);
    } else {
        printf("[Erro] Nao foi possivel salvar %s\n", ARQ_LIVROS);
    }

    FILE *fUsuarios = fopen(ARQ_USUARIOS, "w");
    if (fUsuarios != NULL) {
        fprintf(fUsuarios, "%d\n", sys->qtdUsuarios);
        for (int i = 0; i < sys->qtdUsuarios; i++) {
            fprintf(fUsuarios, "%d;%s;%s;%s;%d;%d;%d\n",
                    sys->usuarios[i].matricula,
                    sys->usuarios[i].nome,
                    sys->usuarios[i].curso,
                    sys->usuarios[i].telefone,
                    sys->usuarios[i].dataCadastro.dia,
                    sys->usuarios[i].dataCadastro.mes,
                    sys->usuarios[i].dataCadastro.ano);
        }
        fclose(fUsuarios);
    } else {
        printf("[Erro] Nao foi possivel salvar %s\n", ARQ_USUARIOS);
    }

    FILE *fEmp = fopen(ARQ_EMPRESTIMOS, "w");
    if (fEmp != NULL) {
        fprintf(fEmp, "%d\n", sys->qtdEmprestimos);
        for (int i = 0; i < sys->qtdEmprestimos; i++) {
            fprintf(fEmp, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%s\n",
                    sys->emprestimos[i].codigo,
                    sys->emprestimos[i].matriculaUsuario,
                    sys->emprestimos[i].codigoLivro,
                    sys->emprestimos[i].dataEmprestimo.dia,
                    sys->emprestimos[i].dataEmprestimo.mes,
                    sys->emprestimos[i].dataEmprestimo.ano,
                    sys->emprestimos[i].dataPrevistaDevolucao.dia,
                    sys->emprestimos[i].dataPrevistaDevolucao.mes,
                    sys->emprestimos[i].dataPrevistaDevolucao.ano,
                    sys->emprestimos[i].status);
        }
        fclose(fEmp);
    } else {
        printf("[Erro] Nao foi possivel salvar %s\n", ARQ_EMPRESTIMOS);
    }
}

/*
 * Função: carregarDados
 * Objetivo: Ler os arquivos texto ("livros.txt", "usuarios.txt" e "emprestimos.txt")
 * na inicialização do programa e preencher os vetores da memória. Caso os arquivos
 * ainda não existam, ignora silenciosamente e inicia com o sistema vazio.
 * Parâmetro: sys - Ponteiro para a estrutura principal onde os dados serão carregados.
 */
void carregarDados(SistemaBiblioteca *sys) {
    FILE *fLivros = fopen(ARQ_LIVROS, "r");
    if (fLivros != NULL) {
        if (fscanf(fLivros, "%d\n", &sys->qtdLivros) == 1) {
            for (int i = 0; i < sys->qtdLivros && i < MAX_LIVROS; i++) {
                fscanf(fLivros, "%d;%100[^;];%80[^;];%60[^;];%d;%d;%14[^;];%d\n",
                       &sys->livros[i].codigo,
                       sys->livros[i].titulo,
                       sys->livros[i].autor,
                       sys->livros[i].editora,
                       &sys->livros[i].anoPublicacao,
                       &sys->livros[i].exemplaresDisponiveis,
                       sys->livros[i].status,
                       &sys->livros[i].totalEmprestimos);
            }
        }
        fclose(fLivros);
    }

    FILE *fUsuarios = fopen(ARQ_USUARIOS, "r");
    if (fUsuarios != NULL) {
        if (fscanf(fUsuarios, "%d\n", &sys->qtdUsuarios) == 1) {
            for (int i = 0; i < sys->qtdUsuarios && i < MAX_USUARIOS; i++) {
                fscanf(fUsuarios, "%d;%100[^;];%50[^;];%15[^;];%d;%d;%d\n",
                       &sys->usuarios[i].matricula,
                       sys->usuarios[i].nome,
                       sys->usuarios[i].curso,
                       sys->usuarios[i].telefone,
                       &sys->usuarios[i].dataCadastro.dia,
                       &sys->usuarios[i].dataCadastro.mes,
                       &sys->usuarios[i].dataCadastro.ano);
            }
        }
        fclose(fUsuarios);
    }

    FILE *fEmp = fopen(ARQ_EMPRESTIMOS, "r");
    if (fEmp != NULL) {
        if (fscanf(fEmp, "%d\n", &sys->qtdEmprestimos) == 1) {
            for (int i = 0; i < sys->qtdEmprestimos && i < MAX_EMPRESTIMOS; i++) {
                fscanf(fEmp, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%14[^\n]\n",
                       &sys->emprestimos[i].codigo,
                       &sys->emprestimos[i].matriculaUsuario,
                       &sys->emprestimos[i].codigoLivro,
                       &sys->emprestimos[i].dataEmprestimo.dia,
                       &sys->emprestimos[i].dataEmprestimo.mes,
                       &sys->emprestimos[i].dataEmprestimo.ano,
                       &sys->emprestimos[i].dataPrevistaDevolucao.dia,
                       &sys->emprestimos[i].dataPrevistaDevolucao.mes,
                       &sys->emprestimos[i].dataPrevistaDevolucao.ano,
                       sys->emprestimos[i].status);
            }
        }
        fclose(fEmp);
    }
}

/*
 * Função: copiarArquivo
 * Objetivo: Copiar o conteúdo byte a byte de um arquivo de origem para um
 * arquivo de destino (função auxiliar para a rotina de backup).
 * Parâmetros: origem - Nome do arquivo original; destino - Nome do arquivo de cópia.
 */
void copiarArquivo(const char *origem, const char *destino) {
    FILE *src = fopen(origem, "r");
    if (src == NULL) return;

    FILE *dst = fopen(destino, "w");
    if (dst == NULL) {
        fclose(src);
        return;
    }

    int ch;
    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dst);
    }

    fclose(src);
    fclose(dst);
}

/*
 * Função: realizarBackup
 * Objetivo: Gerar cópias de segurança atualizadas dos três arquivos de dados
 * ("livros_backup.txt", "usuarios_backup.txt" e "emprestimos_backup.txt").
 */
void realizarBackup(void) {
    copiarArquivo(ARQ_LIVROS, BKP_LIVROS);
    copiarArquivo(ARQ_USUARIOS, BKP_USUARIOS);
    copiarArquivo(ARQ_EMPRESTIMOS, BKP_EMPRESTIMOS);
}

/* ============================================================================
 * FUNÇÕES DE BUSCA POR ÍNDICE (AUXILIARES)
 * ============================================================================ */

/*
 * Função: buscarIndiceLivroPorCodigo
 * Objetivo: Localizar a posição (índice) de um livro no vetor a partir do seu código.
 * Parâmetros: sys - Ponteiro constante para o sistema; codigo - Código buscado.
 * Retorno: Índice no vetor (0 a qtdLivros-1) se encontrado, ou -1 se não existir.
 */
int buscarIndiceLivroPorCodigo(const SistemaBiblioteca *sys, int codigo) {
    for (int i = 0; i < sys->qtdLivros; i++) {
        if (sys->livros[i].codigo == codigo) return i;
    }
    return -1;
}

/*
 * Função: buscarIndiceUsuarioPorMatricula
 * Objetivo: Localizar a posição (índice) de um usuário no vetor pela matrícula.
 * Parâmetros: sys - Ponteiro constante para o sistema; matricula - Matrícula buscada.
 * Retorno: Índice no vetor (0 a qtdUsuarios-1) se encontrado, ou -1 se não existir.
 */
int buscarIndiceUsuarioPorMatricula(const SistemaBiblioteca *sys, int matricula) {
    for (int i = 0; i < sys->qtdUsuarios; i++) {
        if (sys->usuarios[i].matricula == matricula) return i;
    }
    return -1;
}

/*
 * Função: buscarIndiceEmprestimoPorCodigo
 * Objetivo: Localizar a posição (índice) de um empréstimo no vetor pelo código.
 * Parâmetros: sys - Ponteiro constante para o sistema; codigo - Código do empréstimo.
 * Retorno: Índice no vetor (0 a qtdEmprestimos-1) se encontrado, ou -1 se não existir.
 */
int buscarIndiceEmprestimoPorCodigo(const SistemaBiblioteca *sys, int codigo) {
    for (int i = 0; i < sys->qtdEmprestimos; i++) {
        if (sys->emprestimos[i].codigo == codigo) return i;
    }
    return -1;
}

/* ============================================================================
 * FUNÇÕES MODULARES PRINCIPAIS (CADASTROS, EMPRÉSTIMOS E PESQUISAS)
 * ============================================================================ */

/*
 * Função: cadastrarLivro
 * Objetivo: Solicitar os dados de um novo livro, validar código único, ano e
 * quantidade de exemplares, definir status inicial e salvar/fazer backup.
 * Parâmetro: sys - Ponteiro para a estrutura do sistema que será modificada.
 */
void cadastrarLivro(SistemaBiblioteca *sys) {
    if (sys->qtdLivros >= MAX_LIVROS) {
        printf("\n[Erro] Limite maximo de livros atingido!\n");
        return;
    }

    Livro novo;
    printf("\n--- Cadastro de Livro ---\n");
    printf("Codigo do livro (inteiro positivo): ");
    if (scanf("%d", &novo.codigo) != 1 || novo.codigo <= 0) {
        limparBuffer();
        printf("[Erro] Codigo invalido!\n");
        return;
    }
    limparBuffer();

    if (buscarIndiceLivroPorCodigo(sys, novo.codigo) != -1) {
        printf("[Erro] Ja existe um livro cadastrado com o codigo %d.\n", novo.codigo);
        return;
    }

    printf("Titulo (ate 100 caracteres): ");
    lerString(novo.titulo, sizeof(novo.titulo));
    if (strlen(novo.titulo) == 0) {
        printf("[Erro] O titulo nao pode ser vazio.\n");
        return;
    }

    printf("Autor (ate 80 caracteres): ");
    lerString(novo.autor, sizeof(novo.autor));

    printf("Editora (ate 60 caracteres): ");
    lerString(novo.editora, sizeof(novo.editora));

    printf("Ano de publicacao: ");
    if (scanf("%d", &novo.anoPublicacao) != 1 || novo.anoPublicacao < 1000 || novo.anoPublicacao > 2100) {
        limparBuffer();
        printf("[Erro] Ano de publicacao invalido!\n");
        return;
    }
    limparBuffer();

    printf("Numero de exemplares disponiveis: ");
    if (scanf("%d", &novo.exemplaresDisponiveis) != 1 || novo.exemplaresDisponiveis < 0) {
        limparBuffer();
        printf("[Erro] Quantidade de exemplares invalida!\n");
        return;
    }
    limparBuffer();

    if (novo.exemplaresDisponiveis > 0) {
        strcpy(novo.status, "disponivel");
    } else {
        strcpy(novo.status, "emprestado");
    }
    novo.totalEmprestimos = 0;

    sys->livros[sys->qtdLivros++] = novo;
    salvarDados(sys);
    realizarBackup();
    printf("\n[Sucesso] Livro '%s' cadastrado e salvo!\n", novo.titulo);
}

/*
 * Função: cadastrarUsuario
 * Objetivo: Solicitar os dados de um novo leitor, validar matrícula única,
 * nome não vazio e data de cadastro válida no calendário, salvando em disco.
 * Parâmetro: sys - Ponteiro para a estrutura do sistema que será modificada.
 */
void cadastrarUsuario(SistemaBiblioteca *sys) {
    if (sys->qtdUsuarios >= MAX_USUARIOS) {
        printf("\n[Erro] Limite maximo de usuarios atingido!\n");
        return;
    }

    Usuario novo;
    printf("\n--- Cadastro de Usuario ---\n");
    printf("Matricula (inteiro positivo): ");
    if (scanf("%d", &novo.matricula) != 1 || novo.matricula <= 0) {
        limparBuffer();
        printf("[Erro] Matricula invalida!\n");
        return;
    }
    limparBuffer();

    if (buscarIndiceUsuarioPorMatricula(sys, novo.matricula) != -1) {
        printf("[Erro] Ja existe um usuario com a matricula %d.\n", novo.matricula);
        return;
    }

    printf("Nome completo (ate 100 caracteres): ");
    lerString(novo.nome, sizeof(novo.nome));
    if (strlen(novo.nome) == 0) {
        printf("[Erro] O nome nao pode ser vazio.\n");
        return;
    }

    printf("Curso (ate 50 caracteres): ");
    lerString(novo.curso, sizeof(novo.curso));

    printf("Telefone (ate 15 caracteres): ");
    lerString(novo.telefone, sizeof(novo.telefone));

    printf("Data de cadastro (DD MM AAAA): ");
    if (scanf("%d %d %d", &novo.dataCadastro.dia, &novo.dataCadastro.mes, &novo.dataCadastro.ano) != 3 ||
        !dataValida(novo.dataCadastro)) {
        limparBuffer();
        printf("[Erro] Data invalida!\n");
        return;
    }
    limparBuffer();

    sys->usuarios[sys->qtdUsuarios++] = novo;
    salvarDados(sys);
    realizarBackup();
    printf("\n[Sucesso] Usuario '%s' cadastrado e salvo!\n", novo.nome);
}

/*
 * Função: realizarEmprestimo
 * Objetivo: Registrar o empréstimo de um livro para um usuário. Verifica se
 * ambos existem, se há exemplares disponíveis, calcula a devolução para 7 dias
 * após a data informada, decrementa o estoque e salva os dados.
 * Parâmetro: sys - Ponteiro para a estrutura do sistema que será modificada.
 */
void realizarEmprestimo(SistemaBiblioteca *sys) {
    if (sys->qtdEmprestimos >= MAX_EMPRESTIMOS) {
        printf("\n[Erro] Limite maximo de registros de emprestimo atingido!\n");
        return;
    }

    int matUsuario, codLivro;
    printf("\n--- Realizar Emprestimo ---\n");
    printf("Matricula do usuario: ");
    if (scanf("%d", &matUsuario) != 1) {
        limparBuffer();
        printf("[Erro] Matricula invalida!\n");
        return;
    }
    limparBuffer();

    int idxUser = buscarIndiceUsuarioPorMatricula(sys, matUsuario);
    if (idxUser == -1) {
        printf("[Erro] Usuario com matricula %d nao encontrado.\n", matUsuario);
        return;
    }

    printf("Codigo do livro: ");
    if (scanf("%d", &codLivro) != 1) {
        limparBuffer();
        printf("[Erro] Codigo invalido!\n");
        return;
    }
    limparBuffer();

    int idxLivro = buscarIndiceLivroPorCodigo(sys, codLivro);
    if (idxLivro == -1) {
        printf("[Erro] Livro com codigo %d nao encontrado.\n", codLivro);
        return;
    }

    if (sys->livros[idxLivro].exemplaresDisponiveis <= 0) {
        printf("[Aviso] O livro '%s' nao possui exemplares disponiveis no momento.\n", sys->livros[idxLivro].titulo);
        return;
    }

    Emprestimo novo;
    novo.codigo = (sys->qtdEmprestimos == 0) ? 1 : sys->emprestimos[sys->qtdEmprestimos - 1].codigo + 1;
    novo.matriculaUsuario = matUsuario;
    novo.codigoLivro = codLivro;

    printf("Data do emprestimo (DD MM AAAA): ");
    if (scanf("%d %d %d", &novo.dataEmprestimo.dia, &novo.dataEmprestimo.mes, &novo.dataEmprestimo.ano) != 3 ||
        !dataValida(novo.dataEmprestimo)) {
        limparBuffer();
        printf("[Erro] Data invalida!\n");
        return;
    }
    limparBuffer();

    novo.dataPrevistaDevolucao = somarDias(novo.dataEmprestimo, 7);
    strcpy(novo.status, "ativo");

    /* Atualiza o estoque e o histórico de popularidade do livro */
    sys->livros[idxLivro].exemplaresDisponiveis--;
    sys->livros[idxLivro].totalEmprestimos++;
    if (sys->livros[idxLivro].exemplaresDisponiveis == 0) {
        strcpy(sys->livros[idxLivro].status, "emprestado");
    }

    sys->emprestimos[sys->qtdEmprestimos++] = novo;
    salvarDados(sys);
    realizarBackup();

    printf("\n[Sucesso] Emprestimo #%d registrado!\n", novo.codigo);
    printf("Usuario: %s | Livro: %s\n", sys->usuarios[idxUser].nome, sys->livros[idxLivro].titulo);
    printf("Devolucao prevista para: %02d/%02d/%04d\n",
           novo.dataPrevistaDevolucao.dia,
           novo.dataPrevistaDevolucao.mes,
           novo.dataPrevistaDevolucao.ano);
}

/*
 * Função: realizarDevolucao
 * Objetivo: Dar baixa em um empréstimo ativo pelo seu código, alterando o
 * status para "devolvido" e devolvendo 1 exemplar ao estoque do livro.
 * Parâmetro: sys - Ponteiro para a estrutura do sistema que será modificada.
 */
void realizarDevolucao(SistemaBiblioteca *sys) {
    int codEmp;
    printf("\n--- Realizar Devolucao ---\n");
    printf("Informe o codigo do emprestimo: ");
    if (scanf("%d", &codEmp) != 1) {
        limparBuffer();
        printf("[Erro] Codigo invalido!\n");
        return;
    }
    limparBuffer();

    int idxEmp = buscarIndiceEmprestimoPorCodigo(sys, codEmp);
    if (idxEmp == -1) {
        printf("[Erro] Emprestimo #%d nao encontrado.\n", codEmp);
        return;
    }

    if (strcmp(sys->emprestimos[idxEmp].status, "devolvido") == 0) {
        printf("[Aviso] Este emprestimo ja foi devolvido anteriormente.\n");
        return;
    }

    strcpy(sys->emprestimos[idxEmp].status, "devolvido");

    int idxLivro = buscarIndiceLivroPorCodigo(sys, sys->emprestimos[idxEmp].codigoLivro);
    if (idxLivro != -1) {
        sys->livros[idxLivro].exemplaresDisponiveis++;
        strcpy(sys->livros[idxLivro].status, "disponivel");
    }

    salvarDados(sys);
    realizarBackup();
    printf("\n[Sucesso] Devolucao do emprestimo #%d registrada com exito!\n", codEmp);
}

/*
 * Função: pesquisarLivros
 * Objetivo: Consultar livros cadastrados por código exato, parte do título,
 * parte do nome do autor ou listar todo o acervo.
 * Parâmetro: sys - Ponteiro constante para o sistema (somente leitura).
 */
void pesquisarLivros(const SistemaBiblioteca *sys) {
    int opcao;
    printf("\n--- Pesquisar Livros ---\n");
    printf("1. Por Codigo\n");
    printf("2. Por Titulo\n");
    printf("3. Por Autor\n");
    printf("4. Listar Todos os Livros\n");
    printf("Escolha: ");
    if (scanf("%d", &opcao) != 1) {
        limparBuffer();
        printf("[Erro] Opcao invalida.\n");
        return;
    }
    limparBuffer();

    int encontrados = 0;
    if (opcao == 1) {
        int cod;
        printf("Digite o codigo do livro: ");
        scanf("%d", &cod);
        limparBuffer();
        int idx = buscarIndiceLivroPorCodigo(sys, cod);
        if (idx != -1) {
            const Livro *l = &sys->livros[idx];
            printf("\n[%d] %s | Autor: %s | Editora: %s (%d) | Disp: %d | Status: %s\n",
                   l->codigo, l->titulo, l->autor, l->editora, l->anoPublicacao, l->exemplaresDisponiveis, l->status);
            encontrados++;
        }
    } else if (opcao == 2 || opcao == 3) {
        char termo[101], campoLower[101];
        printf("Digite o termo de busca: ");
        lerString(termo, sizeof(termo));
        paraMinusculo(termo);

        for (int i = 0; i < sys->qtdLivros; i++) {
            strcpy(campoLower, (opcao == 2) ? sys->livros[i].titulo : sys->livros[i].autor);
            paraMinusculo(campoLower);

            if (strstr(campoLower, termo) != NULL) {
                const Livro *l = &sys->livros[i];
                printf("[%d] %s | Autor: %s | Editora: %s (%d) | Disp: %d | Status: %s\n",
                       l->codigo, l->titulo, l->autor, l->editora, l->anoPublicacao, l->exemplaresDisponiveis, l->status);
                encontrados++;
            }
        }
    } else if (opcao == 4) {
        for (int i = 0; i < sys->qtdLivros; i++) {
            const Livro *l = &sys->livros[i];
            printf("[%d] %s | Autor: %s | Editora: %s (%d) | Disp: %d | Status: %s\n",
                   l->codigo, l->titulo, l->autor, l->editora, l->anoPublicacao, l->exemplaresDisponiveis, l->status);
            encontrados++;
        }
    } else {
        printf("[Aviso] Opcao invalida.\n");
        return;
    }

    if (encontrados == 0) {
        printf("\nNenhum livro encontrado para os criterios informados.\n");
    }
}

/*
 * Função: pesquisarUsuarios
 * Objetivo: Consultar usuários cadastrados por matrícula exata, parte do nome
 * ou exibir a lista completa de leitores.
 * Parâmetro: sys - Ponteiro constante para o sistema (somente leitura).
 */
void pesquisarUsuarios(const SistemaBiblioteca *sys) {
    int opcao;
    printf("\n--- Pesquisar Usuarios ---\n");
    printf("1. Por Matricula\n");
    printf("2. Por Nome\n");
    printf("3. Listar Todos os Usuarios\n");
    printf("Escolha: ");
    if (scanf("%d", &opcao) != 1) {
        limparBuffer();
        printf("[Erro] Opcao invalida.\n");
        return;
    }
    limparBuffer();

    int encontrados = 0;
    if (opcao == 1) {
        int mat;
        printf("Digite a matricula: ");
        scanf("%d", &mat);
        limparBuffer();
        int idx = buscarIndiceUsuarioPorMatricula(sys, mat);
        if (idx != -1) {
            const Usuario *u = &sys->usuarios[idx];
            printf("\nMat: %d | Nome: %s | Curso: %s | Tel: %s | Cadastro: %02d/%02d/%04d\n",
                   u->matricula, u->nome, u->curso, u->telefone,
                   u->dataCadastro.dia, u->dataCadastro.mes, u->dataCadastro.ano);
            encontrados++;
        }
    } else if (opcao == 2) {
        char termo[101], nomeLower[101];
        printf("Digite o nome (ou parte dele): ");
        lerString(termo, sizeof(termo));
        paraMinusculo(termo);

        for (int i = 0; i < sys->qtdUsuarios; i++) {
            strcpy(nomeLower, sys->usuarios[i].nome);
            paraMinusculo(nomeLower);
            if (strstr(nomeLower, termo) != NULL) {
                const Usuario *u = &sys->usuarios[i];
                printf("Mat: %d | Nome: %s | Curso: %s | Tel: %s | Cadastro: %02d/%02d/%04d\n",
                       u->matricula, u->nome, u->curso, u->telefone,
                       u->dataCadastro.dia, u->dataCadastro.mes, u->dataCadastro.ano);
                encontrados++;
            }
        }
    } else if (opcao == 3) {
        for (int i = 0; i < sys->qtdUsuarios; i++) {
            const Usuario *u = &sys->usuarios[i];
            printf("Mat: %d | Nome: %s | Curso: %s | Tel: %s | Cadastro: %02d/%02d/%04d\n",
                   u->matricula, u->nome, u->curso, u->telefone,
                   u->dataCadastro.dia, u->dataCadastro.mes, u->dataCadastro.ano);
            encontrados++;
        }
    } else {
        printf("[Aviso] Opcao invalida.\n");
        return;
    }

    if (encontrados == 0) {
        printf("\nNenhum usuario encontrado.\n");
    }
}

/*
 * Função: listarEmprestimosAtivos
 * Objetivo: Percorrer o vetor de empréstimos e exibir apenas aqueles com
 * status "ativo", cruzando dados para mostrar o nome do usuário e título do livro.
 * Parâmetro: sys - Ponteiro constante para o sistema (somente leitura).
 */
void listarEmprestimosAtivos(const SistemaBiblioteca *sys) {
    printf("\n--- Emprestimos Ativos ---\n");
    int ativos = 0;
    for (int i = 0; i < sys->qtdEmprestimos; i++) {
        if (strcmp(sys->emprestimos[i].status, "ativo") == 0) {
            const Emprestimo *e = &sys->emprestimos[i];
            int idxU = buscarIndiceUsuarioPorMatricula(sys, e->matriculaUsuario);
            int idxL = buscarIndiceLivroPorCodigo(sys, e->codigoLivro);

            const char *nomeU = (idxU != -1) ? sys->usuarios[idxU].nome : "Desconhecido";
            const char *titL = (idxL != -1) ? sys->livros[idxL].titulo : "Desconhecido";

            printf("Emp #%d | Usuario: %s (Mat: %d) | Livro: %s (Cod: %d) | Emp: %02d/%02d/%04d | Dev. Prevista: %02d/%02d/%04d\n",
                   e->codigo, nomeU, e->matriculaUsuario, titL, e->codigoLivro,
                   e->dataEmprestimo.dia, e->dataEmprestimo.mes, e->dataEmprestimo.ano,
                   e->dataPrevistaDevolucao.dia, e->dataPrevistaDevolucao.mes, e->dataPrevistaDevolucao.ano);
            ativos++;
        }
    }
    if (ativos == 0) {
        printf("Nenhum emprestimo ativo no momento.\n");
    }
}

/* ============================================================================
 * FUNÇÕES AVANÇADAS (RELATÓRIOS, BUSCA MULTICRITÉRIO E RENOVAÇÃO)
 * ============================================================================ */

/*
 * Função: relatorioLivrosMaisEmprestados
 * Objetivo: Criar uma cópia temporária do vetor de livros, ordená-la em ordem
 * decrescente pelo campo totalEmprestimos (Bubble Sort) sem alterar o vetor
 * original e exibir o ranking dos livros mais retirados.
 * Parâmetro: sys - Ponteiro constante para o sistema (somente leitura).
 */
void relatorioLivrosMaisEmprestados(const SistemaBiblioteca *sys) {
    if (sys->qtdLivros == 0) {
        printf("\nNenhum livro cadastrado no sistema.\n");
        return;
    }

    Livro copia[MAX_LIVROS];
    for (int i = 0; i < sys->qtdLivros; i++) {
        copia[i] = sys->livros[i];
    }

    for (int i = 0; i < sys->qtdLivros - 1; i++) {
        for (int j = 0; j < sys->qtdLivros - i - 1; j++) {
            if (copia[j].totalEmprestimos < copia[j + 1].totalEmprestimos) {
                Livro temp = copia[j];
                copia[j] = copia[j + 1];
                copia[j + 1] = temp;
            }
        }
    }

    printf("\n--- Relatorio: Livros Mais Emprestados ---\n");
    for (int i = 0; i < sys->qtdLivros; i++) {
        printf("%d lugar: [%d] %s - Autor: %s (Total de emprestimos: %d)\n",
               i + 1, copia[i].codigo, copia[i].titulo, copia[i].autor, copia[i].totalEmprestimos);
    }
}

/*
 * Função: relatorioUsuariosComAtraso
 * Objetivo: Comparar a data prevista de devolução dos empréstimos ativos com a
 * data atual do sistema operacional e listar os usuários com devolução vencida.
 * Parâmetro: sys - Ponteiro constante para o sistema (somente leitura).
 */
void relatorioUsuariosComAtraso(const SistemaBiblioteca *sys) {
    Data hoje = obterDataAtual();
    printf("\n--- Relatorio: Usuarios com Emprestimos em Atraso (Ref: %02d/%02d/%04d) ---\n",
           hoje.dia, hoje.mes, hoje.ano);

    int atrasados = 0;
    for (int i = 0; i < sys->qtdEmprestimos; i++) {
        const Emprestimo *e = &sys->emprestimos[i];
        if (strcmp(e->status, "ativo") == 0 && compararDatas(e->dataPrevistaDevolucao, hoje) < 0) {
            int idxU = buscarIndiceUsuarioPorMatricula(sys, e->matriculaUsuario);
            int idxL = buscarIndiceLivroPorCodigo(sys, e->codigoLivro);

            const char *nomeU = (idxU != -1) ? sys->usuarios[idxU].nome : "Desconhecido";
            const char *telU = (idxU != -1) ? sys->usuarios[idxU].telefone : "N/A";
            const char *titL = (idxL != -1) ? sys->livros[idxL].titulo : "Desconhecido";

            printf("Emp #%d | Usuario: %s (Mat: %d | Tel: %s) | Livro: %s | Venceu em: %02d/%02d/%04d\n",
                   e->codigo, nomeU, e->matriculaUsuario, telU, titL,
                   e->dataPrevistaDevolucao.dia, e->dataPrevistaDevolucao.mes, e->dataPrevistaDevolucao.ano);
            atrasados++;
        }
    }

    if (atrasados == 0) {
        printf("Nenhum usuario com emprestimo em atraso encontrado.\n");
    }
}

/*
 * Função: buscaAvancadaLivros
 * Objetivo: Filtrar o acervo combinando até 4 critérios simultâneos: autor,
 * editora, ano mínimo de publicação e disponibilidade em estoque. Critérios
 * deixados em branco (ou 0) são ignorados.
 * Parâmetro: sys - Ponteiro constante para o sistema (somente leitura).
 */
void buscaAvancadaLivros(const SistemaBiblioteca *sys) {
    char filtroAutor[81], filtroEditora[61];
    int anoMin, apenasDisponiveis;

    printf("\n--- Busca Avancada de Livros (Multiplos Criterios) ---\n");
    printf("Autor (deixe vazio + ENTER para ignorar): ");
    lerString(filtroAutor, sizeof(filtroAutor));
    paraMinusculo(filtroAutor);

    printf("Editora (deixe vazio + ENTER para ignorar): ");
    lerString(filtroEditora, sizeof(filtroEditora));
    paraMinusculo(filtroEditora);

    printf("Ano minimo de publicacao (0 para ignorar): ");
    if (scanf("%d", &anoMin) != 1) {
        limparBuffer();
        anoMin = 0;
    }
    limparBuffer();

    printf("Apenas disponiveis? (1 = Sim, 0 = Todos): ");
    if (scanf("%d", &apenasDisponiveis) != 1) {
        limparBuffer();
        apenasDisponiveis = 0;
    }
    limparBuffer();

    printf("\nResultados da Busca Avancada:\n");
    int encontrados = 0;
    for (int i = 0; i < sys->qtdLivros; i++) {
        const Livro *l = &sys->livros[i];
        char autorLower[81], editoraLower[61];
        strcpy(autorLower, l->autor);
        strcpy(editoraLower, l->editora);
        paraMinusculo(autorLower);
        paraMinusculo(editoraLower);

        if (strlen(filtroAutor) > 0 && strstr(autorLower, filtroAutor) == NULL) continue;
        if (strlen(filtroEditora) > 0 && strstr(editoraLower, filtroEditora) == NULL) continue;
        if (anoMin > 0 && l->anoPublicacao < anoMin) continue;
        if (apenasDisponiveis == 1 && l->exemplaresDisponiveis <= 0) continue;

        printf("[%d] %s | Autor: %s | Editora: %s | Ano: %d | Disp: %d\n",
               l->codigo, l->titulo, l->autor, l->editora, l->anoPublicacao, l->exemplaresDisponiveis);
        encontrados++;
    }

    if (encontrados == 0) {
        printf("Nenhum livro atende a todos os criterios selecionados.\n");
    }
}

/*
 * Função: renovarEmprestimo
 * Objetivo: Estender por mais 7 dias a data prevista de devolução de um
 * empréstimo ativo, bloqueando a operação caso o empréstimo já esteja vencido.
 * Parâmetro: sys - Ponteiro para a estrutura do sistema que será modificada.
 */
void renovarEmprestimo(SistemaBiblioteca *sys) {
    int codEmp;
    printf("\n--- Renovacao de Emprestimo ---\n");
    printf("Informe o codigo do emprestimo ativo: ");
    if (scanf("%d", &codEmp) != 1) {
        limparBuffer();
        printf("[Erro] Codigo invalido!\n");
        return;
    }
    limparBuffer();

    int idxEmp = buscarIndiceEmprestimoPorCodigo(sys, codEmp);
    if (idxEmp == -1) {
        printf("[Erro] Emprestimo #%d nao encontrado.\n", codEmp);
        return;
    }

    Emprestimo *e = &sys->emprestimos[idxEmp];
    if (strcmp(e->status, "ativo") != 0) {
        printf("[Erro] Apenas emprestimos ativos podem ser renovados.\n");
        return;
    }

    Data hoje = obterDataAtual();
    if (compararDatas(e->dataPrevistaDevolucao, hoje) < 0) {
        printf("[Bloqueado] Emprestimo em atraso (venceu em %02d/%02d/%04d). Realize a devolucao antes.\n",
               e->dataPrevistaDevolucao.dia, e->dataPrevistaDevolucao.mes, e->dataPrevistaDevolucao.ano);
        return;
    }

    e->dataPrevistaDevolucao = somarDias(e->dataPrevistaDevolucao, 7);
    salvarDados(sys);
    realizarBackup();

    printf("\n[Sucesso] Emprestimo #%d renovado por mais 7 dias!\n", e->codigo);
    printf("Nova data prevista de devolucao: %02d/%02d/%04d\n",
           e->dataPrevistaDevolucao.dia, e->dataPrevistaDevolucao.mes, e->dataPrevistaDevolucao.ano);
}

/* ============================================================================
 * FUNÇÕES DE SUBMENUS (CONTROLE DE FLUXO)
 * ============================================================================ */

/*
 * Função: menuLivros
 * Objetivo: Exibir e controlar o fluxo do submenu de gerenciamento de livros.
 * Parâmetro: sys - Ponteiro para a estrutura principal do sistema.
 */
void menuLivros(SistemaBiblioteca *sys) {
    int op;
    do {
        printf("\n--- MENU DE LIVROS ---\n");
        printf("1. Cadastrar Novo Livro\n");
        printf("2. Pesquisar Livros (Codigo, Titulo, Autor ou Listar)\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha: ");
        if (scanf("%d", &op) != 1) {
            limparBuffer();
            continue;
        }
        limparBuffer();

        switch (op) {
            case 1: cadastrarLivro(sys); break;
            case 2: pesquisarLivros(sys); break;
            case 0: break;
            default: printf("[Aviso] Opcao invalida.\n");
        }
    } while (op != 0);
}

/*
 * Função: menuUsuarios
 * Objetivo: Exibir e controlar o fluxo do submenu de gerenciamento de usuários.
 * Parâmetro: sys - Ponteiro para a estrutura principal do sistema.
 */
void menuUsuarios(SistemaBiblioteca *sys) {
    int op;
    do {
        printf("\n--- MENU DE USUARIOS ---\n");
        printf("1. Cadastrar Novo Usuario\n");
        printf("2. Pesquisar Usuarios (Matricula, Nome ou Listar)\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha: ");
        if (scanf("%d", &op) != 1) {
            limparBuffer();
            continue;
        }
        limparBuffer();

        switch (op) {
            case 1: cadastrarUsuario(sys); break;
            case 2: pesquisarUsuarios(sys); break;
            case 0: break;
            default: printf("[Aviso] Opcao invalida.\n");
        }
    } while (op != 0);
}

/*
 * Função: menuEmprestimos
 * Objetivo: Exibir e controlar o fluxo do submenu de empréstimos, devoluções,
 * listagem de ativos e renovação de prazos.
 * Parâmetro: sys - Ponteiro para a estrutura principal do sistema.
 */
void menuEmprestimos(SistemaBiblioteca *sys) {
    int op;
    do {
        printf("\n--- MENU DE EMPRESTIMOS E DEVOLUCOES ---\n");
        printf("1. Realizar Emprestimo\n");
        printf("2. Realizar Devolucao\n");
        printf("3. Listar Emprestimos Ativos\n");
        printf("4. Renovar Emprestimo (+7 dias)\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha: ");
        if (scanf("%d", &op) != 1) {
            limparBuffer();
            continue;
        }
        limparBuffer();

        switch (op) {
            case 1: realizarEmprestimo(sys); break;
            case 2: realizarDevolucao(sys); break;
            case 3: listarEmprestimosAtivos(sys); break;
            case 4: renovarEmprestimo(sys); break;
            case 0: break;
            default: printf("[Aviso] Opcao invalida.\n");
        }
    } while (op != 0);
}

/*
 * Função: menuRelatorios
 * Objetivo: Exibir e controlar o fluxo do submenu de relatórios gerenciais e
 * busca avançada multicritério.
 * Parâmetro: sys - Ponteiro para a estrutura principal do sistema.
 */
void menuRelatorios(SistemaBiblioteca *sys) {
    int op;
    do {
        printf("\n--- MENU DE RELATORIOS E BUSCA AVANCADA ---\n");
        printf("1. Relatorio de Livros Mais Emprestados\n");
        printf("2. Relatorio de Usuarios com Emprestimos em Atraso\n");
        printf("3. Busca Avancada de Livros (Multiplos Criterios)\n");
        printf("0. Voltar ao Menu Principal\n");
        printf("Escolha: ");
        if (scanf("%d", &op) != 1) {
            limparBuffer();
            continue;
        }
        limparBuffer();

        switch (op) {
            case 1: relatorioLivrosMaisEmprestados(sys); break;
            case 2: relatorioUsuariosComAtraso(sys); break;
            case 3: buscaAvancadaLivros(sys); break;
            case 0: break;
            default: printf("[Aviso] Opcao invalida.\n");
        }
    } while (op != 0);
}