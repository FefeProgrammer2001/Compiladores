#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX 256

typedef enum { OPERANDO, OPERADOR, ABRE_PAR, FECHA_PAR, FIM } TipoToken;

typedef struct {
    TipoToken tipo;
    union {
        double valor;
        char op;
    } u;
} Token;

/* ----- Pilha sequencial ----- */
typedef struct {
    Token dados[MAX];
    int topo;
} Pilha;

void inicializa(Pilha *p) { p->topo = -1; }
int vazia(Pilha *p) { return p->topo == -1; }
int cheia(Pilha *p) { return p->topo == MAX - 1; }
int tamanho(Pilha *p) { return p->topo + 1; }

int empilhar(Pilha *p, Token t) {
    if(cheia(p)) return 0;
    p->dados[++p->topo] = t;
    return 1;
}

int desempilhar(Pilha *p, Token *t) {
    if(vazia(p)) return 0;
    *t = p->dados[p->topo--];
    return 1;
}

int topo(Pilha *p, Token *t) {
    if(vazia(p)) return 0;
    *t = p->dados[p->topo];
    return 1;
}

void imprimirToken(Token t) {
    if(t.tipo == OPERANDO) printf("%g ", t.u.valor);
    else                   printf("%c ", t.u.op);
}

void imprimir(Pilha *p) {
    for(int i = 0; i <= p->topo; i++)
        imprimirToken(p->dados[i]);
    printf("\n");
}

Token criaOperando(double v) { Token t; t.tipo = OPERANDO; t.u.valor = v; return t; }
Token criaOperador(char c) { Token t; t.tipo = OPERADOR; t.u.op = c; return t; }
Token criaSimbolo(TipoToken tp, char c) { Token t; t.tipo = tp; t.u.op = c; return t; }
Token criaFim(void) { Token t; t.tipo = FIM; t.u.op = '\0'; return t; }

char *buffer;
Pilha saida;

Token tokens[MAX + 1];
int ntokens = 0;
int pos = 0;

/* ----- Mini Léxico -----*/
void erro(const char *msg);

void tokeniza(void) {
    ntokens = 0;
    while(*buffer != '\0') {
        char c = *buffer;

        if(c == ' ' || c == '\t') {
            buffer++;
        } else if(isdigit((unsigned char)c) || c == '.') {
            char *fim;
            double v = strtod(buffer, &fim);
            if(fim == buffer) {
                printf("Erro lexico, numero invalido em '%s'\n", buffer);
                exit(1);
            }
            tokens[ntokens++] = criaOperando(v);
            buffer = fim;
        } else if(c == '+' || c == '-' || c == '*' || c == '/') {
            tokens[ntokens++] = criaOperador(c);
            buffer++;
        } else if(c == '(') {
            tokens[ntokens++] = criaSimbolo(ABRE_PAR, c);
            buffer++;
        } else if(c == ')') {
            tokens[ntokens++] = criaSimbolo(FECHA_PAR, c);
            buffer++;
        } else {
            printf("Erro lexico: caractere invalido '%c'\n", c);
            exit(1);
        }
    }
    tokens[ntokens++] = criaFim();
}

void mostraTokens(void) {
    printf("Tokens:\n");
    for(int i = 0; i < ntokens; i++) {
        Token t = tokens[i];
        switch(t.tipo) {
            case OPERANDO:  printf("  OPERANDO  %g\n", t.u.valor); break;
            case OPERADOR:  printf("  OPERADOR  %c\n", t.u.op);    break;
            case ABRE_PAR:  printf("  ABRE_PAR  (\n");             break;
            case FECHA_PAR: printf("  FECHA_PAR )\n");             break;
            case FIM:       printf("  FIM\n");                     break;
        }
    }
}

/* ----- Parser (lê o vetor de tokens) ----- */
char lookahead(void);
void consome(char c);

void E(void);
void E_linha(void);
void T(void);
void T_linha(void);
void F(void);

char lookahead(void) {
    Token t = tokens[pos];
    switch(t.tipo) {
        case OPERANDO: return 'n';
        case FIM:      return '\0';
        default:       return t.u.op;
    }
}

void consome(char c) {
    if(lookahead() == c) {
        if(tokens[pos].tipo != FIM) pos++;
    } else {
        char obtido = lookahead();
        if(obtido == '\0')
            printf("Erro: esperando '%c', mas a expressao terminou\n", c);
        else if(obtido == 'n')
            printf("Erro: esperando '%c', encontrado numero %g\n", c, tokens[pos].u.valor);
        else
            printf("Erro: esperando '%c', encontrado '%c'\n", c, obtido);
        exit(1);
    }
}

void erro(const char *msg) {
    printf("Erro sintatico: %s\n", msg);
    exit(1);
}

void E(void) { T(); E_linha(); }

void E_linha(void) {
    char c = lookahead();
    if(c == '+' || c == '-') {
        consome(c);
        T();
        empilhar(&saida, criaOperador(c));
        E_linha();
    }
}

void T(void) { F(); T_linha(); }

void T_linha(void) {
    char c = lookahead();
    if(c == '*' || c == '/') {
        consome(c);
        F();
        empilhar(&saida, criaOperador(c));
        T_linha();
    }
}

void F(void) {
    char c = lookahead();
    if(c == '(') {
        consome('(');
        E();
        consome(')');
    } else if(c == 'n') {
        empilhar(&saida, criaOperando(tokens[pos].u.valor));
        pos++;
    } else {
        erro("esperando numero ou '('");
    }
}

int main(void) {
    char entrada[MAX];

    printf("Digite a expressao (digitos, + - * / e parenteses):\n> ");
    if(!fgets(entrada, sizeof(entrada), stdin)) return 1;

    for(char *p = entrada; *p; p++)
        if(*p == '\n') { *p = '\0'; break; }

    inicializa(&saida);
    buffer = entrada;

    tokeniza(); /* Fase léxica */
    mostraTokens();

    pos = 0;
    E(); /* Fase sintática */

    if(lookahead() != '\0') {
        printf("Erro: simbolo inesperado '%c'\n", lookahead() == 'n' ? '#' : lookahead());
        return 1;
    }

    printf("Expressao valida!\n");
    printf("Posfixa: ");
    imprimir(&saida);
    printf("Quantidade de tokens: %d\n", tamanho(&saida));

    Token t;
    if(topo(&saida, &t)) {
        printf("Topo da pilha: ");
        imprimirToken(t);
        printf("\n");
    }
    return 0;
}
