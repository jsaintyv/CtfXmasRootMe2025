#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file;
    char ch;

    // Ouvre le fichier en mode lecture
    file = fopen("/app/flag.txt", "r");

    // Vérifie si le fichier a été ouvert avec succès
    if (file == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier.\n");
        return 1;
    }

    // Lit et affiche le contenu du fichier caractère par caractère
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    // Ferme le fichier
    fclose(file);

    return 0;
}
