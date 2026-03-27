#include <stdio.h>

int main() {
    int age;

    printf("Bonjour, bienvenue au guichet.\n");
    printf("Quel age avez-vous ? ");
    scanf("%d", &age);

    if (age < 0) {
        printf("Age invalide.\n");
    } else if (age < 6) {
        printf("Votre ticket est gratuit.\n");
    } else if (age <= 10) {
        printf("Vous avez droit a un tarif reduit (50%%).\n");
    } else {
        printf("Vous payez le tarif normal.\n");
    }

    printf("Merci et bonne visite.\n");

    return 0;
}
