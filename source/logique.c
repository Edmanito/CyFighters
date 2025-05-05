/*
// ----- src/logique.c -----
#include "data.h"
#include "logic.h"
#include "interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Initialise une équipe vide
void initialiser_equipe(Equipe* equipe) {
    for (int i = 0; i < NB_PERSOS_EQUIPE; i++)
        equipe->persos[i] = NULL;
}

// Ajoute un perso à la première place libre ; retourne 1 si OK, 0 sinon
int ajouter_personnage_equipe(Equipe* equipe, Fighter* perso) {
    for (int i = 0; i < NB_PERSOS_EQUIPE; i++) {
        if (equipe->persos[i] == NULL) {
            equipe->persos[i] = perso;
            return 1;
        }
    }
    return 0;
}

// Affiche les stats de tous les persos de l’équipe
void afficher_equipe(const Equipe* equipe) {
    printf("=== Équipe ===\n");
    for (int i = 0; i < NB_PERSOS_EQUIPE; i++) {
        Fighter* p = equipe->persos[i];
        if (p) {
            printf("%d) %s  | PV %d/%d  | ATT %d  | DEF %d  | VIT %d  | AGI %d  | Élé %d\n",
                   i+1, p->nom, p->actu_pv, p->max_pv,
                   p->attaque, p->defense, p->vitesse, p->agilite, p->element);
        }
    }
    printf("==============\n");
}

// Interaction console pour choisir NB_PERSOS_EQUIPE persos
// ----- src/logique.c -----
void selectionner_personnages(Equipe* equipe) {
    int choix;
    initialiser_equipe(equipe);

    printf("Sélectionnez %d personnages pour votre équipe :\n", NB_PERSOS_EQUIPE);
    for (int i = 0; i < NB_PERSOS_EQUIPE; i++) {
        // Affiche la liste de tous les persos disponibles
        printf("\nListe des personnages disponibles :\n");
        for (int j = 0; j < 8; j++) {
            printf("%d - %s\n", j, personnage[j].nom);
        }

        // Lecture du choix
        printf("Entrez le numéro du personnage %d : ", i + 1);
        scanf("%d", &choix);
        while (choix < 0 || choix >= 8
               || !ajouter_personnage_equipe(equipe, &personnage[choix])) {
            printf("Choix invalide, recommencez : ");
            scanf("%d", &choix);
        }

        // Optionnel : afficher l'équipe actuelle
        // afficher_equipe(equipe);
    }
}


// Initialise une partie : on choisit 2 équipes et on applique les buffs de map
void initialiser_jeu(Equipe* e1, Equipe* e2) {
    selectionner_personnages(e1);
    selectionner_personnages(e2);
    MapType map = generer_map_aleatoire();
    BonusMap b1 = obtenir_bonus_map(map, e1->persos[0]->element);
    BonusMap b2 = obtenir_bonus_map(map, e2->persos[0]->element);
    // Appliquer buffs à tous
    for (int i = 0; i < NB_PERSOS_EQUIPE; i++) {
        appliquer_buffs(e1->persos[i], b1);
        appliquer_buffs(e2->persos[i], b2);
    }
    afficher_nom_map(map);
}

// Boucle principale du jeu (simplifié)
void debuter_jeu(void) {
    printf("Début du combat !\n");
    // … Votre logique de tour par tour …
}

// Vitesse moyenne d'une équipe
float calculer_vitesse_moyenne(const Equipe* equipe) {
    float somme = 0.0f;
    for (int i = 0; i < NB_PERSOS_EQUIPE; i++)
        somme += equipe->persos[i]->vitesse;
    return somme / NB_PERSOS_EQUIPE;
}

// Tirage aléatoire d'une map
MapType generer_map_aleatoire(void) {
    static int seed_init = 0;
    if (!seed_init) {
        srand((unsigned)time(NULL));
        seed_init = 1;
    }
    return (MapType)(rand() % NB_MAPS);
}

// Affiche le nom de la map tirée
void afficher_nom_map(MapType map) {
    static const char* noms[NB_MAPS] = {
        "Cristal", "Glace", "Feu", "Éclair", "Vent",
        "Eau", "Ombre", "Neutre", "Temps", "Mystère"
    };
    if (map >= 0 && map < NB_MAPS)
        printf("Map générée : %s\n", noms[map]);
    else
        printf("Map inconnue (%d)\n", map);
}



*/