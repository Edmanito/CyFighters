
// Include/logic.h
#ifndef LOGIC_H
#define LOGIC_H

#include <stdio.h>
#include <string.h>

#include "data.h"

#define NB_PERSOS_EQUIPE 3

/*
typedef struct {
    Fighter* persos[NB_PERSOS_EQUIPE];
} Equipe;

typedef struct {
    Equipe      equipe1;
    Equipe      equipe2;
    ElementType element_map;
    int         id_map;
} Partie;


// Équipes
void  initialiser_equipe         (Equipe* equipe);
int   ajouter_personnage_equipe  (Equipe* equipe, Fighter* perso);
void  afficher_equipe            (const Equipe* equipe);

// Sélection
void  selectionner_personnages   (Equipe* equipe);

// Initialisation et gameplay
void  initialiser_jeu            (Equipe* e1, Equipe* e2);
void  debuter_jeu                (void);
float calculer_vitesse_moyenne   (const Equipe* equipe);

// Maps
MapType generer_map_aleatoire    (void);
void    afficher_nom_map         (MapType map);
*/
#endif // LOGIC_H
