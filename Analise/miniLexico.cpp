#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include "mini.hpp"

const char* buffer = nullptr;
std::vector<Token> tokens;

Token criaOperando(double v, TipoNum tn) {
    Token t; t.tipo = OPERANDO; t.tnum = tn; t.u.valor = v; return t;
}
Token criaOperador(char c) {
    Token t; t.tipo = OPERADOR; t.tnum = INTEIRO; t.u.op = c; return t;
}
Token criaSimbolo(TipoToken tp, char c) {
    Token t; t.tipo = tp; t.tnum = INTEIRO; t.u.op = c; return t;
}
Token criaFim() {
    Token t; t.tipo = FIM; t.tnum = INTEIRO; t.u.op = '\0'; return t;
}

void imprimirToken(const Token& t) {
    if(t.tipo == OPERANDO) std::cout << t.u.valor << ' ';
    else std::cout << t.u.op << ' ';
}

void imprimir(const Pilha<Token>& p) {
    p.paraCada(imprimirToken);
    std::cout << '\n';
}

void tokeniza() {
    auto ehEspaco = [](char c) { return c == ' ' || c == '\t'; };
    auto ehOperador = [](char c) { return c == '+' || c == '-' || c == '*' || c == '/'; };
    auto iniciaNum = [](char c) { return std::isdigit(static_cast<unsigned char>(c)) || c == '.'; };

    tokens.clear();
    while(*buffer != '\0') {
        char c = *buffer;

        if(ehEspaco(c)) {
            buffer++;
        } else if(iniciaNum(c)) {
            char* fim;
            double v = std::strtod(buffer, &fim);
            if(fim == buffer)
                throw ErroLexico(std::string("numero invalido em '") + buffer + "'");
            bool real = std::any_of(buffer, static_cast<const char*>(fim), [](char ch) { return ch == '.'; });
            tokens.push_back(criaOperando(v, real ? REAL : INTEIRO));
            buffer = fim;
        } else if(ehOperador(c)) {
            tokens.push_back(criaOperador(c));
            buffer++;
        } else if(c == '(') {
            tokens.push_back(criaSimbolo(ABRE_PAR, c));
            buffer++;
        } else if(c == ')') {
            tokens.push_back(criaSimbolo(FECHA_PAR, c));
            buffer++;
        } else {
            throw ErroLexico(std::string("caractere invalido '") + c + "'");
        }
    }
    tokens.push_back(criaFim());
}

void mostraTokens() {
    std::cout << "Tokens:\n";

    std::for_each(tokens.begin(), tokens.end(), [](const Token& t) {
        switch(t.tipo) {
            case OPERANDO:
                std::cout << "  OPERANDO  " << t.u.valor << " (" << (t.tnum == INTEIRO ? "inteiro" : "real") << ")\n"; break;
            case OPERADOR: std::cout << "  OPERADOR  " << t.u.op << '\n'; break;
            case ABRE_PAR: std::cout << "  ABRE_PAR  (\n"; break;
            case FECHA_PAR: std::cout << " FECHA_PAR )\n"; break;
            case FIM: std::cout << "  FIM\n"; break;
        }
    });
}

