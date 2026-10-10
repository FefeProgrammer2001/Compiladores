#include "mini.hpp"

int pos = 0;
Pilha<Token> saida;

char lookahead() {
    const Token& t = tokens[pos];
    switch(t.tipo) {
        case OPERANDO: return 'n';
        case FIM: return '\0';
        default: return t.u.op;
    }
}

void consome(char c) {
    char obtido = lookahead();
    if(obtido == c) {
        if(tokens[pos].tipo != FIM) pos++;
        return;
    }
    std::string esperado(1, c);
    if(obtido == '\0')
        throw ErroSintatico("esperando '" + esperado + "', mas a expressao terminou");
    if(obtido == 'n')
        throw ErroSintatico("esperado '" + esperado + "', encontrado numero " + fmt(tokens[pos].u.valor));
    throw ErroSintatico("esperando '" + esperado + "', encontrado '" + std::string(1, obtido) + "'");
}

void E() { T(); E_linha(); }

void E_linha() {
    char c = lookahead();
    if(c == '+' || c == '-') {
        consome(c);
        T();
        saida.empilhar(criaOperador(c));
        E_linha();
    }
}

void T() { F(); T_linha(); }

void T_linha() {
    char c = lookahead();
    if(c == '*' || c == '/') {
        consome(c);
        F();
        saida.empilhar(criaOperador(c));
        T_linha();
    }
}

void F() {
    char c = lookahead();
    if(c == '(') {
        consome('(');
        E();
        consome(')');
    } else if(c == 'n') {
        saida.empilhar(tokens[pos]);
        pos++;
    } else {
        throw ErroSintatico("esperando numero ou '('");
    }
}

void analisaSintatico() {
    pos = 0;
    saida = Pilha<Token>();
    E();
    char c = lookahead();
    if(c == 'n')
        throw ErroSintatico("numero inesperado " + fmt(tokens[pos].u.valor));
    if(c != '\0')
        throw ErroSintatico(std::string("simbolo inesperado '") + c + "'");
}
