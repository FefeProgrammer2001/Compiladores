#include <functional>
#include <iostream>
#include <map>
#include <string>
#include "mini.hpp"

Resultado analisaSemantica(const Pilha<Token>& posfixa) {
    using Op = std::function<double(double, double)>;

    static const std::map<char, Op> operacoes = {
        {'+', [](double a, double b) { return a + b; }},
        {'-', [](double a, double b) { return a - b; }},
        {'*', [](double a, double b) { return a * b; }},
        {'/', [](double a, double b) {
            if(b == 0) throw ErroSemantico("divisao por zero");
            return a / b;
        }},
    };

    auto infereTipo = [](const Token& a, const Token& b, char op) {
        return(a.tnum == REAL || b.tnum == REAL || op == '/') ? REAL : INTEIRO;
    };

    auto passo = [&](Pilha<Token> pilha, const Token& t) {
        if(t.tipo == OPERANDO) {
            pilha.empilhar(t);
        } else if(t.tipo == OPERADOR) {
            auto b = pilha.desempilhar();
            auto a = pilha.desempilhar();
            if(!a || !b)
                throw ErroSemantico(std::string("operandos insuficientes para '") + t.u.op + "'");
            
            auto it = operacoes.find(t.u.op);
            if(it == operacoes.end())
                throw ErroSemantico(std::string("operador desconhecido '") + t.u.op + "'");
            
            double r = it->second(a->u.valor, b->u.valor);
            pilha.empilhar(criaOperando(r, infereTipo(*a, *b, t.u.op)));
        } else {
            throw ErroSemantico("token inesperado na posfixa");
        }
        return pilha;
    };

    Pilha<Token> final = posfixa.reduz(Pilha<Token>(), passo);

    if(final.tamanho() != 1)
        throw ErroSemantico("expressao malformada (" + std::to_string(final.tamanho()) + " valores restantes)");
    
    Token res = *final.topo();
    return Resultado{res.u.valor, res.tnum};
}

int main() {
    std::string entrada;
    std::cout << "Digite a expressao (numeros, + - * / e parenteses):\n> ";
    if(!std::getline(std::cin, entrada)) return 1;

    try {
        buffer = entrada.c_str();

        tokeniza();
        mostraTokens();

        analisaSintatico();
        std::cout << "Expressao sintaticamente valida\n";
        std::cout << "Posfixa: ";
        imprimir(saida);
        std::cout << "Quantidade de tokens: " << saida.tamanho() << '\n';

        Resultado r = analisaSemantica(saida);
        std::cout << "Expressao semanticamente valida\n";
        std::cout << "Tipo do resultado: " << (r.tipo == INTEIRO ? "inteiro" : "real") << '\n';
        std::cout << "Resultado: " << r.valor << '\n';
    } catch(const ErroMini& e) {
        std::cout << e.what() << '\n';
        return 1;
    }
    return 0;
}
