#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 256

int validar(const char *s) {
    int i = 0;
    char c;
    int tipo;

q0:
    while(s[i] == ' ') i++;
    if(strncmp(&s[i], "int", 3) == 0 && s[i+3] == ' ') {
        tipo = 0;
        i += 3;
        goto q_esp1;
    }
    else if(strncmp(&s[i], "float", 5) == 0 && s[i+5] == ' ') {
        tipo = 1;
        i += 5;
        goto q_esp1;
    }
    else {
        goto rejeita;
    }

q_esp1:
    if(s[i] != ' ') goto rejeita;
    while(s[i] == ' ') i++;
    goto q_id;

q_id:
    c = s[i];
    if(isalpha((unsigned char)c) || c == '_') {
        i++;
        goto q_id_resto;
    }
    goto rejeita;

q_id_resto:
    c = s[i];
    if(isalnum((unsigned char)c) || c == '_') {
        i++;
        goto q_id_resto;
    }
    while(s[i] == ' ') i++;
    goto q_igual;

q_igual:
    if(s[i] != '=') goto rejeita;
    i++;
    while(s[i] == ' ') i++;
    goto q_num;

q_num:
    if(!isdigit((unsigned char)s[i])) goto rejeita;
    while(isdigit((unsigned char)s[i])) i++;

    if(tipo == 0) {
        goto q_fim;
    } else {
        if(s[i] != '.') goto rejeita;
        i++;
        if(!isdigit((unsigned char)s[i])) goto rejeita;
        while(isdigit((unsigned char)s[i])) i++;
        goto q_fim;
    }

q_fim:
    while(s[i] == ' ') i++;
    if(s[i] == '\0') goto aceita;
    goto rejeita;

aceita:
    return 1;

rejeita:
    return 0;

}

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Uso: %s <nome_arquivo>\n", argv[0]);
        return -1;
    }

    char *nome_arquivo = argv[1];
    FILE *fp = fopen(nome_arquivo, "r");
    char linha[MAX];
    int numLinha = 1;

    if(fp == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo\n");
        return -1;
    }

    while(fgets(linha, MAX, fp) != NULL) {
        linha[strcspn(linha, "\r\n")] = '\0';

        if(linha[0] == '\0') continue;

        int resultado = validar(linha);

        printf("Linha %d: \"%s\" -> %d (%s)\n", numLinha, linha, resultado, resultado ? "aceito" : "rejeitado");

        numLinha++;
    }

    fclose(fp);
    return 0;
}
