# 📚 Sistema de Gerenciamento de Biblioteca em C

Sistema de linha de comando (CLI) desenvolvido exclusivamente em **linguagem C padrão** para o gerenciamento completo do acervo de uma biblioteca, controle de usuários, empréstimos, devoluções, relatórios gerenciais e persistência automática de dados em arquivos de texto.

## 📋 Índice

1. [Funcionalidades](#-funcionalidades)
2. [Pré-requisitos](#-pré-requisitos)
3. [Como Compilar e Executar](#-como-compilar-e-executar)
4. [Manual de Utilização](#-manual-de-utilização)
   * [Menu Principal](#menu-principal)
   * [1. Gerenciar Livros](#1-gerenciar-livros)
   * [2. Gerenciar Usuários](#2-gerenciar-usuários)
   * [3. Gerenciar Empréstimos e Devoluções](#3-gerenciar-empréstimos-e-devoluções)
   * [4. Relatórios e Busca Avançada](#4-relatórios-e-busca-avançada)
5. [Persistência e Backup Automático](#-persistência-e-backup-automático)
6. [Estrutura de Arquivos](#-estrutura-de-arquivos)
7. [Autor](#-autor)

---

## ✨ Funcionalidades

* **Gestão de Acervo:** Cadastro de livros com validação de código único, controle automático de status (`disponivel`/`emprestado`) e pesquisa por código, título ou autor.
* **Gestão de Leitores:** Cadastro de usuários com validação de matrícula única, validação de calendário (incluindo anos bissextos) e busca por matrícula ou nome.
* **Controle de Circulação:** Empréstimos com cálculo automático de devolução para **7 dias corridos**, devolução com baixa no estoque, listagem de pendências ativas e sistema de renovação (+7 dias) com bloqueio para itens em atraso.
* **Relatórios e Filtros:** Ranking de livros mais emprestados, relatório de usuários inadimplentes baseado na data atual do sistema operacional e busca avançada com múltiplos critérios combinados.
* **Segurança de Dados:** Carregamento automático na inicialização, gravação contínua em `.txt` e geração automática de arquivos de backup.

---

## 🛠️ Pré-requisitos

* Compilador C compatível com o padrão C99 ou superior (ex.: **GCC**, **Clang** ou **MinGW** no Windows).
* Utiliza apenas bibliotecas padrão da linguagem C (`stdio.h`, `stdlib.h`, `string.h`, `ctype.h`, `time.h`), não exigindo dependências externas.

---

## 🚀 Como Compilar e Executar

1. Abra o terminal na pasta raiz do projeto onde se encontra o arquivo `biblioteca.c`.
2. Compile o código-fonte executando:

```bash
gcc biblioteca.c -o biblioteca -Wall
```

3. Execute o programa gerado:

* **Linux / macOS:**

```bash
./biblioteca
```

* **Windows (CMD / PowerShell):**

```cmd
biblioteca.exe
```

> **Nota:** Na primeira execução, caso os arquivos de dados ainda não existam, o sistema iniciará com os registros vazios e criará os arquivos `.txt` automaticamente assim que o primeiro cadastro for concluído.

---

## 📖 Manual de Utilização

### Menu Principal

Ao iniciar o sistema, o menu principal apresentará as seguintes opções:

| Opção | Funcionalidade | Descrição |
| :--- | :--- | :--- |
| **1** | Gerenciar Livros | Abre o submenu para cadastrar e pesquisar obras no acervo. |
| **2** | Gerenciar Usuários | Abre o submenu para cadastrar e consultar leitores. |
| **3** | Gerenciar Empréstimos e Devoluções | Abre o submenu de movimentação, renovação e consulta de empréstimos. |
| **4** | Relatórios e Busca Avançada | Abre o submenu de relatórios gerenciais e filtros combinados. |
| **5** | Salvar Dados e Gerar Backup Manual | Força a gravação imediata nos arquivos e atualiza os backups. |
| **0** | Sair do Sistema | Salva todas as alterações, atualiza o backup e encerra a aplicação. |

### 1. Gerenciar Livros

* **Cadastrar Novo Livro:**
  * **Código do livro:** Número inteiro positivo e único (códigos já cadastrados são rejeitados).
  * **Título, Autor e Editora:** Campos de texto (até 100, 80 e 60 caracteres, respectivamente). *Evite utilizar ponto e vírgula (`;`), pois esse caractere é usado como separador nos arquivos de dados.*
  * **Ano de publicação:** Número inteiro válido (entre `1000` e `2100`).
  * **Exemplares disponíveis:** Se maior que `0`, o status é definido automaticamente como `disponivel`; se igual a `0`, como `emprestado`.
* **Pesquisar Livros:**
  * Permite buscar por **Código exato**, **Título** (busca parcial, sem diferenciar maiúsculas de minúsculas), **Autor** (busca parcial) ou **Listar todos** os livros do acervo.

### 2. Gerenciar Usuários

* **Cadastrar Novo Usuário:**
  * **Matrícula:** Número inteiro positivo e único.
  * **Nome completo, Curso e Telefone:** Campos de texto (até 100, 50 e 15 caracteres, respectivamente).
  * **Data de cadastro:** Deve ser digitada separada por espaços no formato `DD MM AAAA` (exemplo: `16 11 2026`). O sistema valida a quantidade de dias de cada mês e anos bissextos.
* **Pesquisar Usuários:**
  * Permite consultar leitores pela **Matrícula exata**, por parte do **Nome** ou exibir a lista completa de cadastrados.

### 3. Gerenciar Empréstimos e Devoluções

* **Realizar Empréstimo:**
  * Solicita a **Matrícula do usuário** e o **Código do livro**, validando se ambos existem e se há pelo menos `1` exemplar disponível em estoque.
  * Solicita a **Data do empréstimo** (`DD MM AAAA`) e calcula automaticamente a **Data prevista de devolução somando 7 dias corridos** (ajustando viradas de mês e ano).
  * Gera um código sequencial único para o empréstimo e decrementa o estoque do livro.
* **Realizar Devolução:**
  * Solicita o **Código do empréstimo**, altera seu status para `devolvido` e retorna `1` exemplar ao estoque disponível do respectivo livro.
* **Listar Empréstimos Ativos:**
  * Exibe todos os empréstimos com status `ativo`, apresentando o nome do usuário, título do livro, data de retirada e data limite de devolução.
* **Renovar Empréstimo (+7 dias):**
  * Solicita o **Código do empréstimo ativo**.
  * Se o empréstimo estiver **dentro do prazo** (comparado à data atual do computador), estende a data prevista de devolução em **+7 dias**. Empréstimos já vencidos têm a renovação bloqueada até que a devolução seja efetuada.

### 4. Relatórios e Busca Avançada

* **Relatório de Livros Mais Emprestados:**
  * Lista todo o acervo ordenado de forma decrescente pelo número total de vezes em que cada livro foi emprestado.
* **Relatório de Usuários com Empréstimos em Atraso:**
  * Compara a data prevista de devolução dos empréstimos ativos com o **relógio atual do sistema operacional**, exibindo os leitores com pendências vencidas e seus telefones para contato.
* **Busca Avançada de Livros:**
  * Permite combinar até **4 critérios simultâneos**:
    1. **Autor** (parcial)
    2. **Editora** (parcial)
    3. **Ano mínimo de publicação**
    4. **Disponibilidade** (apenas livros com exemplares disponíveis ou todos)
  * Para ignorar qualquer filtro de texto, basta pressionar `ENTER` com o campo vazio; para ignorar filtros numéricos, digite `0`.

---

## 💾 Persistência e Backup Automático

A cada operação de escrita (cadastro, empréstimo, devolução ou renovação), o sistema executa automaticamente duas rotinas de segurança:

1. **Gravação Principal:** Atualiza os arquivos `livros.txt`, `usuarios.txt` e `emprestimos.txt`.
2. **Backup Automático:** Espelha os dados atualizados para `livros_backup.txt`, `usuarios_backup.txt` e `emprestimos_backup.txt`.

---

## 📂 Estrutura de Arquivos

```text
├── biblioteca.c               # Código-fonte completo do sistema
├── README.md                  # Documentação e manual do usuário
├── livros.txt                 # Base de dados de livros (gerado automaticamente)
├── usuarios.txt               # Base de dados de usuários (gerado automaticamente)
├── emprestimos.txt            # Base de dados de empréstimos (gerado automaticamente)
├── livros_backup.txt          # Cópia de segurança de livros
├── usuarios_backup.txt        # Cópia de segurança de usuários
└── emprestimos_backup.txt     # Cópia de segurança de empréstimos
```

---

## 👤 Autor

* **Pedro de Oliveira**