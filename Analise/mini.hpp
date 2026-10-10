#ifndef MINI_HPP
#define MINI_HPP

#include <functional>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct ErroMini : std::runtime_error {
    using std::runtime_error::runtime_error;
};
struct ErroLexico : ErroMini {
    explicit ErroLexico(const std::string& m) : ErroMini("Erro lexico: " + m) {}
};
struct ErroSintatico : ErroMini {
    explicit ErroSintatico(const std::string& m) : ErroMini("Erro sintatico: " + m) {}
};
struct ErroSemantico : ErroMini {
    explicit ErroSemantico(const std::string& m) : ErroMini("Erro semantico: " + m) {}
};

inline std::string fmt(double v) {
    std::ostringstream o;
    o << v;
    return o.str();
}

enum TipoToken { OPERANDO, OPERADOR, ABRE_PAR, FECHA_PAR, FIM };
enum TipoNum { INTEIRO, REAL };

struct Token {
    TipoToken tipo;
    TipoNum tnum;
    union {
        double valor;
        char op;
    } u;
};

template <typename T>
class Pilha {
    T* dados;
    int capacidade;
    int topo_;

    void cresce() {
        int nova = capacidade ? capacidade * 2 : 16;
        T* novo = new T[nova];
        for(int i = 0; i <= topo_; i++) novo[i] = dados[i];
        delete[] dados;
        dados = novo;
        capacidade = nova;
    }

public:
    explicit Pilha(int cap = 16) : dados(new T[cap]), capacidade(cap), topo_(-1) {}

    Pilha(const Pilha& o) : dados(new T[o.capacidade]), capacidade(o.capacidade), topo_(o.topo_) {
        for(int i = 0; i <= topo_; i++) dados[i] = o.dados[i];
    }

    Pilha(Pilha&& o) noexcept : dados(o.dados), capacidade(o.capacidade), topo_(o.topo_) {
        o.dados = nullptr;
        o.capacidade = 0;
        o.topo_ = -1;
    }

    Pilha& operator=(Pilha o) {
        std::swap(dados, o.dados);
        std::swap(capacidade, o.capacidade);
        std::swap(topo_, o.topo_);
        return *this;
    }

    ~Pilha() { delete[] dados; }

    bool vazia() const { return topo_ == -1; }
    int tamanho() const { return topo_ + 1; }

    void empilhar(const T& v) {
        if(topo_ + 1 >= capacidade) cresce();
        dados[++topo_] = v;
    }

    std::optional<T> desempilhar() {
        if(vazia()) return std::nullopt;
        return dados[topo_--];
    }

    std::optional<T> topo() const {
        if(vazia()) return std::nullopt;
        return dados[topo_];
    }

    void paraCada(const std::function<void(const T&)>& f) const {
        for(int i = 0; i <= topo_; i++) f(dados[i]);
    }

    template <typename R, typename F>
    R reduz(R ini, F f) const {
        for(int i = 0; i <= topo_; i++) ini = f(std::move(ini), dados[i]);
        return ini;
    }
};

extern const char* buffer;
extern std::vector<Token> tokens;
extern int pos;
extern Pilha<Token> saida;

Token criaOperando(double v, TipoNum tn);
Token criaOperador(char c);
Token criaSimbolo(TipoToken tp, char c);
Token criaFim();
void imprimirToken(const Token& t);
void imprimir(const Pilha<Token>& p);
void tokeniza();
void mostraTokens();

char lookahead();
void consome(char c);
void E();
void E_linha();
void T();
void T_linha();
void F();
void analisaSintatico();

struct Resultado {
    double valor;
    TipoNum tipo;
};
Resultado analisaSemantica(const Pilha<Token>& posfixa);

#endif
