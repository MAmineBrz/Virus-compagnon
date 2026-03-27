#include <stdio.h>
#include <stdlib.h>

int fact(int n) {
    if (n <= 1) return 1;
    else return n * fact(n - 1);
}

int main() {
    int n;

    printf("Bonjour, besoin de faire une factorielle ?!\nEntrez votre nombre :\n");
    scanf("%d", &n);

    if (n < 0) {
        printf("Erreur : nombre negatif\n");
    } else {
        printf("%d! = %d\n", n, fact(n));
    }

    return 0;
}
