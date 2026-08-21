#include <stdio.h>

int contarCaracteres(FILE *arquivo) {
    int c = fgetc(arquivo);

    if(c == EOF) { // Caso base
        return 0;
    }

    return 1 + contarCaracteres(arquivo); // Recursão para contar caracteres
}

int main() {
    FILE *arquivo = fopen("teste.txt", "r");

    if(arquivo == NULL) {
        fprintf(stderr, "Erro ao ler o arquivo\n");
        return -1;
    }

    int total = contarCaracteres(arquivo);
    printf("Total de caracteres: %d\n", total);
    fclose(arquivo);
    return 0;
}
