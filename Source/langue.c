#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//test

#include "data.h"
#include "logic.h"
#include "interface.h"

#define NB_TEXTES 20

typedef struct {
    char cle[32];
    char valeur[64];
} Texte;

static Texte textes[NB_TEXTES];

void charger_langue(const char* chemin) {
    FILE* fichier = fopen(chemin, "r");
    if (!fichier) {
        perror("Erreur ouverture fichier langue");
        exit(1);
    }

    char ligne[128];
    int i = 0;

    while (fgets(ligne, sizeof(ligne), fichier) && i < NB_TEXTES) {
        char* egal = strchr(ligne, '=');
        if (egal) {
            *egal = '\0';
            strcpy(textes[i].cle, ligne);
            strcpy(textes[i].valeur, egal + 1);
            textes[i].valeur[strcspn(textes[i].valeur, "\n")] = '\0'; // supprime \n
            i++;
        }
    }

    fclose(fichier);
}

const char* get_texte(const char* cle) {
    for (int i = 0; i < NB_TEXTES; i++) {
        if (strcmp(textes[i].cle, cle) == 0)
            return textes[i].valeur;
    }
    return "[manquant]";
}
